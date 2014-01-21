#include <iostream>

#include <core/module/ModuleLoader.hpp>

#include <monitoring/statistics/collector/StatisticsProviderPlugin.hpp>
#include <monitoring/statistics/collector/StatisticsCollector.hpp>


using namespace std;

using namespace monitoring;
using namespace core;


int main( int argc, char const * argv[] )
{
	StatisticsProviderPlugin * plugin = module_create_instance<StatisticsProviderPlugin>( "", "siox-monitoring-statisticsPlugin-OSMemUsage" , MONITORING_STATISTICS_PLUGIN_INTERFACE );

	StatisticsProviderPluginOptions options;
	plugin->init(options);

	auto list = plugin->availableMetrics();

	cout << "OSMemUsage plugin" << endl;


	plugin->nextTimestep();
	for( auto it = list.begin() ; it != list.end(); it ++ ) {
		StatisticsProviderDatatypes & stat = *it;
		if( stat.scope == DEVICE ) {
			cout << stat.topologyPath << " ";
		}
		cout << stat.metrics << ": " << stat.value << " " << stat.si_unit << endl;
	}

	delete( plugin );

	cout << "OK" << endl;
	return 0;
}


