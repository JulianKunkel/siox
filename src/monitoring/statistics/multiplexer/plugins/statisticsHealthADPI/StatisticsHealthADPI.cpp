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

		// Fields to hold the statistics delivered to us
		vector<string> statisticsNames; // The names of the statistics to watch
		vector<StatisticsValue> statisticsValues; // The latest values of the respective statistics

};


StatisticsHealthADPI::~StatisticsHealthADPI(){
}


void StatisticsHealthADPI::initPlugin() throw() {
	// Retrieve options
	StatisticsHealthADPIOptions &  o = getOptions<StatisticsHealthADPIOptions>();

	// Subscribe to a set of statistics
	statisticsCollector = GET_INSTANCE( StatisticsCollector, o.statisticsCollector );
	if ( statisticsCollector != nullptr ){
		// Request everything we'll need, according to our options
		nodeStatistics = StatisticsCollection::makeCollection(statisticsCollector, {{
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
		}}, true);
	}

	// TODO:
	// Read the statistics to subscribe to from our options, handling unavailable ones gracefully!
}


ComponentOptions* StatisticsHealthADPI::AvailableOptions() {
	return new StatisticsHealthADPIOptions();
}


/*
 * The set of statistics available to us has changed.
 */
void StatisticsHealthADPI::notifyAvailableStatisticsChange( const vector<shared_ptr<Statistic> > & statistics, bool addedStatistics, bool removedStatistics ) throw(){
	/*
	 * Right now, we are not interested in any statistic besides the one we already subscribed to.
	 */

	// TODO:
	// Should any statistics our options tell us we should monitor become available after being unavailable,
	// add them to our collection; should any we subscribed to have become unavailable, unsubscribe.
	 OUTPUT( "New statistics available!" );
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
	if( nodeStatistics != nullptr ) {
		nodeStatistics->fetchValues();
		for(size_t i=0; i < statisticsValues.size() ; i++){
			statisticsValues[i] = (*nodeStatistics)[i];
			OUTPUT( "Current node statistic[" << statisticsNames[i] << "]: " << statisticsValues[i] );
		}
	}

	// TODO:
	// Test them for problems
	// Flag any problems fround to reasoner
}


#undef OUTPUT
#undef ERROR


extern "C" {
	void* MONITORING_STATISTICS_MULTIPLEXER_PLUGIN_INSTANCIATOR_NAME() {
		return new StatisticsHealthADPI();
	}
}
