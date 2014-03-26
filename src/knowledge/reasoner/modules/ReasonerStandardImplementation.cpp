#include <knowledge/reasoner/modules/ReasonerStandardImplementation.hpp>
#include <monitoring/statistics/collector/StatisticsCollector.hpp>

#include <iostream>

using namespace std;
using namespace core;
using namespace monitoring;

namespace knowledge {

enum DataIndex{
	UTILIZATION_CPU = 0,
	UTILIZATION_MEMORY,
	UTILIZATION_IO,
	UTILIZATION_NETWORK_SEND,
	UTILIZATION_NETWORK_RECEIVE,

	CONSUMED_CPU_SECONDS,
	CONSUMED_ENERGY_JOULE,
	CONSUMED_MEMORY_BYTES,
	CONSUMED_NETWORK_BYTES,
	CONSUMED_IO_BYTES,

	DATA_INDEX_COUNT
};

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

		if ( role == ReasonerStandardImplementationOptions::Role::NODE )
			return nodeHealth;
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

		nPushesReceived++;
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
			// aggregate statistics
			for (int i=0; i < NODE_STATISTIC_COUNT; i++){
				node_statistics[i] += health.statistics[i];
			}
			cout << "CPU: " << health.statistics[6] << endl;
			cout << "RAPL: " << health.statistics[7] << endl;

			nodeHealth = make_shared<NodeHealth>(health);

			//cout << id << " received status from node reasoner " << data.reasonerID << endl;
		}
		else if ( data.reasonerID == "INJECT" ) // Workaround for testing!
			nodeHealth = make_shared<NodeHealth>( health );
		else
			cout << id << " received inappropriate message from " << data.reasonerID << "!" << endl;

		nPushesReceived++;
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

		nPushesReceived++;
}


