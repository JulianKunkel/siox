#include <monitoring/statistics/collector/StatisticsProviderPluginImplementation.hpp>

#include "ProviderSkeletonOptions.hpp"


using namespace std;
using namespace monitoring;
using namespace core;

class ProviderSkeleton: public StatisticsProviderPlugin {

		StatisticsValue i = ( int32_t ) 0;
		StatisticsValue f = ( double ) 0.4;

		virtual ComponentOptions * AvailableOptions() override {
			return new ProviderSkeletonOptions();
		}

		virtual void nextTimestep() override {
			int32_t cur = i.int32();
			i = cur + 1;

			double f2 = f.dbl();
			f = f2 * 2;
		}

		virtual vector<StatisticsProviderDatatypes> availableMetrics() override {
			vector<StatisticsProviderDatatypes> result;

			result.push_back( {SOFTWARE_SPECIFIC, GLOBAL, "test/metrics", LOCALHOST, i, GAUGE, "%", "test desc", 0, 0} );
			result.push_back( {HARDWARE_SPECIFIC, NODE, "test/weather", LOCALHOST PATH_SEP "weather-station:0", f, INCREMENTAL, "unit", "desc2", 0, 0} );
			result.push_back( {HARDWARE_SPECIFIC, DEVICE, "test/hdd", LOCALHOST PATH_SEP "sda:1", f, INCREMENTAL, "GB/s", "Throughput", 0, 0} );

			return result;
		}

		virtual void init( StatisticsProviderPluginOptions * options ) {
			// ProviderSkeletonOptions * o = (ProviderSkeletonOptions*) options;
			// no options in the skeleton
		}
};

extern "C" {
	void * MONITORING_STATISTICS_PLUGIN_INSTANCIATOR_NAME()
	{
		return new ProviderSkeleton();
	}
}
