#include <knowledge/reasoner/modules/ReasonerStandardImplementation.hpp>

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



void ReasonerStandardImplementation::injectLocalHealth( shared_ptr<NodeHealth> health ){
	{	// Disallow other access to aggregated data fields
		unique_lock<mutex> dataLock( dataMutex );

		localHealth = health;
	}
}

shared_ptr<NodeHealth> ReasonerStandardImplementation::getNodeHealth(){
	{	// Disallow other access to aggregated data fields
		unique_lock<mutex> dataLock( dataMutex );

		if ( role == ReasonerStandardImplementationOptions::Role::NODE )
			return localHealth;
	}
	// For other roles, return NULL
	return nullptr;
}

shared_ptr<SystemHealth> ReasonerStandardImplementation::getSystemHealth(){
	{	// Disallow other access to aggregated data fields
		unique_lock<mutex> dataLock( dataMutex );

		if ( role == ReasonerStandardImplementationOptions::Role::SYSTEM )
			return localHealth;
	}
	// For other roles, return NULL
	return nullptr;
}

shared_ptr<ProcessHealth> ReasonerStandardImplementation::getProcessHealth(){
	{	// Disallow other access to aggregated data fields
		unique_lock<mutex> dataLock( dataMutex );

		if ( role == ReasonerStandardImplementationOptions::Role::PROCESS )
			return localHealth;
	}
	// For other roles, return NULL
	return nullptr;
}

void ReasonerStandardImplementation::receivedReasonerProcessHealth(ReasonerMessageReceived & data, ProcessHealth & health){
	{	// Disallow other access to aggregated data fields
		unique_lock<mutex> dataLock( dataMutex );

		// Only Node level reasoners should receive process level reports; others ignore them
		if ( role == ReasonerStandardImplementationOptions::Role::NODE ){
			(*processHealthMap)[data.reasonerID] = health;
			cout << "Received status from process reasoner " << data.reasonerID << endl;
		}
		else
			cerr << "Received inappropriate message from reasoner " << data.reasonerID << "!" << endl;
	}
}

void ReasonerStandardImplementation::receivedReasonerNodeHealth(ReasonerMessageReceived & data, NodeHealth & health){
	{	// Disallow other access to aggregated data fields
		unique_lock<mutex> dataLock( dataMutex );

		if ( role == ReasonerStandardImplementationOptions::Role::SYSTEM ){
			(*nodeHealthMap)[data.reasonerID] = health;
			cout << "Received status from node reasoner " << data.reasonerID << endl;
		}
		else if ( role == ReasonerStandardImplementationOptions::Role::PROCESS ){
			cout << "Received status from node reasoner " << data.reasonerID << endl;
			nodeHealth = health;
		}
		else
			cerr << "Received inappropriate message from reasoner " << data.reasonerID << "!" << endl;
	}
}