void ReasonerStandardImplementation::PeriodicRun(){
	bool persistingAnomaly; // Last cycle's anomalyDetected
	bool anomalyDetected = false;

	while( ! terminated ) {
		// cout << "PeriodicRun started" << endl;

		auto timeout = chrono::system_clock::now() + chrono::milliseconds( update_intervall_ms );

		cyclesTriggered++;

		persistingAnomaly = anomalyDetected;
		anomalyDetected = false;

		/*
		 * Query all registered ADPIs for news and aggregate them
		 */
		{	// Disallow other access to aggregated data fields
			unique_lock<mutex> dataLock( dataMutex );

			Health * localHealth = nullptr;
			switch (role){
				case ReasonerStandardImplementationOptions::Role::PROCESS :
					localHealth = processHealth.get();
					break;
				case ReasonerStandardImplementationOptions::Role::NODE :
					localHealth = nodeHealth.get();
					break;
				case ReasonerStandardImplementationOptions::Role::SYSTEM :
					localHealth = systemHealth.get();
					break;
				default:
					assert(false && "Invalid process role!");
			}

			{	// Disallow changes in registered plugins while cycling through ADPIs
				unique_lock<mutex> pluginLock( pluginMutex );

				// Query recent observations of all connected plugins and integrate them into the local performance issues.
				// Loop over all registered ADPIs
				for( auto adpi : adpis ) {
					// cout << "ADPI: " << adpi << endl;
					// Memory management passes to our responsibility here!
					unique_ptr<unordered_map<ComponentID, AnomalyPluginHealthStatistic>>  newObservations = adpi->queryRecentObservations();
					// cout << "Reports: " << gatheredStatistics->map.size() << " -> " << newObservations->size() << endl;
					// Loop over all reports from current ADPI (by ComponentID)
					for (auto report : *newObservations ) {
						// cout << "Report for CID: " << report.first << endl;
						// Merge this report into the global set
						// TODO: In Funktion auslagern

						// Merge occurrences into gatheredStatistics and local Health
						for ( int healthState=0; healthState < HEALTH_STATE_COUNT; healthState++ ){
							// cout << "healthState = " << healthState;
							// cout << "\tBefore: " << gatheredStatistics->map[report.first].occurrences[healthState];
							gatheredStatistics->map[report.first].cid = report.first;
							gatheredStatistics->map[report.first].occurrences[healthState] += report.second.occurrences[healthState];
							// cout << "\tAfter:  " << gatheredStatistics->map[report.first].occurrences[healthState] << endl;
							localHealth->occurrences[healthState] += report.second.occurrences[healthState];
							// cout << "Updated: " << gatheredStatistics->map[report.first] << endl;

						}

						// Merge positive issues into gatheredStatistics and *localHealth
						unordered_map<string, HealthIssue> * sourceIssueMap = & (report.second.positiveIssues);
						unordered_map<string, HealthIssue> * targetIssueMap = & (gatheredStatistics->map[report.first].positiveIssues);
						list<HealthIssue> * targetIssueList = & (localHealth->positiveIssues);
						for ( auto issue : *sourceIssueMap )
						{
							// cout << id << " received issue: "<< issue.second << endl;

							{ // gatheredStatistics
								auto precedent = targetIssueMap->find( issue.first );
								if ( precedent == targetIssueMap->end() )
									(*targetIssueMap)[issue.first] = HealthIssue( issue.first, issue.second.occurrences, issue.second.delta_time_ms );
								else
									precedent->second.add(issue.second);
							}

							{ // *localHealth
								HealthIssue * precedent = nullptr;
								for( auto candidate : *targetIssueList ){
									if ( candidate.name == issue.first )
										precedent = & candidate;
								}
								if( precedent == nullptr )
									targetIssueList->push_back( HealthIssue( issue.first, issue.second.occurrences, issue.second.delta_time_ms ) );
								else
									precedent->add( issue.second );
							}
						}

						// Merge negative issues into gatheredStatistics and *localHealth
						sourceIssueMap = & (report.second.negativeIssues);
						targetIssueMap = & (gatheredStatistics->map[report.first].negativeIssues);
						targetIssueList = & (localHealth->negativeIssues);
						for ( auto issue : *sourceIssueMap )
						{
							// cout << id << " received issue: "<< issue.second << endl;

							{ // gatheredStatistics
							auto precedent = targetIssueMap->find( issue.first );
							if ( precedent == targetIssueMap->end() ){
								(*targetIssueMap)[issue.first] = HealthIssue( issue.first, issue.second.occurrences, issue.second.delta_time_ms );
								// cout << "Created " << (*targetIssueMap)[issue.first] << endl;
							}
							else{
								precedent->second.add(issue.second);
								// cout << "Added to " << precedent->second << endl;
							}
							}

							{ // *localHealth
								HealthIssue * precedent = nullptr;
								for( auto candidate : *targetIssueList ){
									if ( candidate.name == issue.first ){
										precedent = & candidate;
									}
								}
								if( precedent == nullptr )
									targetIssueList->push_back( HealthIssue( issue.first, issue.second.occurrences, issue.second.delta_time_ms ) );
								else
									precedent->add( issue.second );
							}
						}
					}
					// cout << id << " reports: " << gatheredStatistics->map.size() << " -> " << newObservations->size() << endl;
				}
			} // pluginLock

			// Remember last step's statistics for comparison
			oldObservationCounts = observationCounts;
			oldObservationRatios = observationRatios;
			oldObservationTotal = observationTotal;
			// Reset current counts and total
			observationTotal = 0;
			for ( int i = 0; i< HEALTH_STATE_COUNT; i++ )
				observationCounts[i] = 0;
			// Add all gathered statistics by health state
			for ( auto report : gatheredStatistics->map )
				for ( int i = 0; i < HEALTH_STATE_COUNT; i++ )
					observationCounts[i] += report.second.occurrences[i];
			// Total up current counts
			for ( int i = 0; i< HEALTH_STATE_COUNT; i++ )
				observationTotal += observationCounts[i];
			// Compute current ratios
			if( observationTotal > 0 ){
				float scale = 100.0 / observationTotal;
				for ( int i = 0; i< HEALTH_STATE_COUNT; i++ ){
					observationRatios[i] = observationCounts[i] * scale;
					// cout << observationCounts[i] << "=" << (int) observationRatios[i] << "% ";
				}
				// cout << endl;
			}

			// Check whether anything happened
			if ( observationTotal != oldObservationTotal ){
				// cout << id << " has news: " << oldObservationTotal << "->" << observationTotal << endl;

				// Check whether anything interesting happened
				for (int i = 0; i < HEALTH_STATE_COUNT; ++i)
				{
					// cout << i << ": " << (int)oldObservationRatios[i] << "->" << (int)observationRatios[i]<< endl;
					// Did ratio of any non-OK state move by more than 2 points?
					if( i != HealthState::OK ){
						int diff = observationRatios[i] - oldObservationRatios[i];
						if( abs(diff) >= 2 ){
							anomalyDetected = true;
							// cout << id << " detected an anomaly (moving observation ratio for state " << i << " from " << (int)oldObservationRatios[i] << " to "<< (int)observationRatios[i] << ", or " << observationCounts[i] << " out of " << observationTotal << ")!" << endl;
						}
					}

					// Did number of any ABNORMAL state rise?
					if( i == HealthState::ABNORMAL_SLOW
					   || i == HealthState::ABNORMAL_FAST
					   || i == HealthState::ABNORMAL_OTHER ){
						if( observationCounts[i] > oldObservationCounts[i] ){
							anomalyDetected = true;
							// cout << id << " detected an anomaly (abnormal state " << i << ")!" << endl;
						}
					}
				}
			}

			// Run assessment appropriate to reasoner's role;
			// then, forward current state to remote reasoners
			switch ( role ) {

				case ReasonerStandardImplementationOptions::Role::PROCESS:
					assessProcessHealth();
					if (upstreamReasonerExists){
						// cout << id << " pushing process state upstream!" << endl;
						comm->pushProcessStateUpstream( processHealth);
						nPushesSent++;
					}
					break;

				case ReasonerStandardImplementationOptions::Role::NODE:
					assessNodeHealth();
					if (upstreamReasonerExists){
						// cout << id << " pushing node state upstream!" << endl;
						comm->pushNodeStateUpstream( nodeHealth);
						nPushesSent++;
					}
					break;

				case ReasonerStandardImplementationOptions::Role::SYSTEM:
					assessSystemHealth();
					if (upstreamReasonerExists){
						// cout << id << " pushing system state upstream!" << endl;
						comm->pushSystemStateUpstream( systemHealth);
						nPushesSent++;
					}
					break;

				default:
					break;
			}
		} // dataLock

		// Determine local performance issues based on recent observations and remote issues.


		// now retrieve the nodeStatistics.
		if( nodeStatistics != nullptr ) {

			nodeStatistics->fetchValues();

			for(int i=0; i < NODE_STATISTIC_COUNT ; i++){				
				nodeHealth->statistics[i] = (*nodeStatistics)[i].toFloat();
				cout << "CurrentNodeStatistics: " << i << " " << nodeHealth->statistics[i] << endl;
				nodeHealth->statistics[i] = 111.0f;
			}
		}

		// Save recentIssues
		// TODO

		// Update knownIssues based on recentIssues
		// TODO

		if (anomalyDetected){
			anomaliesTriggered++;
		}
		// Trigger anomaly if any:

		{	// Disallow changes in registered plugins while cycling through triggers
			unique_lock<mutex> pluginLock( pluginMutex );

			if( anomalyDetected && ! persistingAnomaly ) {
				// cout << " Calling triggers!" << endl;
				for( auto itr = triggers.begin(); itr != triggers.end() ; itr++ ) {
					( *itr )->triggerResponseForAnomaly(false);
				}
			}
		}
		// TODO sometimes we'd like to have persistent Anomalies ... persistingAnomaly = anomalyDetected;



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

void ReasonerStandardImplementation::stop(){
	recentIssuesMutex.lock();
	terminated = true;
	running_condition.notify_one();
	recentIssuesMutex.unlock();
	periodicThread.join();
	delete(comm);

	Reasoner::stop();
}

void ReasonerStandardImplementation::start(){
	ReasonerStandardImplementationOptions & options = getOptions<ReasonerStandardImplementationOptions>();

	StatisticsCollector * statColl = GET_INSTANCE(StatisticsCollector, options.statisticsCollector);
	if ( statColl != nullptr ){
		// request everything we'll need.

		nodeStatistics = StatisticsCollection::makeCollection(statColl, {{
			{"utilization/cpu", "@localhost"},
			{"utilization/memory", "@localhost"}, // Alternative: {"utilization/memory/vm", "@localhost"},
			{"utilization/io", "@localhost"},
			{"utilization/network/send", "@localhost"},
			{"utilization/network/receive", "@localhost"},
			{"time/cpu/RuntimeUnhalted", "@localhost"}, // CONSUMED_CPU_SECONDS
// #ifdef ENABLE_LIKWID_POWER			
			{"power/rapl", "@localhost"}, // you may replace this with utilization/cpu to make it runnable :-)
// #else
			//{"utilization/cpu", "@localhost"}, // stupid replacement for the energy metric...
// #endif ENABLE_LIKWID_POWER		
			{"utilization/memory", "@localhost"}, // CONSUMED_MEMORY_BYTES
			// If likwid is used to observe the memory throughput the correct counter could be used:
			//{"quantity/memory/volume", "@localhost"}, // CONSUMED_MEMORY_BYTES
			{"quantity/network/volume", "@localhost"}, // CONSUMED_NETWORK_BYTES
			{"quantity/io/volume", "@localhost"}, // CONSUMED_IO_BYTES
		}}, true);
	}

	comm = new ReasonerCommunication(*this);
	comm->init( options.communicationOptions );

	periodicThread = thread( & ReasonerStandardImplementation::PeriodicRun, this );

	Reasoner::start();
}

ReasonerStandardImplementation::~ReasonerStandardImplementation() {
	if (childProcessesHealthMap) delete(childProcessesHealthMap);
	if (childNodesHealthMap) delete(childNodesHealthMap);
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
		case ReasonerStandardImplementationOptions::Role::NONE:
			break;
	}

	update_intervall_ms = options.update_intervall_ms;
	upstreamReasonerExists = (options.communicationOptions.upstreamReasoner != "");
}


shared_ptr<HealthStatistics> ReasonerStandardImplementation::queryRuntimePerformanceIssues(){
	return gatheredStatistics;
}


ComponentReport ReasonerStandardImplementation::prepareReport() {

	ComponentReport result;

	{	// Disallow other access to aggregated data fields
		unique_lock<mutex> dataLock( dataMutex );

		result.addEntry( "ANOMAL_RUNTIME_MS", ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( anomaliesTriggered * update_intervall_ms )));
		result.addEntry( "OBSERVED_RUNTIME_MS", ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( cyclesTriggered * update_intervall_ms )));		

		result.addEntry( "STATES_SENT_UPSTREAM", ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO,  nPushesSent));
		result.addEntry( "STATES_RECEIVED", ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO,  nPushesReceived));

		if ( nPushesReceived > 0 ){
			const char * text [] = {"CPU_SECONDS", "ENERGY_JOULE", "MEMORY_MIB", "NETWORK_MIB", "IO_MIB"};
			const float conversionFactor[] = {1.0f, 1.0f, 1.0f/1024/1024, 1.0f/1024/1024, 1.0f/1024/1024};
			for (int i=0; i < NODE_STATISTIC_COUNT; i++){
				result.addEntry( text[i], ReportEntry( ReportEntry::Type::APPLICATION_PERFORMANCE,  node_statistics[i] * conversionFactor[i] ));		
			}
		}

		// for debugging
		ostringstream reportText;
		reportText << this;
		result.addEntry( "TEXT_STATE", ReportEntry( ReportEntry::Type::SIOX_INTERNAL_DEBUG, VariableDatatype( reportText.str() )));
	}

	return result;
}


