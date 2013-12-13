#include <assert.h>
#include <iostream>
#include <mutex>
#include <condition_variable>

#include <core/module/ModuleLoader.hpp>
#include <core/comm/CommunicationModule.hpp>

#include <knowledge/reasoner/Reasoner.hpp>
#include <knowledge/reasoner/AnomalyPlugin.hpp>

#include <knowledge/reasoner/modules/ReasonerStandardImplementationOptions.hpp>

using namespace std;

using namespace core;
using namespace knowledge;

/*
 Teststubs
 */
class TestAnomalyTrigger : public AnomalyTrigger {
	private:
		mutex                   clock;
		condition_variable      cond;
	public:
		int anomaliesTriggered = 0;

		void clearAnomalyStatus(){

		}

		void triggerResponseForAnomaly(bool anomalyStillOngoing) {
			clock.lock();
			anomaliesTriggered++;
			cond.notify_one();
			clock.unlock();
			cout << "Triggered" << anomaliesTriggered << endl;
		}

		bool waitForAnomalyCount( int i ) {
			while( 1 ) {
				auto timeout = chrono::system_clock::now() + chrono::milliseconds( 30 );

				unique_lock<mutex> lock( clock );
				//cout << "A" <<anomaliesTriggered << endl;
				if( anomaliesTriggered >= i ) {
					//cout << "T" << endl;
					return true;
				}
				if( cond.wait_until( lock, timeout ) == cv_status::timeout ) {
					//cout << "F" << endl;
					return false;
				}
			}
		}
};

class TestQualitativeUtilization : public QualitativeUtilization {
	public:
		uint8_t nextValue = 0;

		uint8_t lastUtilization( monitoring::OntologyAttributeID id ) const throw( NotFoundError ){
			return nextValue;
		}
};

class TestAnomalyPlugin : public AnomalyPlugin {
	public:
		void injectObservation( ComponentID cid, UniqueInterfaceID uid, HealthState state,  const string & issue, int32_t delta_time_ms ) {
			addObservation(cid, uid, state, issue, delta_time_ms);
		}
};

// Contains the last period: struct DetailedProcessHealth

void accumulate(const list<HealthIssue> & src, unordered_map<string, HealthIssue> & tgt){
	for( auto itr = src.begin(); itr != src.end(); itr++ ){
		auto searchResult = tgt.find( itr->name );
		if( searchResult != tgt.end() ){
			HealthIssue & found = searchResult->second;
			found.occurrences += itr->occurrences;
			found.delta_time_ms += itr->delta_time_ms;
		}else{
			tgt[itr->name] = *itr;
		}
	}
}

string toString(HealthState s){
	switch(s){
		case ABNORMAL_SLOW:
			return "ABNORMAL_SLOW";
		case ABNORMAL_FAST:
			return "ABNORMAL_FAST";
		case ABNORMAL_OTHER:
			return "ABNORMAL_OTHER";
		case FAST:
			return "FAST";
		case OK:
			return "OK";
		case SLOW:
			return "SLOW";
		default:
			return "UNKNOWN";
	}	
}

string toString(UtilizationIndex s){
	switch(s){
		case CPU:
			return "CPU";
		case MEMORY:
			return "MEMORY";
		case IO:
			return "IO";
		case NETWORK:
			return "NETWORK";
		default:
			return "UNKNOWN";
	}	
}


void mergeOutput(list<ProcessHealth> & l, Health & health, array<uint8_t, HEALTH_STATE_COUNT> & operationRatio, uint64_t & totalOpCount){
	// aggregate the results of the individual processes
	unordered_map<string, HealthIssue> positiveIssues, negativeIssues;
	for( auto itr = l.begin(); itr != l.end(); itr++ ){
		accumulate( itr->positiveIssues, positiveIssues );
		accumulate( itr->negativeIssues, negativeIssues );
		/* 
			The node health state is the average HealthState across all processes weighted by the number of occurrences.
			=> If some occurrences are fast and others are slow the average is OK.		
		*/
		for ( int i=0; i < HEALTH_STATE_COUNT; i++ ){
			health.occurrences[i] += itr->occurrences[i];
		}
	}

	totalOpCount = 0;
	for ( int i=0; i < HEALTH_STATE_COUNT; i++ ){		
		totalOpCount += health.occurrences[i];		
	}

	if( totalOpCount == 0 ){
		// What are we doing if no operation has been performed at all
		for ( int i=0; i < HEALTH_STATE_COUNT; i++ ){
			operationRatio[i] = 0;
		}
		return;
	}

	for ( int i=0; i < HEALTH_STATE_COUNT; i++ ){
		operationRatio[i] = health.occurrences[i] * 100 / totalOpCount;
		cout << health.occurrences[i] << "=" << (int) operationRatio[i] << "% ";
	}
	cout << endl;
}

