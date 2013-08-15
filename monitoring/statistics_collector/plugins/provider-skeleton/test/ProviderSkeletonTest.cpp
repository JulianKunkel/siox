#include <iostream>

#include <core/module/module-loader.hpp>
#include <monitoring/statistics_collector/StatisticsProviderPlugin.hpp>

#include <monitoring/statistics_collector/StatisticsCollector.hpp>

#include "../ProviderSkeletonOptions.hpp"

using namespace std;

using namespace monitoring;
using namespace core;

class StatisticsCollectorTester : public StatisticsCollector {
public:	
	virtual void registerPlugin(StatisticsProviderPlugin * plugin){

		auto list = plugin->availableMetrics();

		cout << "new plugin" << endl;

		for(int i=0; i < 5; i++){
			for(auto it = list.begin() ; it != list.end(); it ++){
				StatisticsProviderDatatypes & stat = *it;
				cout << i << " " << stat.metrics << ": " << stat.value << endl;				
			}
			plugin->nextTimestep();
		}
	}

	virtual void unregisterPlugin(StatisticsProviderPlugin * plugin){
		
	}


	virtual array<StatisticsValue,10> getStatistics(StatisticsIntervall intervall, StatisticsDescription & stat){
		return array<StatisticsValue,10>();
	}

	virtual StatisticsValue getRollingStatistics(StatisticsIntervall intervall, StatisticsDescription & stat){
		return StatisticsValue(1);
	}

	virtual StatisticsValue getReducedStatistics(StatisticsIntervall intervall, StatisticsDescription & stat, StatisticsReduceOperator op){
		return StatisticsValue(1);	
	}

	virtual StatisticsValue getStatistics(StatisticsIntervall intervall, StatisticsDescription & stat, StatisticsReduceOperator op){
		return StatisticsValue(1);
	}

	virtual list<StatisticsDescription> availableMetrics(){
		auto metrics = list<StatisticsDescription>();
		return metrics;
	}


	virtual void init(){

	}

	virtual ComponentOptions * AvailableOptions(){
		return new ComponentOptions();
	}
};

int main(int argc, char const *argv[]){
	StatisticsProviderPlugin * ps = module_create_instance<StatisticsProviderPlugin>("", "siox-monitoring-statisticsPlugin-providerskel" , MONITORING_STATISTICS_PLUGIN_INTERFACE);

	ps->getOptions<ProviderSkeletonOptions>().statisticsCollector.componentID = (ComponentReferenceID) new StatisticsCollectorTester();

	ps->init();

	delete(ps);

	cout << "OK" << endl;	
	return 0;
}


