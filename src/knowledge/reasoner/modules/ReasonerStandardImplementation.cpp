#include <knowledge/reasoner/modules/ReasonerStandardImplementation.hpp>
#include <iostream>

using namespace std;
using namespace core;

namespace knowledge {


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
/*
	#include <iostream>
	#include <cassert>

	enum X { a, b };

	std::ostream& operator<<(std::ostream& os, X x)
	{
	    switch (x) {
	        case a: return os << "a";
	        case b: return os << "b";
	    }
	    assert(false && "wrong enum value");
	}
*/

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



shared_ptr<ProcessHealth> ReasonerStandardImplementation::getProcessHealth(){
	{	// Disallow other access to aggregated data fields
		unique_lock<mutex> dataLock( dataMutex );

		if ( role == ReasonerStandardImplementationOptions::Role::PROCESS )
			return processHealth;
	}
	// For other roles, return NULL; this should never happen
	return nullptr;
}



shared_ptr<NodeHealth> ReasonerStandardImplementation::getNodeHealth(){
	{	// Disallow other access to aggregated data fields
		unique_lock<mutex> dataLock( dataMutex );

		if ( role == ReasonerStandardImplementationOptions::Role::NODE ){
			assessNodeHealth();
			return nodeHealth;
		}
	}
	// For other roles, return NULL; this should never happen
	return nullptr;
}


shared_ptr<SystemHealth> ReasonerStandardImplementation::getSystemHealth(){
	{	// Disallow other access to aggregated data fields
		unique_lock<mutex> dataLock( dataMutex );

		if ( role == ReasonerStandardImplementationOptions::Role::SYSTEM )
			return systemHealth;
	}
	// For other roles, return NULL; this should never happen
	return nullptr;
}


void ReasonerStandardImplementation::receivedReasonerProcessHealth(ReasonerMessageReceived & data, ProcessHealth & health){
	{	// Disallow other access to aggregated data fields
		unique_lock<mutex> dataLock( dataMutex );

		// Only Node level reasoners should receive process level reports; others ignore them
		if ( role == ReasonerStandardImplementationOptions::Role::NODE ){
			(*childProcessesHealthMap)[data.reasonerID] = health;
			// cout << id << " received status from process reasoner " << data.reasonerID << endl;
		}
		else if ( data.reasonerID == "INJECT" ) // Workaround for testing!
			processHealth = make_shared<ProcessHealth>( health );
		else
			cout << id << " received inappropriate message from " << data.reasonerID << "!" << endl;
	}
}


void ReasonerStandardImplementation::receivedReasonerNodeHealth(ReasonerMessageReceived & data, NodeHealth & health){
	{	// Disallow other access to aggregated data fields
		unique_lock<mutex> dataLock( dataMutex );

		if ( role == ReasonerStandardImplementationOptions::Role::SYSTEM ){
			(*childNodesHealthMap)[data.reasonerID] = health;
			// cout << id << " received status from node reasoner " << data.reasonerID << endl;
		}
		else if ( role == ReasonerStandardImplementationOptions::Role::PROCESS ){
			nodeHealth = make_shared<NodeHealth>(health);
			// cout << id << " received status from node reasoner " << data.reasonerID << endl;
		}
		else if ( data.reasonerID == "INJECT" ) // Workaround for testing!
			nodeHealth = make_shared<NodeHealth>( health );
		else
			cout << id << " received inappropriate message from " << data.reasonerID << "!" << endl;
	}
}


void ReasonerStandardImplementation::receivedReasonerSystemHealth(ReasonerMessageReceived & data, SystemHealth & health){
	{	// Disallow other access to aggregated data fields
		unique_lock<mutex> dataLock( dataMutex );

		if ( role == ReasonerStandardImplementationOptions::Role::NODE ){
			// cout << id << " received status from system reasoner " << data.reasonerID << endl;
			systemHealth = make_shared<SystemHealth>(health);
		}
		else if ( data.reasonerID == "INJECT" ) // Workaround for testing!
			systemHealth = make_shared<SystemHealth>( health );
		else
			cout << id << " received inappropriate message from " << data.reasonerID << "!" << endl;
	}
}


void ReasonerStandardImplementation::PeriodicRun(){
	bool persistingAnomaly = false;
	while( ! terminated ) {
		// cout << "PeriodicRun started" << endl;

		auto timeout = chrono::system_clock::now() + chrono::milliseconds( update_intervall_ms );

		/*
		 * Query all registered ADPIs for news and aggregate them
		 */
		{	// Disallow other access to aggregated data fields
			unique_lock<mutex> dataLock( dataMutex );

			{	// Disallow changes in registered plugins while cycling through ADPIs
				unique_lock<mutex> pluginLock( pluginMutex );

				// Query recent observations of all connected plugins and integrate them into the local performance issues.
				// Loop over all registered ADPIs
				for( auto adpi = adpis.begin(); adpi != adpis.end() ; adpi++ ) {
					cout << "ADPI: " << (* adpi) << endl;
					// Memory management passes to our responsibility here!
					unique_ptr<unordered_map<ComponentID, AnomalyPluginHealthStatistic>>  newObservations = ( *adpi )->queryRecentObservations();
					cout << "Reports: " << gatheredStatistics->map.size() << " -> " << newObservations->size() << endl;
					// Loop over all reports from current ADPI (by ComponentID)
					for (auto report = newObservations->begin(); report != newObservations->end(); report++) {
						cout << "Report for CID: " << report->first << endl;
						// Merge this report into the global set
						// TODO: In Funktion auslagern

						// Merge occurrences
						for ( int healthState=0; healthState < HEALTH_STATE_COUNT; healthState++ ){
							cout << "healthState = " << healthState;
							cout << "\tBefore: " << gatheredStatistics->map[report->first].occurrences[healthState];
							gatheredStatistics->map[report->first].occurrences[healthState] += report->second.occurrences[healthState];
							cout << "\tAfter:  " << gatheredStatistics->map[report->first].occurrences[healthState] << endl;
						}

						// Merge positive issues
						unordered_map<string, HealthIssue> * targetIssueMap = & (gatheredStatistics->map[report->first].positiveIssues);
						unordered_map<string, HealthIssue> * sourceIssueMap = & (report->second.positiveIssues);
						for ( auto issue = sourceIssueMap->begin(); issue != sourceIssueMap->end(); issue++)
						{
							auto precedent = targetIssueMap->find( issue->first );
							if ( precedent == targetIssueMap->end() ){
								(*targetIssueMap)[issue->first] = { issue->first, 1, issue->second.delta_time_ms };
							}else{
								HealthIssue & knownIssue = precedent->second;
								knownIssue.occurrences++;
								knownIssue.delta_time_ms += issue->second.delta_time_ms;
							}
						}

						// Merge negative issues
						targetIssueMap = & (gatheredStatistics->map[report->first].negativeIssues);
						sourceIssueMap = & (report->second.negativeIssues);
						for ( auto issue = sourceIssueMap->begin(); issue != sourceIssueMap->end(); issue++)
						{
							auto precedent = targetIssueMap->find( issue->first );
							if ( precedent == targetIssueMap->end() ){
								(*targetIssueMap)[issue->first] = { issue->first, 1, issue->second.delta_time_ms };
							}else{
								HealthIssue & knownIssue = precedent->second;
								knownIssue.occurrences++;
								knownIssue.delta_time_ms += issue->second.delta_time_ms;
							}
						}
					}
					// cout << id << " reports: " << gatheredStatistics->map.size() << " -> " << newObservations->size() << endl;
				}
			} // pluginLock
			// Update statistics on the different observation categories
			observationTotal = 0;
			for ( int i = 0; i< HEALTH_STATE_COUNT; i++ ){
				observationCounts[i] = 0;
			}
			for ( auto report : gatheredStatistics->map ) {
				for ( int i = 0; i < HEALTH_STATE_COUNT; i++ ){
					observationCounts[i] += report.second.occurrences[i];
					observationTotal += report.second.occurrences[i];
				}
			}
			if( observationTotal == 0 ){
				// What to do if no operation has been performed at all
				for (  auto i = 0; i< HEALTH_STATE_COUNT; i++  ){
					observationRatios[i] = 0;
				}
			}
			else{
				for ( int i = 0; i< HEALTH_STATE_COUNT; i++ ){
					float scale = 100 / observationTotal;
					observationRatios[i] = observationCounts[i] * scale;
					cout << observationCounts[i] << "=" << (int) observationRatios[i] << "% ";
				}
				cout << endl;
			}

			// Run assessment appropriate to reasoner's role
			switch ( role ) {

				case ReasonerStandardImplementationOptions::Role::PROCESS:
					assessProcessHealth();
					if (upstreamReasonerExists){
						// cout << id << " pushing process state upstream!" << endl;
						comm->pushProcessStateUpstream( processHealth);
					}
					break;

				case ReasonerStandardImplementationOptions::Role::NODE:
					assessNodeHealth();
					if (upstreamReasonerExists){
						// cout << id << " pushing node state upstream!" << endl;
						comm->pushNodeStateUpstream( nodeHealth);
					}
					break;

				case ReasonerStandardImplementationOptions::Role::SYSTEM:
					assessSystemHealth();
					if (upstreamReasonerExists){
						// cout << id << " pushing system state upstream!" << endl;
						comm->pushSystemStateUpstream( systemHealth);
					}
					break;

				default:
					break;
			}
		} // dataLock

		// TODO Think:
		// Update recent performance issues that are stored locally.
		// Use significiant information as stored in the observations.

		// Restrict anomaly triggering to cases when there actually is an ADPI.
		// This will avoid a race condition.
		bool anomalyDetected = (adpis.size() > 0);
		// TODO

		// Determine local performance issues based on recent observations and remote issues.
		// TODO

		// For testing:
		//if( adpiObservations->size() > 0 ) {
		//	anomalyDetected = true;
		//}
		if( utilization != nullptr ) {
//						StatisticObservation so = utilization->lastObservation( 4711 );
		}

		// Save recentIssues
		// TODO

		// Update knownIssues based on recentIssues
		// TODO

		// Trigger anomaly if any:
		{	// Disallow changes in registered plugins while cycling through triggers
			unique_lock<mutex> pluginLock( pluginMutex );

			if( anomalyDetected && ! persistingAnomaly ) {
				cout << " Calling triggers!" << endl;
				for( auto itr = triggers.begin(); itr != triggers.end() ; itr++ ) {
					( *itr )->triggerResponseForAnomaly(false);
				}
			}
		}
		// TODO sometimes we'd like to have persistent Anomalies ... persistingAnomaly = anomalyDetected;

		// Forward detected performance issues to remote reasoners
		// TODO


		// cout << "PeriodicRun() going to sleep..." << endl;
		unique_lock<mutex> lock( recentIssuesMutex );
		// TODO: Understand, clarify and comment this!
		if( terminated ) {
			break;
		}
		running_condition.wait_until( lock, timeout );
	}
	// cout << "PeriodicRun finished" << endl;
}



ReasonerStandardImplementation::~ReasonerStandardImplementation() {
	// {	unique_lock<mutex> dataLock( dataMutex );
		// unique_lock<mutex> pluginLock( pluginMutex );
			recentIssuesMutex.lock();
			// TODO: Understand, clarify and comment this!
			terminated = true;
			running_condition.notify_one();
			recentIssuesMutex.unlock();
		periodicThread.join();
	// }

	delete(comm);
}

void ReasonerStandardImplementation::init(){
	ReasonerStandardImplementationOptions & options = getOptions<ReasonerStandardImplementationOptions>();

	id = options.communicationOptions.reasonerID;
	role = options.role;
	switch( role ){
		case ReasonerStandardImplementationOptions::Role::SYSTEM :
			systemHealth = make_shared<SystemHealth>();
			childNodesHealthMap = new unordered_map<string,NodeHealth>;
			break;
		case ReasonerStandardImplementationOptions::Role::NODE :
			systemHealth = make_shared<SystemHealth>();
			nodeHealth = make_shared<NodeHealth>();
			childProcessesHealthMap = new unordered_map<string,ProcessHealth>;
			break;
		case ReasonerStandardImplementationOptions::Role::PROCESS :
			nodeHealth = make_shared<NodeHealth>();
			processHealth = make_shared<ProcessHealth>();
			break;
	}

	update_intervall_ms = options.update_intervall_ms;

	comm = new ReasonerCommunication(*this);

	comm->init( options.communicationOptions );
	if (options.communicationOptions.upstreamReasoner != "")
		upstreamReasonerExists = true;

	periodicThread = thread( & ReasonerStandardImplementation::PeriodicRun, this );
}


shared_ptr<HealthStatistics> ReasonerStandardImplementation::queryRuntimePerformanceIssues(){
	return gatheredStatistics;
}


ComponentReport ReasonerStandardImplementation::prepareReport() {

	ComponentReport result;

	{	// Disallow other access to aggregated data fields
		unique_lock<mutex> dataLock( dataMutex );

		ostringstream reportText;

		reportText << this;
		result.addEntry( id, ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( reportText.str() )));
	}