void testAssessNodeAggregation(){
	// Input: node statistics, process health, system health	
	// Configuration: global reasoner address, local address
	// Result: node health
	SystemHealth sh = { HealthState::OK, {{0,1,5,1,0,0}}, {}, {} };

	ProcessHealth p1 = { HealthState::SLOW, 	{{0,1,5,5,0,0}}, { {"cache hits", 5, 0} }, { {"cache misses", 4, 0} } };
	ProcessHealth p2 = { HealthState::OK, 		{{0,1,5,1,0,0}}, { { "suboptimal access pattern type 1", 2, 10 }, {"cache hits", 2, -1} }, { {"cache misses", 1, +1} } };
	ProcessHealth p3 = { HealthState::FAST, 	{{0,5,5,1,0,0}}, { { "optimal access pattern type 1", 2, 10 }, {"cache hits", 3, 0} }, { {"cache misses", 3, 0} } };

	list<ProcessHealth> l = {p1, p2, p3};

	NodeHealth nh;

	nh.utilization[UtilizationIndex::CPU] = 20;
	nh.utilization[UtilizationIndex::NETWORK] = 40;
	nh.utilization[UtilizationIndex::IO] = 30;
	nh.utilization[UtilizationIndex::MEMORY] = 10;

	array<uint8_t, HEALTH_STATE_COUNT> operationRatio;
	uint64_t totalOpCount;
	mergeOutput(l, nh, operationRatio, totalOpCount);

	// determine node health based on the historic knowledge AND the statistics
	if ( nh.occurrences[ABNORMAL_FAST] || nh.occurrences[ABNORMAL_SLOW] || nh.occurrences[ABNORMAL_OTHER] ){
		// we have an condition in which we must fire the anomaly trigger.

		if ( operationRatio[ABNORMAL_FAST] > 5 && 
			operationRatio[ABNORMAL_FAST] > 2 * operationRatio[ABNORMAL_SLOW] && 
			operationRatio[ABNORMAL_FAST] > 2 * operationRatio[ABNORMAL_OTHER] )
		{
			nh.overallState = HealthState::ABNORMAL_FAST;
		}else if (operationRatio[ABNORMAL_SLOW] > 5 && 
			operationRatio[ABNORMAL_SLOW] > 2 * operationRatio[ABNORMAL_FAST] && 
			operationRatio[ABNORMAL_SLOW] > 2 * operationRatio[ABNORMAL_OTHER] )
		{
			nh.overallState = HealthState::ABNORMAL_SLOW;
		}else{
			nh.overallState = HealthState::ABNORMAL_OTHER;
		}	
	}else{
		// normal condition
		// We have to pick between FAST, OK & SLOW
		if ( operationRatio[FAST] > 5 && 
			operationRatio[FAST] > 3*operationRatio[SLOW] )
		{
			nh.overallState = HealthState::FAST;
		}else if ( operationRatio[SLOW] > 5 && 
			operationRatio[SLOW] > 3*operationRatio[FAST] )
		{
			nh.overallState = HealthState::SLOW;
		}else{
			nh.overallState = HealthState::OK;
		}
	}

	// If the current state is not OK and a statistics behaves suboptimal, we can take this into account and potentially add new issues and even set the health state to OK.
	if ( nh.overallState != HealthState::OK ){
		uint totalUtilization = 0;

		if ( nh.overallState == HealthState::SLOW || nh.overallState == HealthState::ABNORMAL_SLOW || nh.overallState == HealthState::ABNORMAL_OTHER ){

			for(int i=0; i < UTILIZATION_STATISTIC_COUNT; i++ ){
				totalUtilization += nh.utilization[i];
				if ( nh.utilization[i] > 80 ){ // if more than 80% utilization in one category
					// overloaded!
					nh.negativeIssues.push_back( { toString( (UtilizationIndex) i) + " overloaded", 1, 0 } ); 
					// actually the decision should depend on this nodes role. A compute node is expected to have a high CPU and MEMORY utilization...
					// TODO
					nh.overallState = HealthState::OK;
				}
			}
			if ( totalUtilization > 60*4 ){ // the overall system is overloaded
				// do we really need this? More sophisticated decisions would be good.
			} 
		}
	}

	cout << "State: " << toString(nh.overallState) << endl;
}


