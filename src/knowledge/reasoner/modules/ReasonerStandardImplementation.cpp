#include <knowledge/reasoner/modules/ReasonerStandardImplementation.hpp>

using namespace std;
using namespace core;

namespace knowledge {

shared_ptr<NodeHealth> ReasonerStandardImplementation::getNodeHealth(){
	return nullptr;
}

shared_ptr<SystemHealth> ReasonerStandardImplementation::getSystemHealth(){
	return nullptr;
}

shared_ptr<ProcessHealth> ReasonerStandardImplementation::getProcessHealth(){
	return nullptr;
}

void ReasonerStandardImplementation::receivedReasonerProcessHealth(ReasonerMessageReceived & data, ProcessHealth & health){
	cout << "Received msg" << endl;
}

void ReasonerStandardImplementation::receivedReasonerNodeHealth(ReasonerMessageReceived & data, NodeHealth & health){
	cout << "Received msg" << endl;
}

void ReasonerStandardImplementation::receivedReasonerSystemHealth(ReasonerMessageReceived & data, SystemHealth & health){
	cout << "Received msg" << endl;
}


void ReasonerStandardImplementation::PeriodicRun(){
	bool persistingAnomaly = false;
	while( ! terminated ) {
		//cout << "PeriodicRun" << terminated << endl;
		auto timeout = chrono::system_clock::now() + chrono::milliseconds( update_intervall_ms );

		unique_ptr<unordered_map<ComponentID, AnomalyPluginHealthStatistic>>  adpiObservations;
		// query recent observations of all connected plugins and integrate them into the local performance issues.
		for( auto itr = adpis.begin(); itr != adpis.end() ; itr++ ) {
			// memory management is going in our responsibility.
			unique_ptr<unordered_map<ComponentID, AnomalyPluginHealthStatistic>>  apo = ( *itr )->queryRecentObservations();

			// Merge this set of the adpi into the global list.
			//mergeObservations( adpiObservations, *apo );
			//cout << adpiObservations.size() << " " << apo->size() << endl;
		}

		// Think:
		// Update recent performance issues that are stored locally.
		// Use significiant information as stored in the observations.
		bool anomalyDetected = false;
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
		if( anomalyDetected && ! persistingAnomaly ) {
			for( auto itr = triggers.begin(); itr != triggers.end() ; itr++ ) {
				( *itr )->triggerResponseForAnomaly(false);
			}
		}
		// TODO sometimes we'd like to have persistent Anomalies ... persistingAnomaly = anomalyDetected;

		// Forward detected performance issues to remote reasoners

		unique_lock<mutex> lock( recentIssues_lock );
		if( terminated ) {
			break;
		}
		running_condition.wait_until( lock, timeout );
	}
	//cout << "PeriodicRun finished" << endl;
}



ReasonerStandardImplementation::~ReasonerStandardImplementation() {
	recentIssues_lock.lock();
	terminated = true;
	running_condition.notify_one();
	recentIssues_lock.unlock();

	periodicThread.join();
}

void ReasonerStandardImplementation::init(){
	ReasonerStandardImplementationOptions & options = getOptions<ReasonerStandardImplementationOptions>();

	update_intervall_ms = options.update_intervall_ms;

	comm.init( options.communicationOptions );

	periodicThread = thread( & ReasonerStandardImplementation::PeriodicRun, this );
}

shared_ptr<HealthStatistics> ReasonerStandardImplementation::queryRuntimePerformanceIssues(){
	return nullptr;
}

} // namespace knowledge

extern "C" {
	void * KNOWLEDGE_REASONER_INSTANCIATOR_NAME()
	{
		return new knowledge::ReasonerStandardImplementation();
	}
}