	return result;
}


std::ostream & operator<<( std::ostream & os, const ReasonerStandardImplementation * r )
{
	ostringstream result;

	result << "\t\"" << r->id << "\":" << endl;
	switch ( r->role ){
		case ReasonerStandardImplementationOptions::Role::PROCESS :
			result << (*(r->processHealth)) << endl;
			break;
		case ReasonerStandardImplementationOptions::Role::NODE :
			result << (*(r->nodeHealth)) << endl;
			break;
		case ReasonerStandardImplementationOptions::Role::SYSTEM :
			result << (*(r->systemHealth)) << endl;
			break;
		default:
			assert(false && "Invalid reasoner role!");
	}

	return os << result.str();
}





void ReasonerStandardImplementation::assessNodeHealth(){
	// cout << "Reasoner " << id << " assessing NODE health..." << endl;
/*
	// Input: node statistics, process health, system health
	// Configuration: global reasoner address, local address
	// Result: node health
*/
	// For now: Simulate historic occurences by totalling child processes' occurences //TODO Replace!
	for (int i = 0; i < HEALTH_STATE_COUNT; ++i)
		nodeHealth->occurrences[i] = 0;
	for(auto childHealth : *childProcessesHealthMap)
		for (int i = 0; i < HEALTH_STATE_COUNT; ++i)
			nodeHealth->occurrences[i] += childHealth.second.occurrences[i];


	// determine node health based on the historic knowledge AND the statistics
	if ( nodeHealth->occurrences[ABNORMAL_FAST] || nodeHealth->occurrences[ABNORMAL_SLOW] || nodeHealth->occurrences[ABNORMAL_OTHER] ){
		// we have a condition in which we must fire the anomaly trigger.
		// cout << "Checkpoint 1a passed..." << endl;
		if ( observationRatios[ABNORMAL_FAST] > 5 &&
			observationRatios[ABNORMAL_FAST] > 2 * observationRatios[ABNORMAL_SLOW] &&
			observationRatios[ABNORMAL_FAST] > 2 * observationRatios[ABNORMAL_OTHER] )
		{
			nodeHealth->overallState = HealthState::ABNORMAL_FAST;
		}else if (observationRatios[ABNORMAL_SLOW] > 5 &&
			observationRatios[ABNORMAL_SLOW] > 2 * observationRatios[ABNORMAL_FAST] &&
			observationRatios[ABNORMAL_SLOW] > 2 * observationRatios[ABNORMAL_OTHER] )
		{
			nodeHealth->overallState = HealthState::ABNORMAL_SLOW;
		}else{
			nodeHealth->overallState = HealthState::ABNORMAL_OTHER;
		}
	}else{
		// cout << "Checkpoint 1b passed..." << endl;
		// normal condition
		// We have to pick between FAST, OK & SLOW
		if ( observationRatios[FAST] > 5 &&
			observationRatios[FAST] > 3*observationRatios[SLOW] )
		{
			// cout << "Checkpoint 1ba passed..." << endl;
			nodeHealth->overallState = HealthState::FAST;
		}else if ( observationRatios[SLOW] > 5 &&
			observationRatios[SLOW] > 3*observationRatios[FAST] )
		{
			// cout << "Checkpoint 1bb passed..." << endl;
			nodeHealth->overallState = HealthState::SLOW;
		}else{
			// cout << "Checkpoint 1bc passed..." << endl;
			nodeHealth->overallState = HealthState::OK;
		}
	}

	// cout << "Checkpoint 2 passed..." << endl;
	// If the current state is not OK and a statistics behaves suboptimal, we can take this into account and potentially add new issues and even set the health state to OK.
	if ( nodeHealth->overallState != HealthState::OK ){
		uint totalUtilization = 0;

		if ( nodeHealth->overallState == HealthState::SLOW || nodeHealth->overallState == HealthState::ABNORMAL_SLOW || nodeHealth->overallState == HealthState::ABNORMAL_OTHER ){

			for(int i=0; i < UTILIZATION_STATISTIC_COUNT; i++ ){
				totalUtilization += nodeHealth->utilization[i];
				if ( nodeHealth->utilization[i] > 80 ){ // if more than 80% utilization in one category
					// overloaded!
					nodeHealth->negativeIssues.push_back( { toString( (UtilizationIndex) i) + " overloaded", 1, 0 } );
					// actually the decision should depend on this nodes role. A compute node is expected to have a high CPU and MEMORY utilization...
					// TODO
					nodeHealth->overallState = HealthState::OK;
				}
			}
			if ( totalUtilization > 60*4 ){ // the overall system is overloaded
				// do we really need this? More sophisticated decisions would be good.
			}
		}
	}
/*
*/
	// cout << "State of resoner " << id << ": " << toString(nodeHealth->overallState) << endl;
}


void ReasonerStandardImplementation::assessProcessHealth(){
	// cout << "Reasoner " << id << " assessing PROCESS health..." << endl;
	// FIXME: Implementieren!

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

	// cout << "State of resoner " << id << ": " << toString(processHealth->overallState) << endl;
}



void ReasonerStandardImplementation::assessSystemHealth(){
	// cout << "Reasoner " << id << " assessing SYSTEM health..." << endl;
	// FIXME: Implementieren!

	// Input: node health
	// Configuration: set of nodes belonging to each file system (at the moment we consider only  one file system)
	// Result: system health
	// Test case
	// Three nodes report overlapping issues.

	// cout << "State of resoner " << id << ": " << toString(systemHealth->overallState) << endl;
}


} // namespace knowledge

extern "C" {
	void * KNOWLEDGE_REASONER_INSTANCIATOR_NAME()
	{
		return new knowledge::ReasonerStandardImplementation();
	}
}