void ReasonerStandardImplementation::receivedReasonerSystemHealth(ReasonerMessageReceived & data, SystemHealth & health){
	{	// Disallow other access to aggregated data fields
		unique_lock<mutex> dataLock( dataMutex );

		if ( role == ReasonerStandardImplementationOptions::Role::NODE ){
			cout << "Received status from system reasoner " << data.reasonerID << endl;
			systemHealth = health;
		}
		else
			cerr << "Received inappropriate message from reasoner " << data.reasonerID << "!" << endl;
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
				// unique_ptr<unordered_map<ComponentID, AnomalyPluginHealthStatistic>>  adpiObservations( new unordered_map<ComponentID, AnomalyPluginHealthStatistic> );
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
					// cout << "Reports: " << gatheredStatistics->map.size() << " -> " << newObservations->size() << endl;
				}
			}

			// Update statistics on the different observation categories
			observationTotal = 0;
			for ( int i=0; i < HEALTH_STATE_COUNT; i++ ){
				observationCounts[i] = 0;
			}
			for ( auto report = gatheredStatistics->map.begin(); report != gatheredStatistics->map.end(); report++) {
				for ( int i=0; i < HEALTH_STATE_COUNT; i++ ){
					observationCounts[i] += report->second.occurrences[i];
					observationTotal += report->second.occurrences[i];
				}
			}
			if( observationTotal == 0 ){
				// What to do if no operation has been performed at all
				for ( int i=0; i < HEALTH_STATE_COUNT; i++ ){
					observationRatios[i] = 0;
				}
			}
			else{
				for ( int i=0; i < HEALTH_STATE_COUNT; i++ ){
					float scale = 100 / observationTotal;
					observationRatios[i] = observationCounts[i] * scale;
					cout << observationCounts[i] << "=" << (int) observationRatios[i] << "% ";
				}
				cout << endl;
			}
		}

		// Run assessment appropriate to reasoner's role
		switch ( role ) {
			case ReasonerStandardImplementationOptions::Role::PROCESS:
				assessProcessHealth();
				break;
			case ReasonerStandardImplementationOptions::Role::NODE:
				assessNodeHealth();
				break;
			case ReasonerStandardImplementationOptions::Role::SYSTEM:
				assessSystemHealth();
				break;

		}

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
	recentIssuesMutex.lock();
	// TODO: Understand, clarify and comment this!
	terminated = true;
	running_condition.notify_one();
	recentIssuesMutex.unlock();

	periodicThread.join();
}

void ReasonerStandardImplementation::init(){
	ReasonerStandardImplementationOptions & options = getOptions<ReasonerStandardImplementationOptions>();

	role = options.role;
	if (role == ReasonerStandardImplementationOptions::Role::NODE)
		processHealthMap = new unordered_map<string,ProcessHealth>;
	if (role == ReasonerStandardImplementationOptions::Role::SYSTEM)
		nodeHealthMap = new unordered_map<string,NodeHealth>;

	update_intervall_ms = options.update_intervall_ms;

	comm.init( options.communicationOptions );

	periodicThread = thread( & ReasonerStandardImplementation::PeriodicRun, this );

/*
	// Some mock-up values until true utilization is available
	localHealth->utilization[UtilizationIndex::CPU] = 20;
	localHealth->utilization[UtilizationIndex::NETWORK] = 40;
	localHealth->utilization[UtilizationIndex::IO] = 30;
	localHealth->utilization[UtilizationIndex::MEMORY] = 10;
*/
}


shared_ptr<HealthStatistics> ReasonerStandardImplementation::queryRuntimePerformanceIssues(){
	return gatheredStatistics;
}


ComponentReport ReasonerStandardImplementation::prepareReport() {
	{	// Disallow other access to aggregated data fields
		unique_lock<mutex> dataLock( dataMutex );

		// FIXME: Implementieren!
	}
	return ComponentReport();
}


