#include <knowledge/reasoner/modules/ReasonerStandardImplementation.hpp>
#include <monitoring/statistics/collector/StatisticsCollector.hpp>
#include <util/threadSafety.h>

#include <iostream>

using namespace std;
using namespace core;
using namespace monitoring;


#define DEBUG

#ifndef DEBUG
#define OUTPUT(...)
#else
// #define OUTPUT(...) do { cout << "[Reasoner " << id << ", role " << role << "] " << __VA_ARGS__ << "\n"; } while(0)
#define OUTPUT(...) do { cout << "[Reasoner " << id << "] " << __VA_ARGS__ << "\n"; } while(0)
#endif

#define ERROR(...) do { cerr << "[Reasoner " << id << ", role " << role << "] " << __VA_ARGS__ << "!\n"; } while(0)


namespace knowledge {

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
			// OUTPUT( "Received status from process reasoner " << data.reasonerID );
		}
		else if ( data.reasonerID == "INJECT" ) // Workaround for testing!
			processHealth = make_shared<ProcessHealth>( health );
		else
			ERROR( "Received inappropriate message from " << data.reasonerID << "!" );

		nPushesReceived++;
	}
}


void ReasonerStandardImplementation::receivedReasonerNodeHealth(ReasonerMessageReceived & data, NodeHealth & health){
	{	// Disallow other access to aggregated data fields
		unique_lock<mutex> dataLock( dataMutex );

		if ( role == ReasonerStandardImplementationOptions::Role::SYSTEM ){
			(*childNodesHealthMap)[data.reasonerID] = health;
			// OUTPUT( "Received status from node reasoner " << data.reasonerID );
		}
		else if ( role == ReasonerStandardImplementationOptions::Role::PROCESS ){
			// aggregate statistics
			for (int i=0; i < NODE_STATISTIC_COUNT; i++){
				node_statistics[i] += health.statistics[i];
			}
			// OUTPUT( "CPU: " << health.statistics[CONSUMED_CPU_SECONDS] );
			// OUTPUT( "RAPL: " << health.statistics[CONSUMED_ENERGY_JOULE] );

			nodeHealth = make_shared<NodeHealth>(health);

		// OUTPUT( "Received status from node reasoner " << data.reasonerID );
		}
		else if ( data.reasonerID == "INJECT" ) // Workaround for testing!
			nodeHealth = make_shared<NodeHealth>( health );
		else
			ERROR( "Received inappropriate message from " << data.reasonerID << "!" );

		nPushesReceived++;
	}
}


void ReasonerStandardImplementation::receivedReasonerSystemHealth(ReasonerMessageReceived & data, SystemHealth & health){
	{	// Disallow other access to aggregated data fields
		unique_lock<mutex> dataLock( dataMutex );

		if ( role == ReasonerStandardImplementationOptions::Role::NODE ){
			// OUTPUT( "Received status from system reasoner " << data.reasonerID );
			systemHealth = make_shared<SystemHealth>(health);
		}
		else if ( data.reasonerID == "INJECT" ) // Workaround for testing!
			systemHealth = make_shared<SystemHealth>( health );
		else
			ERROR( "Received inappropriate message from " << data.reasonerID << "!" );
	}

		nPushesReceived++;
}


