#include "monitoring/statistics_collector/StatisticsProviderPluginImplementation.hpp"
#include "ProviderSkeletonOptions.hpp"
#include <core/component/ComponentReferenceSerializable.hpp>


using namespace std;
using namespace monitoring;
using namespace core;

// Create an implementation of the options.
CREATE_SERIALIZEABLE_CLS(ProviderSkeletonOptions)

class ProviderSkeleton: public StatisticsProviderPlugin{

	StatisticsValue i = (int32_t) 0;
	StatisticsValue f = (double) 0.4;

	virtual ComponentOptions * AvailableOptions(){
		return new ProviderSkeletonOptions();
	}

	virtual void nextTimestep(){
		int32_t cur = boost::get<int32_t>(i);		
		i = cur + 1;

		double f2 = boost::get<double>(f);
		f = f2 * 2;
	}

	virtual list<StatisticsProviderDatatypes> availableMetrics(){
		auto lst = list<StatisticsProviderDatatypes>();

		lst.push_back({SOFTWARE_SPECIFIC, GLOBAL, "test/metrics", {{"node", LOCAL_HOSTNAME}, {"semantics", "testing"}}, i, "%", "test desc", 100, 0, 0});
		lst.push_back({HARDWARE_SPECIFIC, NODE, "test/weather", {{"node", LOCAL_HOSTNAME}, {"tschaka", "test2"}}, f, "uhh", "desc2", 10, 0, 0});
		lst.push_back({HARDWARE_SPECIFIC, DEVICE, "test/metric", {{"node", LOCAL_HOSTNAME}, {"sda", "test3"}}, f, "GB/s", "Throughput", 50, 0, 0});

		return lst;
	}

	virtual void init(StatisticsProviderPluginOptions * options){
		ProviderSkeletonOptions * o = (ProviderSkeletonOptions*) options;
		// no options in the skeleton
	}
};

PLUGIN(ProviderSkeleton)

