#include <monitoring/statistics/multiplexer/StatisticsMultiplexer.hpp>
#include <monitoring/statistics/multiplexer/StatisticsMultiplexerPluginImplementation.hpp>
#include <knowledge/activity_plugin/ActivityPluginDereferencing.hpp>
#include <monitoring/ontology/OntologyDatatypes.hpp>
#include <monitoring/datatypes/Topology.hpp>

// #include <monitoring/statistics/StatisticsCollection.hpp>
// #include <monitoring/statistics/collector/StatisticsCollector.hpp>

#include <monitoring/statistics/multiplexer/plugins/statisticsHealthADPI/StatisticsHealthADPIOptions.hpp>

#include <string>


using namespace std;
using namespace monitoring;
using namespace core;

//#define DEBUG

#ifndef DEBUG
#define OUTPUT(...)
#else
#define OUTPUT(...) do { cout << "[Statistics Health ADPI] " << __VA_ARGS__ << "\n"; } while(0)
#endif

#define ERROR(...) do { cerr << "[Statistics Health ADPI] " << __VA_ARGS__ << "!\n"; } while(0)


class StatisticsHealthADPI : public StatisticsMultiplexerPlugin {

	public:

		virtual void initPlugin() throw();

		virtual ComponentOptions * AvailableOptions();

		virtual void notifyAvailableStatisticsChange( const vector<shared_ptr<Statistic> > & statistics, bool addedStatistics, bool removedStatistics ) throw();
		virtual void newDataAvailable() throw();

		~StatisticsHealthADPI();


	private:

		/*
		 * Fields to hold information on the statistics we are to watch, as per our options
		 */
		// A flag indicating whether all statistics requested are being obtained right now.
		// Used for easier checking when those available change.
		bool gotAllRequested = false;
		// Ontology and topology paths of the statistics to watch
		vector<string> requestedOntologyPaths;
		vector<string> requestedTopologyPaths;
		// Ontology and topology IDs of the statistics to watch
		vector<OntologyAttributeID> requestedOntologyIDs;
		vector<TopologyObjectId> requestedTopologyIDs;
		// The names of the statistics to watch
		vector<string> 	statisticsNames;
		// Pointers to the actual statistics objects currently being watched;
		// each object comes with a sliding history of values at different granularities
		vector<shared_ptr<Statistic>> statistics;

		/*
		 * Fields to hold the statistics values delivered to us
		 */
		// The latest values of the respective statistics
		vector<StatisticsValue> statisticsValues;
		// A field holding the indices of the statistics that are available at the moment.
		// Used to facilitate selctive updating of only those values available.
		vector<size_t> availableStatisticsIndices;
};


StatisticsHealthADPI::~StatisticsHealthADPI(){
}


void StatisticsHealthADPI::initPlugin() throw() {
	// Retrieve options and any other module links necessary
	StatisticsHealthADPIOptions &  options = getOptions<StatisticsHealthADPIOptions>();
	ActivityPluginDereferencing * facade = GET_INSTANCE( ActivityPluginDereferencing, options.dereferencingFacade );
	Topology * topology = facade->topology();

	for ( auto request : options.requestedStatistics )
	{
		/*
		 * Look up and remember information for all requested statistics
		 */
		// Remember the statistics we are requested to watch
		requestedOntologyPaths.push_back( request.first );
		requestedTopologyPaths.push_back( request.second );
		// Find and remember ontology id
		int lastSlashPosition = request.first.rfind( "/" );
		string ontologyDomainName = request.first.substr(0, lastSlashPosition);
		string ontologyAttributeName = request.first.substr(lastSlashPosition);
		OUTPUT( "Domain: " << ontologyDomainName );
		OUTPUT( "Attribute: " << ontologyAttributeName );
		OntologyAttribute ontologyAttribute = facade->lookup_attribute_by_name( ontologyDomainName, ontologyAttributeName );
		requestedOntologyIDs.push_back( ontologyAttribute.aID );
		// Find and remember topology id
		TopologyObject topologyObject = topology->lookupObjectByPath( request.second );
		requestedTopologyIDs.push_back( topologyObject.id() );
		// Remember a handy name for statistic; used for readable output
		statisticsNames.push_back( request.first + request.second );

		// Prepare a variable to hold the statistic's current values
		statisticsValues.push_back( StatisticsValue( 0.0 ) );
	}
}


