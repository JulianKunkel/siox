#include <monitoring/statistics/collector/StatisticsProviderPluginImplementation.hpp>


using namespace std;
using namespace monitoring;
using namespace core;

class IOstats: public StatisticsProviderPlugin {

		StatisticsValue i = ( int32_t ) 0;
		StatisticsValue f = ( double ) 0.4;

		virtual void nextTimestep() override {
			int32_t cur = i.int32();
			i = cur + 1;

			double f2 = f.dbl();
			f = f2 * 2;
		}

		virtual vector<StatisticsProviderDatatypes> availableMetrics() override {
			vector<StatisticsProviderDatatypes> result;

			result.push_back( {SOFTWARE_SPECIFIC, GLOBAL, "test/metrics", "@localhost", i, GAUGE, "%", "test desc", 0, 0} );
			result.push_back( {HARDWARE_SPECIFIC, NODE, "test/weather", "@localhost/weather-station:1", f, INCREMENTAL, "uhh", "desc2",  0, 0} );
			result.push_back( {HARDWARE_SPECIFIC, DEVICE, "test/metric", "@localhost/sda:1", f, INCREMENTAL, "GB/s", "Throughput", 0, 0} );

			return result;
		}
};

extern "C" {
	void * MONITORING_STATISTICS_PLUGIN_INSTANCIATOR_NAME()
	{
		return new IOstats();
	}
}