void ReasonerStandardImplementation::PeriodicRun(){
	monitoring_namespace_protect_thread();
	bool persistingAnomaly; // Last cycle's anomalyDetected
	bool anomalyDetected = false;

	while( ! terminated ) {
		// OUTPUT( "PeriodicRun started" );

		auto timeout = chrono::system_clock::now() + chrono::milliseconds( update_intervall_ms );

		cyclesTriggered++;

		// Remember last cycle's anomaly state, if persisting anomalies are switched on
		persistingAnomaly = anomalyDetected && kpersistentAnomalies;
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
					// OUTPUT( "ADPI: " << adpi );
					// Memory management passes to our responsibility here!
					unique_ptr<unordered_map<ComponentID, AnomalyPluginHealthStatistic>>  newObservations = adpi->queryRecentObservations();
					// OUTPUT( "Reports: " << gatheredStatistics->map.size() << " -> " << newObservations->size() );
					// Loop over all reports from current ADPI (by ComponentID)
					for (auto report : *newObservations ) {
						// OUTPUT( "Report for CID: " << report.first );
						// Merge this report into the global set
						// TODO: In Funktion auslagern

						// Merge occurrences into gatheredStatistics and local Health
						for ( int healthState=0; healthState < HEALTH_STATE_COUNT; healthState++ ){
							// OUTPUT( "HealthState = " << healthState );
							// OUTPUT( "Before: " << gatheredStatistics->map[report.first].occurrences[healthState] );
							gatheredStatistics->map[report.first].cid = report.first;
							gatheredStatistics->map[report.first].occurrences[healthState] += report.second.occurrences[healthState];
							// OUTPUT( "After:  " << gatheredStatistics->map[report.first].occurrences[healthState] );
							localHealth->occurrences[healthState] += report.second.occurrences[healthState];
							// OUTPUT( "Updated: " << gatheredStatistics->map[report.first] );

						}

						// Merge positive issues into gatheredStatistics and *localHealth
						unordered_map<string, HealthIssue> * sourceIssueMap = & (report.second.positiveIssues);
						unordered_map<string, HealthIssue> * targetIssueMap = & (gatheredStatistics->map[report.first].positiveIssues);
						list<HealthIssue> * targetIssueList = & (localHealth->positiveIssues);
						for ( auto issue : *sourceIssueMap )
						{
							// OUTPUT( "Received issue: " << issue.second );

							{ // gatheredStatistics
								auto precedent = targetIssueMap->find( issue.first );
								if ( precedent == targetIssueMap->end() )
								{
									(*targetIssueMap)[issue.first] = HealthIssue( issue.first, issue.second.occurrences, issue.second.delta_time_ms );
									// OUTPUT( "Created " << (*targetIssueMap)[issue.first] );
								}
								else
								{
									precedent->second.add(issue.second);
									// OUTPUT( "Added to " << precedent->second );
								}
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
							// OUTPUT( "Received issue: "<< issue.second );

							{ // gatheredStatistics
								auto precedent = targetIssueMap->find( issue.first );
								if ( precedent == targetIssueMap->end() )
								{
									(*targetIssueMap)[issue.first] = HealthIssue( issue.first, issue.second.occurrences, issue.second.delta_time_ms );
									// OUTPUT( "Created " << (*targetIssueMap)[issue.first] );
								}
								else
								{
									precedent->second.add(issue.second);
									// OUTPUT( "Added to " << precedent->second );
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
					// OUTPUT( "Reports: " << gatheredStatistics->map.size() << " -> " << newObservations->size() );
				}
			} // pluginLock

			// Remember last step's statistics for comparison
			oldObservationCounts = observationCounts;
			oldObservationRatios = observationRatios;
			oldObservationTotal = observationTotal;
			// Reset current counts and total
			observationTotal = 0;
			for ( int state = 0; state< HEALTH_STATE_COUNT; state++ )
				observationCounts[state] = 0;
			// Add all gathered statistics by health state
			for ( auto report : gatheredStatistics->map )
				for ( int state = 0; state < HEALTH_STATE_COUNT; state++ )
					observationCounts[state] += report.second.occurrences[state];
			// Total up current counts
			for ( int state = 0; state< HEALTH_STATE_COUNT; state++ )
				observationTotal += observationCounts[state];
			// Compute current ratios
			if( observationTotal > 0 ){
				float scale = 100.0 / observationTotal;
				for ( int state = 0; state< HEALTH_STATE_COUNT; state++ ){
					observationRatios[state] = observationCounts[state] * scale;
					// OUTPUT( observationCounts[state] << "=" << (int) observationRatios[state] << "% " );
				}
			}

			// Check whether anything happened
			if ( observationTotal != oldObservationTotal ){
				// OUTPUT( "Has news: " << oldObservationTotal << "->" << observationTotal );

				// Check whether anything interesting happened
				for (int state = 0; state < HEALTH_STATE_COUNT; ++state)
				{
					// OUTPUT( state << ": " << (int)oldObservationRatios[state] << "->" << (int)observationRatios[state] );
					// Did ratio of any non-OK state move by more than 2 points?
					if( state != HealthState::OK ){
						int diff = observationRatios[state] - oldObservationRatios[state];
						if( abs(diff) >= 2 ){
							anomalyDetected = true;
							// OUTPUT( "Detected an anomaly (moving observation ratio for state " << state << " from " << (int)oldObservationRatios[state] << " to "<< (int)observationRatios[state] << ", or " << observationCounts[state] << " out of " << observationTotal << ")!" );
						}
					}

					// Did number of any ABNORMAL state rise?
					if( state == HealthState::ABNORMAL_BAD

					   || state == HealthState::ABNORMAL_GOOD

					   || state == HealthState::ABNORMAL_OTHER ){
						if( observationCounts[state] > oldObservationCounts[state] ){
							anomalyDetected = true;
							// OUTPUT( "Detected an anomaly (abnormal state " << state << ")!" );
						}
					}
				}
			}

			// TODO combine health of the next level with my local information:
			switch (role){
				case ReasonerStandardImplementationOptions::Role::PROCESS :
					if ( nodeHealth->overallState != GOOD && nodeHealth->overallState != BAD && nodeHealth->overallState != OK ){
						anomalyDetected = true;
					}
					break;
			}



			// Run assessment appropriate to reasoner's role;
			// then, forward current state to remote reasoners
			switch ( role ) {

				case ReasonerStandardImplementationOptions::Role::PROCESS:
					assessProcessHealth();
					if (upstreamReasonerExists){
						// OUTPUT( "Pushing process state upstream!" );
						comm->pushProcessStateUpstream( processHealth);
						nPushesSent++;
					}
					break;

				case ReasonerStandardImplementationOptions::Role::NODE:
					assessNodeHealth();
					if (upstreamReasonerExists){
						// OUTPUT( "Pushing node state upstream!" );
						comm->pushNodeStateUpstream( nodeHealth);
						nPushesSent++;
					}
					break;

				case ReasonerStandardImplementationOptions::Role::SYSTEM:
					assessSystemHealth();
					if (upstreamReasonerExists){
						// OUTPUT( "Pushing system state upstream!" );
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
				// OUTPUT( "CurrentNodeStatistics: " << i << " " << nodeHealth->statistics[i] );
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

			if( anomalyDetected || persistingAnomaly ) {
				// OUTPUT( "New anomaly: " << anomalyDetected << "; persisting anomaly: " << persistingAnomaly );
				// OUTPUT( "Calling triggers!" );
				for( auto itr = triggers.begin(); itr != triggers.end() ; itr++ ) {
					( *itr )->triggerResponseForAnomaly(false);
				}
			}
		}

		// OUTPUT( "PeriodicRun() going to sleep..." );
		unique_lock<mutex> lock( recentIssuesMutex );
		// TODO: Understand, clarify and comment this!
		if( terminated ) {
			break;
		}
		running_condition.wait_until( lock, timeout );
	}
	// OUTPUT( "PeriodicRun finished" );
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
			{"time/cpu/UnhaltedConsumed", "@localhost"}, // CONSUMED_CPU_SECONDS
// #ifdef ENABLE_LIKWID_POWER
			{"energy/Socket/rapl", "@localhost"}, // you may replace this with utilization/cpu to make it runnable :-)
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
			const char * text [] = {
				"UTILIZATION_CPU",
				"UTILIZATION_MEMORY",
				"UTILIZATION_IO",
				"UTILIZATION_NETWORK_SEND",
				"UTILIZATION_NETWORK_RECEIVE",
				"CONSUMED_CPU_SECONDS",
				"CONSUMED_ENERGY_JOULE",
				"CONSUMED_MEMORY_BYTES",
				"CONSUMED_NETWORK_BYTES",
				"CONSUMED_IO_BYTES",
			};

			for (int i=0; i < NODE_STATISTIC_COUNT; i++){
				result.addEntry( text[i], ReportEntry( ReportEntry::Type::APPLICATION_PERFORMANCE,  node_statistics[i] ));
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
	// OUTPUT( "Reasoner " << id << " assessing NODE health..." );


	// Create fresh NodeHealth to hold our results
	shared_ptr<NodeHealth> newHealth = make_shared<NodeHealth>();

	// determine node health based on the historic knowledge AND the statistics
	if ( nodeHealth->occurrences[ABNORMAL_GOOD] || nodeHealth->occurrences[ABNORMAL_BAD] || nodeHealth->occurrences[ABNORMAL_OTHER] ){
		// we have a condition in which we must fire the anomaly trigger.
		if ( observationRatios[ABNORMAL_GOOD] >= 1 &&
			observationRatios[ABNORMAL_GOOD] > 2 * observationRatios[ABNORMAL_BAD] &&
			observationRatios[ABNORMAL_GOOD] > 2 * observationRatios[ABNORMAL_OTHER] )
		{
			newHealth->overallState = HealthState::ABNORMAL_GOOD;
		}else if (observationRatios[ABNORMAL_BAD] >= 1 &&
			observationRatios[ABNORMAL_BAD] > 2 * observationRatios[ABNORMAL_GOOD] &&
			observationRatios[ABNORMAL_BAD] > 2 * observationRatios[ABNORMAL_OTHER] )
		{
			newHealth->overallState = HealthState::ABNORMAL_BAD;
		}else{
			newHealth->overallState = HealthState::ABNORMAL_OTHER;
		}
	}else{
		// normal condition
		// We have to pick between GOOD, OK & BAD
		if ( observationRatios[GOOD] > 5 &&
			observationRatios[GOOD] > 3*observationRatios[BAD] )
		{
			newHealth->overallState = HealthState::GOOD;
		}else if ( observationRatios[BAD] > 5 &&
			observationRatios[BAD] > 3*observationRatios[GOOD] )
		{
			newHealth->overallState = HealthState::BAD;
		}else{
			newHealth->overallState = HealthState::OK;
		}
	}

	// If the current state is not OK and a statistics behaves suboptimal, we can take this into account and potentially add new issues and even set the health state to OK.
	if ( newHealth->overallState != HealthState::OK ){
		uint totalUtilization = 0;

		if ( newHealth->overallState == HealthState::BAD || newHealth->overallState == HealthState::ABNORMAL_BAD
 || newHealth->overallState == HealthState::ABNORMAL_OTHER ){

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
	// OUTPUT( "State of resoner " << id << ": " << toString(nodeHealth->overallState) );
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

	// OUTPUT( "Reasoner " << id << " assessing PROCESS health..." );

	shared_ptr<ProcessHealth> newHealth = make_shared<ProcessHealth>();

	// FIXME: Implementieren!

	processHealth = newHealth;
	// OUTPUT( "State of resoner " << id << ": " << toString(processHealth->overallState) );
}



void ReasonerStandardImplementation::assessSystemHealth(){
	// Input: node health
	// Configuration: set of nodes belonging to each file system (at the moment we consider only  one file system)
	// Result: system health
	// Test case
	// Three nodes report overlapping issues.

	// OUTPUT( "Reasoner " << id << " assessing SYSTEM health..." );

	shared_ptr<SystemHealth> newHealth = make_shared<SystemHealth>();

	// FIXME: Implementieren!

	systemHealth = newHealth;
	// OUTPUT( "State of resoner " << id << ": " << toString(systemHealth->overallState) );
}


} // namespace knowledge


#undef OUTPUT
#undef ERROR


extern "C" {
	void * KNOWLEDGE_REASONER_INSTANCIATOR_NAME()
	{
		return new knowledge::ReasonerStandardImplementation();
	}
}
