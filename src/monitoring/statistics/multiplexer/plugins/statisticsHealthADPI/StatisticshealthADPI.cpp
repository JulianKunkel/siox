#include <monitoring/statistics/multiplexer/StatisticsMultiplexer.hpp>
#include <monitoring/statistics/multiplexer/StatisticsMultiplexerPluginImplementation.hpp>

#include <monitoring/statistics/StatisticsCollection.hpp>

#include <monitoring/statistics/multiplexer/plugins/statisticsHealthADPI/StatisticshealthADPIOptions.hpp>

using namespace std;
using namespace monitoring;
using namespace core;

class StatisticsHealthADPI : public StatisticsMultiplexerPlugin {
	public:
		virtual void initPlugin() throw();

		virtual ComponentOptions * AvailableOptions();

		virtual void notifyAvailableStatisticsChange( const vector<shared_ptr<Statistic> > & statistics, bool addedStatistics, bool removedStatistics ) throw();
		virtual void newDataAvailable() throw();

		~StatisticsHealthADPI();
	private:

};

StatisticsHealthADPI::~StatisticsHealthADPI(){
}

void StatisticsHealthADPI::initPlugin() throw() {
	StatisticsHealthADPIOptions &  o = getOptions<StatisticsHealthADPIOptions>();

}

ComponentOptions* StatisticsHealthADPI::AvailableOptions() {
	return new StatisticsHealthADPIOptions();
}

void StatisticsHealthADPI::notifyAvailableStatisticsChange( const vector<shared_ptr<Statistic> > & statistics, bool addedStatistics, bool removedStatistics ) throw(){
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
}

extern "C" {
	void* MONITORING_STATISTICS_MULTIPLEXER_PLUGIN_INSTANCIATOR_NAME() {
		return new StatisticsHealthADPI();
	}
}
