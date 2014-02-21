#include <iostream>

#include <core/module/ModuleLoader.hpp>

#include <monitoring/statistics/collector/StatisticsProviderPlugin.hpp>
#include <monitoring/statistics/collector/StatisticsCollector.hpp>
#include <monitoring/statistics/collector/plugins/likwid/ProviderLikwidOptions.hpp>


using namespace std;

using namespace monitoring;
using namespace core;


int main( int argc, char const * argv[] )
{
	StatisticsProviderPlugin * plugin = module_create_instance<StatisticsProviderPlugin>( "",
	                                    "siox-monitoring-statisticsPlugin-likwid" , MONITORING_STATISTICS_PLUGIN_INTERFACE );

	ProviderLikwidOptions options;
	options.groups = "MEM";
	plugin->init(& options);

	auto list = plugin->availableMetrics();

	cout << "Likwid plugin" << endl;

	for(int i=0; i < 3 ; i++){
		cout << endl; 
		cout << "next timestep" << endl;
		plugin->nextTimestep();

		for( auto it = list.begin() ; it != list.end(); it ++ ) {
			StatisticsProviderDatatypes & stat = *it;
			cout << stat.topologyPath << " " << stat.metrics << ": " << stat.value << " " << stat.si_unit << endl;
		}
	}
	delete( plugin );

	cout << "OK" << endl;
	return 0;
}


