#include <iostream>

#include <core/module/module-loader.hpp>

#include <monitoring/statistics_collector/StatisticsProviderPlugin.hpp>
#include <monitoring/statistics_collector/ThreadedStatisticsCollector.hpp>




int main(int argc, char const *argv[]){
	StatisticsCollector * tsc = core::module_create_instance<StatisticsCollector>("", "ThreadedStatisticsCollector", THREADED_STATISTICS_COLLECTOR_INTERFACE);
	assert(o != nullptr);
	ThreadedStatisticsCollectorOptions & op = tsc->getOptions<ThreadedStatisticsCollectorOptions>();
	op.filename = "ThreadedStatisticsCollector.dat";
	tsc->init();
	string it1("Test 1");
	string it2("Test 2");

	}

	cout << "OK" << endl;	
	return 0;
}


