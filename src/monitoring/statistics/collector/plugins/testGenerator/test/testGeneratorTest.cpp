#include <iostream>

#include <core/module/ModuleLoader.hpp>

#include <monitoring/statistics/collector/StatisticsProviderPlugin.hpp>
#include <monitoring/statistics/collector/StatisticsCollector.hpp>

#include <monitoring/statistics/collector/plugins/testGenerator/TestGeneratorOptions.hpp>


using namespace std;

using namespace monitoring;
using namespace core;


int main( int argc, char const * argv[] )
{
	StatisticsProviderPlugin * plugin = module_create_instance<StatisticsProviderPlugin>( "", "siox-monitoring-statisticsPlugin-testGenerator" , MONITORING_STATISTICS_PLUGIN_INTERFACE );

	TestGeneratorOptions options;
	plugin->init(& options);

	auto list = plugin->availableMetrics();

	cout << "testGenerator plugin" << endl;

	for (int c = 0; c < 4; c++ ){

		plugin->nextTimestep();
		for( auto it = list.begin() ; it != list.end(); it ++ ) {
			StatisticsProviderDatatypes & stat = *it;
			cout << stat.topologyPath << " ";
			cout << stat.metrics << ": " << stat.value << " " << stat.si_unit << endl;
		}
		cout << endl;
	}

	delete( plugin );

	cout << "OK" << endl;
	return 0;
}