void testAssessProcessAggregation(){
	// Input: node health, system health, activity anomalies from the plugins: the AnomalyPluginHealthStatistic

	// Configuration: address of node global reasoner
	// Result: process health (& user information upon process termination)
	/* 
		Detailed process health:
		For EACH layer: (Operation Count, Time lost/won)
			Good (Reason A, Reason B, ...)
			Average (no detailed information needed here)
			Bad (Reason A, Reason B, ...)
		
		The detailed process health table is managed for the whole program run.

		Compact process health (for the last interval) like for node etc.
  	 */
}



void testAssessGlobalAggregation(){
	// Input: node health
	// Configuration: set of nodes belonging to each file system (at the moment we consider only  one file system)
	// Result: system health
	// Test case
	// Three nodes report overlapping issues.
}

void testReasoner(){

	// Obtain a FileOntology instance from module loader
	Reasoner * r = core::module_create_instance<Reasoner>( "", "siox-knowledge-ReasonerStandardImplementation", KNOWLEDGE_REASONER_INTERFACE );

	Reasoner * r2 = core::module_create_instance<Reasoner>( "", "siox-knowledge-ReasonerStandardImplementation", KNOWLEDGE_REASONER_INTERFACE );

	CommunicationModule * comm = core::module_create_instance<CommunicationModule>( "", "siox-core-comm-gio", CORE_COMM_INTERFACE );

	assert(comm != nullptr);
	comm->init();

	assert( r != nullptr );

	TestAnomalyTrigger at1;
	TestAnomalyTrigger at2;
	TestQualitativeUtilization qu;

	TestAnomalyPlugin adpi1;

	{
	ReasonerStandardImplementationOptions & r_options = r->getOptions<ReasonerStandardImplementationOptions>();
	r_options.comm.componentPointer = comm;
	r_options.serviceAddress = "ipc://reasoner1";
	r->init();
	}

	r->connectTrigger( & at1 );
	r->connectTrigger( & at2 );

	r->connectUtilization( & qu );
	r->connectAnomalyPlugin( & adpi1 );

	// in this initial test we have no anomaly:
	assert( at1.waitForAnomalyCount( 0 ) );
	assert( at2.waitForAnomalyCount( 0 ) );


	// init the second reasoner.
	{
	ReasonerStandardImplementationOptions & r_options = r2->getOptions<ReasonerStandardImplementationOptions>();
	r_options.comm.componentPointer = comm;
	r_options.serviceAddress = "ipc://reasoner2";
	r_options.downstreamReasoners.push_back( "ipc://reasoner1" );
	r2->init();
	}

	// Now we inject an observation which will trigger an reaction:
	adpi1.injectObservation( ComponentID{{1}}, 2, HealthState::SLOW, "", 10 );

	assert( at1.waitForAnomalyCount( 1 ) );
	assert( at2.waitForAnomalyCount( 1 ) );

	// Now we will query the reactions:
	shared_ptr<HealthStatistics> stats = r->queryRuntimePerformanceIssues();

	cout << "Anomalies: " << at1.anomaliesTriggered << endl;

	delete(r);
	delete(r2);
	delete(comm);
}

int main( int argc, char const * argv[] )
{
	testAssessNodeAggregation();
	return 0;

	testReasoner();	

	cout << endl << "OK" << endl;
	return 0;
}


