#include <monitoring/statistics/multiplexer/StatisticsMultiplexer.hpp>
#include <monitoring/statistics/multiplexer/StatisticsMultiplexerPluginImplementation.hpp>

#include <monitoring/statistics/StatisticsCollection.hpp>
#include <monitoring/statistics/collector/StatisticsCollector.hpp>

#include <monitoring/statistics/multiplexer/plugins/statisticsHealthADPI/StatisticsHealthADPIOptions.hpp>

using namespace std;
using namespace monitoring;
using namespace core;


#define OUTPUT(...) do { cout << "[Statistics Health ADPI] " << __VA_ARGS__ << "\n"; } while(0)
#define ERROR(...) do { cerr << "[Statistics Health ADPI] " << __VA_ARGS__ << "!\n"; } while(0)


class StatisticsHealthADPI : public StatisticsMultiplexerPlugin {

	public:

		virtual void initPlugin() throw();

		virtual ComponentOptions * AvailableOptions();

		virtual void notifyAvailableStatisticsChange( const vector<shared_ptr<Statistic> > & statistics, bool addedStatistics, bool removedStatistics ) throw();
		virtual void newDataAvailable() throw();

		~StatisticsHealthADPI();


	private:

		// A bridge object where we can subscribe to select statistics
		StatisticsCollector * statisticsCollector = nullptr;
		// A transfer object used to atomically pass the actual values to us
		StatisticsCollection * nodeStatistics = nullptr;
		// A flag to indicate whether we receive the actual values via a StatisticsCollection, or not.
		bool useCollection = false;

		/*
		 * Fields to hold the statistics values delivered to us
		 */
		// If using a StatisticsCollection
		vector<string> statisticsNames; // The names of the statistics to watch
		vector<StatisticsValue> statisticsValues; // The latest values of the respective statistics
		// If not using a StatisticsCollection
		vector<shared_ptr<Statistic>> statistics; // Pointers to the statistics, complete with history
};


StatisticsHealthADPI::~StatisticsHealthADPI(){
}


void StatisticsHealthADPI::initPlugin() throw() {
	// Retrieve options
	StatisticsHealthADPIOptions &  o = getOptions<StatisticsHealthADPIOptions>();

	// Subscribe to a set of statistics
	statisticsCollector = GET_INSTANCE( StatisticsCollector, o.statisticsCollector );
	if ( statisticsCollector != nullptr )
	{
		OUTPUT( "We have a StatisticsCollector!" );
		// TODO:
		// Read the statistics to subscribe to from our options, handling unavailable ones gracefully!
		// For now, manually set the statistics we subscribe to.
		const std::vector<std::pair<std::string, std::string>>& ontologyAttributeTopologyPathPairs = {{
				{"utilization/cpu", "@localhost"},
				{"utilization/memory", "@localhost"}, // Alternative: {"utilization/memory/vm", "@localhost"},
				{"utilization/io", "@localhost"},
				{"utilization/network/send", "@localhost"},
				{"utilization/network/receive", "@localhost"},
				// {"time/cpu", "@localhost"}, // CONSUMED_CPU_SECONDS
				// {"utilization/cpu", "@localhost"}, // power/rapl
				// {"quantity/memory/volume", "@localhost"}, // CONSUMED_MEMORY_BYTES
				// {"quantity/network/volume", "@localhost"}, // CONSUMED_NETWORK_BYTES
				// {"quantity/io/volume", "@localhost"}, // CONSUMED_IO_BYTES
			}};

		// Subscribe to all statistics we are to watch
		nodeStatistics = StatisticsCollection::makeCollection(statisticsCollector, ontologyAttributeTopologyPathPairs, true);

		for ( auto statisticsString : ontologyAttributeTopologyPathPairs )
		{
			// Add statistic's name and a start value for it to our lists
			statisticsNames.push_back(statisticsString.first + statisticsString.second);
			statisticsValues.push_back(StatisticsValue(0.0));
		}

		useCollection = true;
	}
	else
	{
		OUTPUT( "We don't have a StatisticsCollector!" );
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

	if( useCollection )
	{
		// TODO:
		// Should any statistics our options tell us we are to monitor become available after being unavailable,
		// add them to our collection; should any we subscribed to have become unavailable, unsubscribe.
		if( addedStatistics )
		{
			 OUTPUT( "New statistic(s) became available!" );
		}
		if( removedStatistics )
		{
			 OUTPUT( "Old statistic(s) became unavailable!" );
		}
	}
	else
	{
		// TODO:
		// Pick only those statistics we are to watch as per our options
		// For now: Remove all statistics, and start anew with whatever we're offered.
		if( addedStatistics )
		{
			 OUTPUT( "New statistic(s) became available!" );
		}
		if( removedStatistics )
		{
			 OUTPUT( "Old statistic(s) became unavailable!" );
		}
		statistics.clear();
		for( auto s : offeredStatistics )
		{
			OUTPUT( "Adding statistic [ontID=" << s->ontologyId << ",topID=" << s->topologyId << "]." );
			statistics.push_back( s );
			statisticsValues.push_back( StatisticsValue(0.0) );
		}
	}

	OUTPUT( "Statistics catalogue now holds " << statistics.size() << " entries." );
}


void StatisticsHealthADPI::newDataAvailable() throw(){
	// uint64_t timestamp = (*statistics->begin())->curTimestamp();
	// *oa << timestamp ;
	// uint64_t size = statistics->size();
	// *oa << size;

	// // write out all currently existing statistics
	// for(auto itr = statistics->begin(); itr != statistics->end(); itr++){
	// 	StatisticsDescription & sd = **itr;
	// 	*oa << sd;
	// 	*oa << (*itr)->curValue;
	// }

	OUTPUT( "Received new data!" );
	// TODO:
	// Copy values received into local store
	OUTPUT( "Statistics being watched: " << statisticsValues.size() );
	if( useCollection ) {
		nodeStatistics->fetchValues();
		for(size_t i=0; i < statisticsValues.size() ; i++){
			statisticsValues[i] = (*nodeStatistics)[i];
			OUTPUT( "Current node statistic[" << statisticsNames[i] << "]: " << statisticsValues[i] );
		}
	}
	else
	{
		for(size_t i=0; i < statistics.size() ; i++)
		{
			statisticsValues[i] = statistics[i]->curValue;
			OUTPUT( "Current node statistic[" /*<< statisticsNames[i] <<*/ "]: " << statisticsValues[i] );
		}
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
