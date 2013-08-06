#include <iostream>

#include <core/module/module-loader.hpp>

#include <monitoring/statistics_collector/StatisticsProviderPlugin.hpp>
#include <monitoring/statistics_collector/StatisticsCollector.hpp>


using namespace std;

using namespace monitoring;
using namespace core;


int main(int argc, char const *argv[]){
	StatisticsProviderPlugin * plugin = module_create_instance<StatisticsProviderPlugin>("", "CPUstats" , MONITORING_STATISTICS_PLUGIN_INTERFACE);

	plugin->init();

	auto list = plugin->availableMetrics();

	cout << "cpu plugin" << endl;


	plugin->nextTimestep();
	for(auto it = list.begin() ; it != list.end(); it ++){
		StatisticsProviderDatatypes & stat = *it;
		if ( stat.scope == DEVICE ){
			cout << stat.topology[1].second << " ";	
		}
		cout << stat.metrics << ": " << stat.value << " " << stat.si_unit << endl;				
	}
	
	delete(plugin);

	cout << "OK" << endl;	
	return 0;
}