void ReasonerStandardImplementation::assessNodeHealth(){
	cout << "Assessing NODE health..." << endl;
/*
	// TODO: Write a test feeding this to reasoner via received... methods

	// Input: node statistics, process health, system health
	// Configuration: global reasoner address, local address
	// Result: node health
	//SystemHealth sh = { HealthState::OK, {{0,1,5,1,0,0}}, {}, {} };

	ProcessHealth p1 = { HealthState::SLOW, 	{{0,1,5,5,0,0}}, { {"cache hits", 5, 0} }, { {"cache misses", 4, 0} } };
	ProcessHealth p2 = { HealthState::OK, 		{{0,1,5,1,0,0}}, { { "suboptimal access pattern type 1", 2, 10 }, {"cache hits", 2, -1} }, { {"cache misses", 1, +1} } };
	ProcessHealth p3 = { HealthState::FAST, 	{{0,5,5,1,0,0}}, { { "optimal access pattern type 1", 2, 10 }, {"cache hits", 3, 0} }, { {"cache misses", 3, 0} } };

	list<ProcessHealth> l = {p1, p2, p3};
*/

	// determine node health based on the historic knowledge AND the statistics
	if ( localHealth->occurrences[ABNORMAL_FAST] || localHealth->occurrences[ABNORMAL_SLOW] || localHealth->occurrences[ABNORMAL_OTHER] ){
		// we have a condition in which we must fire the anomaly trigger.
		cout << "Checkpoint 1a passed..." << endl;
		if ( observationRatios[ABNORMAL_FAST] > 5 &&
			observationRatios[ABNORMAL_FAST] > 2 * observationRatios[ABNORMAL_SLOW] &&
			observationRatios[ABNORMAL_FAST] > 2 * observationRatios[ABNORMAL_OTHER] )
		{
			localHealth->overallState = HealthState::ABNORMAL_FAST;
		}else if (observationRatios[ABNORMAL_SLOW] > 5 &&
			observationRatios[ABNORMAL_SLOW] > 2 * observationRatios[ABNORMAL_FAST] &&
			observationRatios[ABNORMAL_SLOW] > 2 * observationRatios[ABNORMAL_OTHER] )
		{
			localHealth->overallState = HealthState::ABNORMAL_SLOW;
		}else{
			localHealth->overallState = HealthState::ABNORMAL_OTHER;
		}
	}else{
		cout << "Checkpoint 1b passed..." << endl;
		// normal condition
		// We have to pick between FAST, OK & SLOW
		if ( observationRatios[FAST] > 5 &&
			observationRatios[FAST] > 3*observationRatios[SLOW] )
		{
			cout << "Checkpoint 1ba passed..." << endl;
			localHealth->overallState = HealthState::FAST;
		}else if ( observationRatios[SLOW] > 5 &&
			observationRatios[SLOW] > 3*observationRatios[FAST] )
		{
			cout << "Checkpoint 1bb passed..." << endl;
			localHealth->overallState = HealthState::SLOW;
		}else{
			cout << "Checkpoint 1bc passed..." << endl;
			localHealth->overallState = HealthState::OK;
		}
	}

	cout << "Checkpoint 2 passed..." << endl;
	// If the current state is not OK and a statistics behaves suboptimal, we can take this into account and potentially add new issues and even set the health state to OK.
	if ( localHealth->overallState != HealthState::OK ){
		uint totalUtilization = 0;

		if ( localHealth->overallState == HealthState::SLOW || localHealth->overallState == HealthState::ABNORMAL_SLOW || localHealth->overallState == HealthState::ABNORMAL_OTHER ){

			for(int i=0; i < UTILIZATION_STATISTIC_COUNT; i++ ){
				totalUtilization += localHealth->utilization[i];
				if ( localHealth->utilization[i] > 80 ){ // if more than 80% utilization in one category
					// overloaded!
					localHealth->negativeIssues.push_back( { toString( (UtilizationIndex) i) + " overloaded", 1, 0 } );
					// actually the decision should depend on this nodes role. A compute node is expected to have a high CPU and MEMORY utilization...
					// TODO
					localHealth->overallState = HealthState::OK;
				}
			}
			if ( totalUtilization > 60*4 ){ // the overall system is overloaded
				// do we really need this? More sophisticated decisions would be good.
			}
		}
	}

	cout << "State: " << toString(localHealth->overallState) << endl;
}


void ReasonerStandardImplementation::assessProcessHealth(){
	cout << "Assessing PROCESS health..." << endl;
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
	cout << "State: " << toString(localHealth->overallState) << endl;
}



void ReasonerStandardImplementation::assessSystemHealth(){
	cout << "Assessing SYSTEM health..." << endl;
	// FIXME: Implementieren!

	// Input: node health
	// Configuration: set of nodes belonging to each file system (at the moment we consider only  one file system)
	// Result: system health
	// Test case
	// Three nodes report overlapping issues.
}


} // namespace knowledge

extern "C" {
	void * KNOWLEDGE_REASONER_INSTANCIATOR_NAME()
	{
		return new knowledge::ReasonerStandardImplementation();
	}
}
