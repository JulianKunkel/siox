#include <monitoring/statistics/collector/StatisticsProviderPluginImplementation.hpp>


using namespace std;
using namespace monitoring;
using namespace core;

class IOstats: public StatisticsProviderPlugin {

		StatisticsValue i = ( int32_t ) 0;
		StatisticsValue f = ( double ) 0.4;

		virtual void nextTimestep() throw() override {
			int32_t cur = i.int32();
			i = cur + 1;

			double f2 = f.dbl();
			f = f2 * 2;
		}

		virtual vector<StatisticsProviderDatatypes> availableMetrics() throw() override {
			vector<StatisticsProviderDatatypes> result;

			result.push_back( { "test/metrics", "@localhost", i, GAUGE, "%", "test desc"} );
			result.push_back( { "test/weather", "@localhosbt/weather-station:1", f, INCREMENTAL, "uhh", "desc2"} );
			result.push_back( { "test/metric", "@localhost/sda:1", f, INCREMENTAL, "GB/s", "Throughput"} );

			return result;
		}
};

extern "C" {
	void * MONITORING_STATISTICS_PLUGIN_INSTANCIATOR_NAME()
	{
		return new IOstats();
	}
}
