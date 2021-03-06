#include <assert.h>
#include <unistd.h> // For sleep()

#include <iostream>
#include <mutex>
#include <condition_variable>

#include <util/TestHelper.hpp>

#include <core/module/ModuleLoader.hpp>
#include <core/comm/CommunicationModule.hpp>

#include <knowledge/reasoner/Reasoner.hpp>
#include <knowledge/reasoner/AnomalyPlugin.hpp>

#include <knowledge/reasoner/modules/ReasonerStandardImplementation.hpp>
#include <knowledge/reasoner/modules/ReasonerStandardImplementationOptions.hpp>
#include <knowledge/reasoner/modules/ReasoningDatatypesSerializable.hpp>

#include <knowledge/reasoner/modules/ReasonerCommunication.hpp>

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
				auto timeout = chrono::system_clock::now() + chrono::milliseconds( 300000 );

				unique_lock<mutex> lock( clock );
				if( anomaliesTriggered >= i ) {
					return true;
				}
				if( cond.wait_until( lock, timeout ) == cv_status::timeout ) {
					return false;
				}
			}
		}
};

class TestAnomalyPlugin : public AnomalyPlugin {
	public:
		void injectObservation( ComponentID cid, HealthState state,  const string & issue, int32_t delta_time_ms ) {
			addObservation(cid, state, issue, delta_time_ms);
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
	health.timeLastModified = time(0);

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

/*
 * Tests the aggregation of states in the reasoner and its reaction to them.
 */
void testAssessNodeAggregation(){
	cout << endl;
	cout << "Test: ReasonerNodeAggregation" << endl;
	cout << "=============================" << endl;

	// Input: node statistics, process health, system health
	// Configuration: global reasoner address, local address
	// Result: node health
	//SystemHealth sh = { HealthState::OK, {{0,1,5,1,0,0}}, {}, {} };

	ProcessHealth p1 = { HealthState::BAD
, 	{{0,1,5,5,0,0}}, { {"cache hits", 5, 0} }, { {"cache misses", 4, 0} } };
	ProcessHealth p2 = { HealthState::OK, 		{{0,1,5,1,0,0}}, { { "suboptimal access pattern type 1", 2, 10 }, {"cache hits", 2, -1} }, { {"cache misses", 1, +1} } };
	ProcessHealth p3 = { HealthState::GOOD
, 	{{0,5,5,1,0,0}}, { { "optimal access pattern type 1", 2, 10 }, {"cache hits", 3, 0} }, { {"cache misses", 3, 0} } };

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
	if ( nh.occurrences[ABNORMAL_GOOD] || nh.occurrences[ABNORMAL_BAD] || nh.occurrences[ABNORMAL_OTHER] ){
		// we have an condition in which we must fire the anomaly trigger.

		if ( operationRatio[ABNORMAL_GOOD] > 5 &&
			operationRatio[ABNORMAL_GOOD] > 2 * operationRatio[ABNORMAL_BAD] &&
			operationRatio[ABNORMAL_GOOD] > 2 * operationRatio[ABNORMAL_OTHER] )
		{
			nh.overallState = HealthState::ABNORMAL_GOOD;
		}else if (operationRatio[ABNORMAL_BAD] > 5 &&
			operationRatio[ABNORMAL_BAD] > 2 * operationRatio[ABNORMAL_GOOD] &&
			operationRatio[ABNORMAL_BAD] > 2 * operationRatio[ABNORMAL_OTHER] )
		{
			nh.overallState = HealthState::ABNORMAL_BAD;
		}else{
			nh.overallState = HealthState::ABNORMAL_OTHER;
		}
	}else{
		// normal condition
		// We have to pick between GOOD, OK & BAD

		if ( operationRatio[GOOD] > 5 &&
			operationRatio[GOOD] > 3*operationRatio[BAD] )
		{
			nh.overallState = HealthState::GOOD;
		}else if ( operationRatio[BAD] > 5 &&
			operationRatio[BAD] > 3*operationRatio[GOOD] )
		{
			nh.overallState = HealthState::BAD;
		}else{
			nh.overallState = HealthState::OK;
		}
	}

	// If the current state is not OK and a statistics behaves suboptimal, we can take this into account and potentially add new issues and even set the health state to OK.
	if ( nh.overallState != HealthState::OK ){
		uint totalUtilization = 0;

		if ( nh.overallState == HealthState::BAD || nh.overallState == HealthState::ABNORMAL_BAD || nh.overallState == HealthState::ABNORMAL_OTHER ){

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

void testReasonerAssessment(){

	cout << endl;
	cout << "Test: Reasoner Assessment" << endl;
	cout << "=========================" << endl;

	// Input: node statistics, process health, system health
	// Configuration: global reasoner address, local address
	// Result: node health
	//SystemHealth sh = { HealthState::OK, {{0,1,5,1,0,0}}, {}, {} };

	// Obtain some instances from module loader
	Reasoner * r = core::module_create_instance<Reasoner>( "", "siox-knowledge-ReasonerStandardImplementation", KNOWLEDGE_REASONER_INTERFACE );
	CommunicationModule * comm = core::module_create_instance<CommunicationModule>( "", "siox-core-comm-gio", CORE_COMM_INTERFACE );

	assert(comm != nullptr);
	comm->init();

	assert( r != nullptr );
	{
	ReasonerStandardImplementationOptions & r_options = r->getOptions<ReasonerStandardImplementationOptions>();
	r_options.role = Role::NODE;
	r_options.communicationOptions.comm.componentPointer = comm;
	r_options.communicationOptions.serviceAddress = "ipc://reasoner1";
	r->init(); // This will start a separate Reasoner thread
	}
	r->start();
	ProcessHealth p1 = { HealthState::BAD, 	{{0,1,5,5,0,0}}, { {"cache hits", 5, 0} }, { {"cache misses", 4, 0} } };
	ProcessHealth p2 = { HealthState::OK, 		{{0,1,5,1,0,0}}, { { "suboptimal access pattern type 1", 2, 10 }, {"cache hits", 2, -1} }, { {"cache misses", 1, +1} } };
	ProcessHealth p3 = { HealthState::GOOD, 	{{0,5,5,1,0,0}}, { { "optimal access pattern type 1", 2, 10 }, {"cache hits", 3, 0} }, { {"cache misses", 3, 0} } };

	ReasonerMessageReceived rmr1 = {"P1"};
	ReasonerMessageReceived rmr2 = {"P2"};
	ReasonerMessageReceived rmr3 = {"P3"};

	// list<ProcessHealth> l = {p1, p2, p3};

	ReasonerMessageReceived rmrInject("INJECT");// Magic word to overwrite reasoner's own health
	shared_ptr<NodeHealth> nh(new NodeHealth) ;

	nh->utilization[UtilizationIndex::CPU] = 20;
	nh->utilization[UtilizationIndex::NETWORK] = 40;
	nh->utilization[UtilizationIndex::IO] = 30;
	nh->utilization[UtilizationIndex::MEMORY] = 10;

	((ReasonerStandardImplementation *) r)->receivedReasonerNodeHealth(rmrInject, *nh);

	((ReasonerStandardImplementation *) r)->receivedReasonerProcessHealth(rmr1, p1);
	((ReasonerStandardImplementation *) r)->receivedReasonerProcessHealth(rmr2, p2);
	((ReasonerStandardImplementation *) r)->receivedReasonerProcessHealth(rmr3, p3);

	// Now we will query the reactions:
	// shared_ptr<HealthStatistics> stats = r->queryRuntimePerformanceIssues();
	// cout << "HealthStatistics: " << *stats << endl;

	nh = ((ReasonerStandardImplementation *) r)->getNodeHealth();
	assert(nh != nullptr);
	cout << "Node health: " << (*nh) << endl;

	r->stop();
	delete(r);
	delete(comm);
}

/*
 * Tests the interplay of ADPIs, reasoner and anomaly triggers.
 */
void testReasonerAnomalies(){

	cout << endl;
	cout << "Test: Reasoner" << endl;
	cout << "==============" << endl;

	// Obtain some instances from module loader
	Reasoner * r = core::module_create_instance<Reasoner>( "", "siox-knowledge-ReasonerStandardImplementation", KNOWLEDGE_REASONER_INTERFACE );
	CommunicationModule * comm = core::module_create_instance<CommunicationModule>( "", "siox-core-comm-gio", CORE_COMM_INTERFACE );

	assert(comm != nullptr);
	comm->init();

	assert( r != nullptr );

	TestAnomalyTrigger at1;
	TestAnomalyTrigger at2;

	TestAnomalyPlugin adpi1;

	{
	ReasonerStandardImplementationOptions & r_options = r->getOptions<ReasonerStandardImplementationOptions>();
	r_options.communicationOptions.comm.componentPointer = comm;
	r_options.communicationOptions.serviceAddress = "ipc://reasoner1";
	r->init(); // This will start a separate Reasoner thread
	}
	r->start();

	r->connectTrigger( & at1 );
	r->connectTrigger( & at2 );

	r->connectAnomalyPlugin( & adpi1 );

	// in this initial test we have no anomaly:
	assert( at1.waitForAnomalyCount( 0 ) );
	assert( at2.waitForAnomalyCount( 0 ) );

	// Now we inject an observation which will trigger a reaction:
	adpi1.injectObservation( ComponentID{{1}}, HealthState::BAD, "SlownessIssue", 10 );
	adpi1.injectObservation( ComponentID{{1}}, HealthState::BAD, "Slowness Issue", 10 );
	adpi1.injectObservation( ComponentID{{1}}, HealthState::ABNORMAL_BAD, "Standstill Issue", 10 );

	assert( at1.waitForAnomalyCount( 1 ) );
	assert( at2.waitForAnomalyCount( 1 ) );
	// at1.waitForAnomalyCount( 1 );
	// at2.waitForAnomalyCount( 1 );

	// Now we will query the reactions:
	shared_ptr<HealthStatistics> stats = r->queryRuntimePerformanceIssues();
	cout << "HealthStatistics: " << *stats << endl;


	cout << "Anomalies at AT1: " << at1.anomaliesTriggered << endl;
	cout << "Anomalies at AT2: " << at2.anomaliesTriggered << endl;

	r->stop();
	delete(r);
	delete(comm);
}

/*
 This test forges a reasoner message and tries to serialize and deserialize it properly.
 */
void testSerializationOfTypes(){

	cout << endl;
	cout << "Test: ReasonerSerialization" << endl;
	cout << "===========================" << endl;

	ReasonerMessageData rsmd;
	rsmd.containedData = ReasonerMessageDataType::NODE;
	rsmd.reasonerID = "testReasoner";

	NodeHealth nh;
	nh.utilization[UtilizationIndex::CPU] = 20;
	nh.utilization[UtilizationIndex::NETWORK] = 40;
	nh.utilization[UtilizationIndex::IO] = 30;
	nh.utilization[UtilizationIndex::MEMORY] = 10;
	nh.overallState = HealthState::OK;
	nh.occurrences = array<uint32_t,6>{{0,1,5,1,0,0}};
	nh.positiveIssues = {{"test", 2, 10}, {"test2", 3, 3}};

	rsmd.messagePayload = & nh;

	uint64_t serLen = j_serialization::serializeLen(nh) + j_serialization::serializeLen(rsmd);
	char * buffer = (char*) malloc(serLen);
	uint64_t pos = 0;
	j_serialization::serialize(rsmd, buffer, pos);
	j_serialization::serialize( *(NodeHealth*) rsmd.messagePayload, buffer, pos);

	assert( pos == serLen );

	pos = 0;

	NodeHealth deserialized;
	ReasonerMessageData rmsdDeserialized;
	j_serialization::deserialize(rmsdDeserialized, buffer, pos, serLen);
	// usually we would de-serialize based on the data contained
	j_serialization::deserialize(deserialized, buffer, pos, serLen);

	rmsdDeserialized.messagePayload = & deserialized;

	free(buffer);

	cout << "Message size: " << serLen << endl;

	// now check the consistency of the result

	// check that all data is consumed
	assert( pos == serLen );

	// assert( serLen == 87 );	// FIXME: The correct one seems to be 98?

	// check that the message is correctly transmitted
	assert( rmsdDeserialized.containedData == ReasonerMessageDataType::NODE );
	assert( rmsdDeserialized.reasonerID == "testReasoner" );


	assert( deserialized.occurrences == nh.occurrences );
	assert( deserialized.overallState == nh.overallState );
	assert( deserialized.utilization == nh.utilization );
	assert( deserialized.positiveIssues == nh.positiveIssues );
	assert( deserialized.negativeIssues == nh.negativeIssues );
}

class MyReasoningDataReceivedCB : public ReasoningDataReceivedCB, public ProtectRaceConditions{
public:
	ReasonerMessageReceived received_data;
	ProcessHealth received_ph;
	SystemHealth received_sh;
	NodeHealth received_nh;

	shared_ptr<NodeHealth> nh;
	shared_ptr<SystemHealth> sh;
	shared_ptr<ProcessHealth> ph;

	void receivedReasonerProcessHealth(ReasonerMessageReceived & data, ProcessHealth & health){
		received_data = data;
		received_ph = health;
		// cout << "receivedReasonerProcessHealth: " << health.overallState << endl;
		sthHappens();
	}

	void receivedReasonerNodeHealth(ReasonerMessageReceived & data, NodeHealth & health){
		received_data = data;
		received_nh = health;
		// cout << "receivedReasonerNodeHealth: " << health.overallState << endl;
		sthHappens();
	}

	void receivedReasonerSystemHealth(ReasonerMessageReceived & data, SystemHealth & health){
		received_data = data;
		received_sh = health;
		// cout << "receivedReasonerSystemHealth: " << health.overallState << endl;
		sthHappens();
	}

	shared_ptr<NodeHealth> getNodeHealth(){
		return nh;
	}

	shared_ptr<SystemHealth> getSystemHealth(){
		return sh;
	}

	shared_ptr<ProcessHealth> getProcessHealth(){
		return ph;
	}

	MyReasoningDataReceivedCB() : received_data("Undefined"){
		// fill dummy data
		{
		NodeHealth * h = new NodeHealth();
		nh = shared_ptr<NodeHealth>(h);
		}
		{
		SystemHealth * h = new SystemHealth();
		sh = shared_ptr<SystemHealth>(h);
		}
		{
		ProcessHealth * h = new ProcessHealth();
		ph = shared_ptr<ProcessHealth>(h);
		}
	}
};


/*
 Test the communication infrastructure of the reasoner.
 Create two communication endpoints, r is the global reasoner endpoint,
 push actively data from r2 upstream to r.
 */
void testReasonerCommunicationRaw(){

	cout << endl;
	cout << "Test: ReasonerCommunicationRaw" << endl;
	cout << "==============================" << endl;

	CommunicationModule * comm = core::module_create_instance<CommunicationModule>( "", "siox-core-comm-gio", CORE_COMM_INTERFACE );

	assert(comm != nullptr);
	comm->init();

	{
		MyReasoningDataReceivedCB mCB1;
		MyReasoningDataReceivedCB mCB2;

		ReasonerCommunication r1(mCB1);
		ReasonerCommunication r2(mCB2);

		{
		ReasonerCommunicationOptions o;
		o.comm.componentPointer = comm;
		o.serviceAddress = "ipc://reasoner1";
		o.reasonerID = "global";
		r1.init(o);
		}

		{
		ReasonerCommunicationOptions o;
		o.comm.componentPointer = comm;
		o.serviceAddress = "ipc://reasoner2";
		o.upstreamReasoner = "ipc://reasoner1";
		o.reasonerID = "node1";
		r2.init(o);
		}

		// push data upstream
		// Step 1: system state:
		{
		cout << "Exchanging SystemHealth...";
		// cout << endl;

		shared_ptr<SystemHealth> sh = shared_ptr<SystemHealth>(new SystemHealth());
		sh->overallState = HealthState::BAD
;
		sh->timeLastModified = 4;
		mCB1.sh = sh;

		shared_ptr<SystemHealth> sh2 = shared_ptr<SystemHealth>(new SystemHealth());
		sh2->overallState = HealthState::BAD
;
		sh2->timeLastModified = 3;
		r2.pushSystemStateUpstream(sh2);

		mCB1.waitUntilSthHappened();
		// Should be the sh2 that r2 just sent up
		// cout << "State 1: "<< mCB1.received_sh.overallState << endl;
		// cout << "Timestamp 1: " << mCB1.received_sh.timeLastModified << endl;
		assert( mCB1.received_sh.overallState == HealthState::BAD
 );
		assert( mCB1.received_sh.timeLastModified == 3);
		// Should be an untouched default SystemHealth()
		// cout << "State 2: "<< mCB2.received_sh.overallState << endl;
		// cout << "Timestamp 2: " << mCB2.received_sh.timeLastModified << endl;
		assert( mCB2.received_sh.overallState == HealthState::OK );
		assert( mCB2.received_sh.timeLastModified >= (Timestamp) time(0) );

		// No response - there's nothing responding "above" system level!

		cout << " done!" << endl;
		}

		// Step 2: node state:
		{
		cout << "Exchanging NodeHealth...";
		// cout << endl;

		shared_ptr<SystemHealth> sh = shared_ptr<SystemHealth>(new SystemHealth());
		sh->overallState = HealthState::BAD
;
		sh->timeLastModified = 2;
		mCB1.sh = sh;

		shared_ptr<NodeHealth> nh = shared_ptr<NodeHealth>(new NodeHealth());
		nh->overallState = HealthState::GOOD
;
		nh->timeLastModified = 1;
		r2.pushNodeStateUpstream(nh);

		mCB1.waitUntilSthHappened();
		// Should be the nh that r2 just sent up
		// cout << "State 1: "<< mCB1.received_nh.overallState << endl;
		// cout << "Timestamp 1: " << mCB1.received_nh.timeLastModified << endl;
		assert( mCB1.received_nh.overallState == HealthState::GOOD );
		assert( mCB1.received_nh.timeLastModified == 1);
		// Should be an untouched default NodeHealth()
		// cout << "State 2: "<< mCB2.received_sh.overallState << endl;
		// cout << "Timestamp 2: " << mCB2.received_sh.timeLastModified << endl;
		assert( mCB2.received_sh.overallState == HealthState::OK );
		assert( mCB2.received_sh.timeLastModified >= (Timestamp) time(0) );

		mCB2.waitUntilSthHappened();
		// Should still be the nh received from r2 before
		// cout << "State 1: "<< mCB1.received_nh.overallState << endl;
		// cout << "Timestamp 1: " << mCB1.received_nh.timeLastModified << endl;
		assert( mCB1.received_nh.overallState == HealthState::GOOD );
		assert( mCB1.received_nh.timeLastModified == 1);
		// Should now be the sh that r1 just sent back
		// cout << "State 2: "<< mCB2.received_sh.overallState << endl;
		// cout << "Timestamp 2: " << mCB2.received_sh.timeLastModified << endl;
		assert( mCB2.received_sh.overallState == HealthState::BAD );
		assert( mCB2.received_sh.timeLastModified == 2 );

		cout << " done!" << endl;
		}

		// Step 3: process state:
		{
		cout << "Exchanging ProcessHealth...";
		// cout << endl;

		shared_ptr<NodeHealth> nh = shared_ptr<NodeHealth>(new NodeHealth());
		nh->overallState = HealthState::BAD;
		nh->timeLastModified = 3;
		mCB1.nh = nh;

		shared_ptr<ProcessHealth> ph = shared_ptr<ProcessHealth>(new ProcessHealth());
		ph->overallState = HealthState::GOOD;
		ph->timeLastModified = 2;
		r2.pushProcessStateUpstream(ph);

		mCB1.waitUntilSthHappened();
		// Should be the ph that r2 just sent up
		// cout << "State 1: "<< mCB1.received_ph.overallState << endl;
		// cout << "Timestamp 1: " << mCB1.received_ph.timeLastModified << endl;
		assert( mCB1.received_ph.overallState == HealthState::GOOD );
		assert( mCB1.received_ph.timeLastModified == 2);
		// Should be an untouched default NodeHealth()
		// cout << "State 2: "<< mCB2.received_nh.overallState << endl;
		// cout << "Timestamp 2: " << mCB2.received_nh.timeLastModified << endl;
		assert( mCB2.received_nh.overallState == HealthState::OK );
		assert( mCB2.received_nh.timeLastModified >= (Timestamp) time(0) );

		mCB2.waitUntilSthHappened();
		// Should still be the ph received from r2 before
		// cout << "State 1: "<< mCB1.received_ph.overallState << endl;
		// cout << "Timestamp 1: " << mCB1.received_ph.timeLastModified << endl;
		assert( mCB1.received_ph.overallState == HealthState::GOOD );
		assert( mCB1.received_ph.timeLastModified == 2);
		// Should now be the nh that r1 just sent back
		// cout << "State 2: "<< mCB2.received_sh.overallState << endl;
		// cout << "Timestamp 2: " << mCB2.received_sh.timeLastModified << endl;
		assert( mCB2.received_nh.overallState == HealthState::BAD );
		assert( mCB2.received_nh.timeLastModified == 3);

		cout << " done!" << endl;
		}
	}

	delete(comm);
}


int main( int argc, char const * argv[] )
{
	testAssessNodeAggregation();
	testSerializationOfTypes();
	testReasonerCommunicationRaw();
	testReasonerAssessment();
	testReasonerAnomalies();
	cout << endl << "=== TEST FINISHED ===" << endl;
	return 0;
}


