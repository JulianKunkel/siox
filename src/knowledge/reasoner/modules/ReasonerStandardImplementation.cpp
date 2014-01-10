#include <knowledge/reasoner/modules/ReasonerStandardImplementation.hpp>

using namespace std;
using namespace core;

namespace knowledge {

shared_ptr<NodeHealth> ReasonerStandardImplementation::getNodeHealth(){
	{	// Disallow other access to aggregated date fields
		unique_lock<mutex> dataLock( dataMutex );

		// FIXME: Implementieren!
	}
	return nullptr;
}

shared_ptr<SystemHealth> ReasonerStandardImplementation::getSystemHealth(){
	{	// Disallow other access to aggregated date fields
		unique_lock<mutex> dataLock( dataMutex );

		// FIXME: Implementieren!
	}
	return nullptr;
}

shared_ptr<ProcessHealth> ReasonerStandardImplementation::getProcessHealth(){
	{	// Disallow other access to aggregated date fields
		unique_lock<mutex> dataLock( dataMutex );

		// FIXME: Implementieren!
	}
	return nullptr;
}

void ReasonerStandardImplementation::receivedReasonerProcessHealth(ReasonerMessageReceived & data, ProcessHealth & health){
	{	// Disallow other access to aggregated date fields
		unique_lock<mutex> dataLock( dataMutex );

		// FIXME: Implementieren!
	}
	cout << "Received msg" << endl;
}

void ReasonerStandardImplementation::receivedReasonerNodeHealth(ReasonerMessageReceived & data, NodeHealth & health){
	{	// Disallow other access to aggregated date fields
		unique_lock<mutex> dataLock( dataMutex );

		// FIXME: Implementieren!
	}
	cout << "Received msg" << endl;
}

void ReasonerStandardImplementation::receivedReasonerSystemHealth(ReasonerMessageReceived & data, SystemHealth & health){
	{	// Disallow other access to aggregated date fields
		unique_lock<mutex> dataLock( dataMutex );

		// FIXME: Implementieren!
	}
	cout << "Received msg" << endl;
}


void ReasonerStandardImplementation::PeriodicRun(){
	bool persistingAnomaly = false;
	while( ! terminated ) {
		cout << "PeriodicRun started" << endl;

		auto timeout = chrono::system_clock::now() + chrono::milliseconds( update_intervall_ms );

		/*
		 * Query all registered ADPIs for news and aggregate them
		 */
		{	// Disallow other access to aggregated date fields
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


		// FIXME: Regeln übernehmen!

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

	update_intervall_ms = options.update_intervall_ms;

	comm.init( options.communicationOptions );

	periodicThread = thread( & ReasonerStandardImplementation::PeriodicRun, this );
}

shared_ptr<HealthStatistics> ReasonerStandardImplementation::queryRuntimePerformanceIssues(){
	return gatheredStatistics;
}

ComponentReport ReasonerStandardImplementation::prepareReport() {
	{	// Disallow other access to aggregated date fields
		unique_lock<mutex> dataLock( dataMutex );

		// FIXME: Implementieren!
	}
	return ComponentReport();
}


/**
 * @brief [brief description]
 * @details [long description]
 *
 * @param operationRatio [description]
 * @param observationCount [description]
 */
/*
void ReasonerStandardImplementation::mergeObservations(list<Health> & l, Health & health, array<uint8_t, HEALTH_STATE_COUNT> & observationRatios, uint64_t & observationCount){
	// aggregate the results of the individual processes
	// unordered_map<string, HealthIssue> positiveIssues, negativeIssues;
	for( auto itr = l.begin(); itr != l.end(); itr++ ){
		// accumulate( itr->positiveIssues, positiveIssues );
		// accumulate( itr->negativeIssues, negativeIssues );

			// The node health state is the average HealthState across all processes weighted by the number of occurrences.
			// => If some occurrences are fast and others are slow the average is OK.

		for ( int i=0; i < HEALTH_STATE_COUNT; i++ ){
			health.occurrences[i] += itr->occurrences[i];
		}
	}

	observationCount = 0;
	for ( int i=0; i < HEALTH_STATE_COUNT; i++ ){
		observationCount += health.occurrences[i];
	}

	if( observationCount == 0 ){
		// What are we doing if no operation has been performed at all
		for ( int i=0; i < HEALTH_STATE_COUNT; i++ ){
			observationRatios[i] = 0;
		}
		return;
	}

	for ( int i=0; i < HEALTH_STATE_COUNT; i++ ){
		observationRatios[i] = health.occurrences[i] * 100 / observationCount;
		cout << health.occurrences[i] << "=" << (int) observationRatios[i] << "% ";
	}
	cout << endl;
}
*/
} // namespace knowledge

extern "C" {
	void * KNOWLEDGE_REASONER_INSTANCIATOR_NAME()
	{
		return new knowledge::ReasonerStandardImplementation();
	}
}