ComponentOptions* StatisticsHealthADPI::AvailableOptions() {
	return new StatisticsHealthADPIOptions();
}


/*
 * The set of statistics available to us has changed.
 */
void StatisticsHealthADPI::notifyAvailableStatisticsChange( const vector<shared_ptr<Statistic> > & offeredStatistics, bool addedStatistics, bool removedStatistics ) throw(){

	OUTPUT( "Fresh statistics catalogue with " << offeredStatistics.size() << " entries." );

	if( gotAllRequested && !removedStatistics ) // We're happy - no need for action.
		return;

	// Remove all statistics, and start anew with whatever we're offered,
	// picking only those statistics we are to watch as per our options.
	availableStatisticsIndices.clear();
	for( auto s: offeredStatistics )
	{
		OUTPUT( "Checking statistic [ontID=" << s->ontologyId << ",topID=" << s->topologyId << "]." );
		for( size_t i = 0; i < requestedOntologyIDs.size(); i++ )
		{
			if( s->ontologyId == requestedOntologyIDs[i]
			   && s->topologyId == requestedTopologyIDs[i] )
			{
				// Assign the statistic object to the proper index
				statistics[i] = s;
				// Remember this statistic as being available
				availableStatisticsIndices.push_back( i );
				break;
			}
		}
	}

	// FIXME:
	// Make the test work again, if possible!
	//
	// Subscribe to a set of statistics
	// const std::vector<std::pair<std::string, std::string>>& ontologyAttributeTopologyPathPairs = {{
	// 		{"utilization/cpu", "@localhost"},
	// 		{"utilization/memory", "@localhost"}, // Alternative: {"utilization/memory/vm", "@localhost"},
	// 		{"utilization/io", "@localhost"},
	// 		{"utilization/network/send", "@localhost"},
	// 		{"utilization/network/receive", "@localhost"},
	// 		// {"time/cpu", "@localhost"}, // CONSUMED_CPU_SECONDS
	// 		// {"utilization/cpu", "@localhost"}, // power/rapl
	// 		// {"quantity/memory/volume", "@localhost"}, // CONSUMED_MEMORY_BYTES
	// 		// {"quantity/network/volume", "@localhost"}, // CONSUMED_NETWORK_BYTES
	// 		// {"quantity/io/volume", "@localhost"}, // CONSUMED_IO_BYTES
	// 	}};
	//
	// statistics.clear();
	// for( auto s : offeredStatistics )
	// {
	// 	statistics.push_back( s );
	// 	statisticsValues.push_back( StatisticsValue(0.0) );
	// }

	OUTPUT( "Statistics catalogue now holds " << statistics.size() << " entries." );

	gotAllRequested = ( requestedOntologyIDs.size() == availableStatisticsIndices.size() );
}


void StatisticsHealthADPI::newDataAvailable() throw(){
	// TODO:
	// Make sense of this?!
	//
	// uint64_t timestamp = (*statistics->begin())->curTimestamp();
	// *oa << timestamp ;
	// uint64_t size = statistics->size();
	// *oa << size;
	//
	// // write out all currently existing statistics
	// for(auto itr = statistics->begin(); itr != statistics->end(); itr++){
	// 	StatisticsDescription & sd = **itr;
	// 	*oa << sd;
	// 	*oa << (*itr)->curValue;
	// }

	OUTPUT( "Received new data!" );
	OUTPUT( "Statistics being watched: " << statisticsValues.size() );

	// Copy values received into local store
	for(size_t j, i=0; i < availableStatisticsIndices.size() ; i++)
	{
		j = availableStatisticsIndices[i];
		statisticsValues[j] = statistics[j]->curValue;
		OUTPUT( "Current node statistic[" << statisticsNames[j] << "]: " << statisticsValues[j] );
	}

	// TODO:
	// Test them for problems
	// Flag any problems found to reasoner
}


#undef OUTPUT
#undef ERROR


extern "C" {
	void* MONITORING_STATISTICS_MULTIPLEXER_PLUGIN_INSTANCIATOR_NAME() {
		return new StatisticsHealthADPI();
	}
}