std::ostream & operator<<( std::ostream & os, const ReasonerStandardImplementation * r )
{
	ostringstream result;

	result << "\"" << r->id << "\":" << endl;
	switch ( r->role ){
		case ReasonerStandardImplementationOptions::Role::PROCESS :
			result << (*(r->processHealth));
			break;
		case ReasonerStandardImplementationOptions::Role::NODE :
			result << (*(r->nodeHealth));
			break;
		case ReasonerStandardImplementationOptions::Role::SYSTEM :
			result << (*(r->systemHealth));
			break;
		default:
			assert(false && "Invalid reasoner role!");
	}
	result << "Pushes: -> " << r->nPushesSent << ", <- " << r->nPushesReceived << endl;

	return os << result.str();
}





void ReasonerStandardImplementation::assessNodeHealth(){
/*
	// Input: node statistics, process health, system health
	// Configuration: global reasoner address, local address
	// Result: node health
*/
	// cout << "Reasoner " << id << " assessing NODE health..." << endl;


	// Create fresh NodeHealth to hold our results
	shared_ptr<NodeHealth> newHealth = make_shared<NodeHealth>();

	// determine node health based on the historic knowledge AND the statistics
	if ( nodeHealth->occurrences[ABNORMAL_FAST] || nodeHealth->occurrences[ABNORMAL_SLOW] || nodeHealth->occurrences[ABNORMAL_OTHER] ){
		// we have a condition in which we must fire the anomaly trigger.
		// cout << "Checkpoint 1a passed..." << endl;
		if ( observationRatios[ABNORMAL_FAST] > 5 &&
			observationRatios[ABNORMAL_FAST] > 2 * observationRatios[ABNORMAL_SLOW] &&
			observationRatios[ABNORMAL_FAST] > 2 * observationRatios[ABNORMAL_OTHER] )
		{
			newHealth->overallState = HealthState::ABNORMAL_FAST;
		}else if (observationRatios[ABNORMAL_SLOW] > 5 &&
			observationRatios[ABNORMAL_SLOW] > 2 * observationRatios[ABNORMAL_FAST] &&
			observationRatios[ABNORMAL_SLOW] > 2 * observationRatios[ABNORMAL_OTHER] )
		{
			newHealth->overallState = HealthState::ABNORMAL_SLOW;
		}else{
			newHealth->overallState = HealthState::ABNORMAL_OTHER;
		}
	}else{
		// cout << "Checkpoint 1b passed..." << endl;
		// normal condition
		// We have to pick between FAST, OK & SLOW
		if ( observationRatios[FAST] > 5 &&
			observationRatios[FAST] > 3*observationRatios[SLOW] )
		{
			// cout << "Checkpoint 1ba passed..." << endl;
			newHealth->overallState = HealthState::FAST;
		}else if ( observationRatios[SLOW] > 5 &&
			observationRatios[SLOW] > 3*observationRatios[FAST] )
		{
			// cout << "Checkpoint 1bb passed..." << endl;
			newHealth->overallState = HealthState::SLOW;
		}else{
			// cout << "Checkpoint 1bc passed..." << endl;
			newHealth->overallState = HealthState::OK;
		}
	}

	// cout << "Checkpoint 2 passed..." << endl;
	// If the current state is not OK and a statistics behaves suboptimal, we can take this into account and potentially add new issues and even set the health state to OK.
	if ( newHealth->overallState != HealthState::OK ){
		uint totalUtilization = 0;

		if ( newHealth->overallState == HealthState::SLOW || newHealth->overallState == HealthState::ABNORMAL_SLOW || newHealth->overallState == HealthState::ABNORMAL_OTHER ){

			for(int i=0; i < UTILIZATION_STATISTIC_COUNT; i++ ){
				totalUtilization += newHealth->utilization[i];
				if ( newHealth->utilization[i] > 80 ){ // if more than 80% utilization in one category
					// overloaded!
					newHealth->negativeIssues.push_back( { toString( (UtilizationIndex) i) + " overloaded", 1, 0 } );
					// actually the decision should depend on this nodes role. A compute node is expected to have a high CPU and MEMORY utilization...
					// TODO
					newHealth->overallState = HealthState::OK;
				}
			}
			if ( totalUtilization > 60*4 ){ // the overall system is overloaded
				// do we really need this? More sophisticated decisions would be good.
			}
		}
	}
/*
*/

	nodeHealth = newHealth;
	// cout << "State of resoner " << id << ": " << toString(nodeHealth->overallState) << endl;
}


void ReasonerStandardImplementation::assessProcessHealth(){
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

	// cout << "Reasoner " << id << " assessing PROCESS health..." << endl;

	shared_ptr<ProcessHealth> newHealth = make_shared<ProcessHealth>();

	// FIXME: Implementieren!

	processHealth = newHealth;
	// cout << "State of resoner " << id << ": " << toString(processHealth->overallState) << endl;
}



void ReasonerStandardImplementation::assessSystemHealth(){
	// Input: node health
	// Configuration: set of nodes belonging to each file system (at the moment we consider only  one file system)
	// Result: system health
	// Test case
	// Three nodes report overlapping issues.

	// cout << "Reasoner " << id << " assessing SYSTEM health..." << endl;

	shared_ptr<SystemHealth> newHealth = make_shared<SystemHealth>();

	// FIXME: Implementieren!

	systemHealth = newHealth;
	// cout << "State of resoner " << id << ": " << toString(systemHealth->overallState) << endl;
}


} // namespace knowledge

extern "C" {
	void * KNOWLEDGE_REASONER_INSTANCIATOR_NAME()
	{
		return new knowledge::ReasonerStandardImplementation();
	}
}
