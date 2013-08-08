#include <monitoring/statistics_collector/StatisticsProviderPluginImplementation.hpp>

#include <core/component/ComponentReferenceSerializable.hpp>


using namespace std;
using namespace monitoring;
using namespace core;

class IOstats: public StatisticsProviderPlugin{

	StatisticsValue i = (int32_t) 0;
	StatisticsValue f = (double) 0.4;

	virtual void nextTimestep(){
		int32_t cur = i.int32();	
		i = cur + 1;

		double f2 = f.dbl();
		f = f2 * 2;
	}

	virtual list<StatisticsProviderDatatypes> availableMetrics(){
		auto lst = list<StatisticsProviderDatatypes>();

		lst.push_back({SOFTWARE_SPECIFIC, GLOBAL, "test/metrics", {{"node", LOCAL_HOSTNAME}, {"semantics", "testing"}}, i, GAUGE, "%", "test desc", 0, 0});
		lst.push_back({HARDWARE_SPECIFIC, NODE, "test/weather", {{"node", LOCAL_HOSTNAME}, {"tschaka", "test2"}}, f, INCREMENTAL, "uhh", "desc2",  0, 0});
		lst.push_back({HARDWARE_SPECIFIC, DEVICE, "test/metric", {{"node", LOCAL_HOSTNAME}, {"sda", "test3"}}, f, INCREMENTAL, "GB/s", "Throughput", 0, 0});

		return lst;
	}
};

PLUGIN(IOstats)

