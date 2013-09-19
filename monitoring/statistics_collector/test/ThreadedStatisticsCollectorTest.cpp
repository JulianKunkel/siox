#include <iostream>

#include <core/module/ModuleLoader.hpp>

#include <monitoring/statistics_collector/StatisticsProviderPlugin.hpp>
#include <monitoring/statistics_collector/StatisticsCollector.hpp>

#include "../ThreadedStatisticsOptions.hpp"


int main( int argc, char const * argv[] )
{
	monitoring::StatisticsCollector * collector = core::module_create_instance<monitoring::StatisticsCollector>( "", "siox-monitoring-ThreadedStatisticsCollector", STATISTICS_COLLECTOR_INTERFACE );
	collector->init();
	monitoring::StatisticsProviderPlugin* plugin = module_create_instance<monitoring::StatisticsProviderPlugin>( "", "siox-monitoring-statisticsPlugin-providerskel", MONITORING_STATISTICS_PLUGIN_INTERFACE);
	plugin->init();

	collector->registerPlugin(plugin);
//	collector->availableMetrics();

	sleep(2);

	delete collector;
	delete plugin;

	cout << "OK" << endl;
	return 1;
}


