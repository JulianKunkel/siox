#ifndef SPLUGIN_H
#define SPLUGIN_H

#include <string>
#include <iostream>
#include <list>
#include <boost/variant.hpp>


#include <monitoring/datatypes/StatisticsTypes.hpp>
#include <core/component/Component.hpp>


using namespace std;

namespace monitoring{


class StatisticsProviderPlugin : public core::Component{
private:
	StatisticsCollector * collector;

public:
	/*
	 The StatisticsProviderPlugin gets a reference to the StatisticsCollector.
	 It registers all the available metrics in the Collector.
	 */
	virtual void init(ComponentOptions * options){
		StatisticsProviderPluginOptions * o = (StatisticsProviderPluginOptions*) options;

		init(o);

		// now register this plugin on the collector
		collector = o->collector.instance<StatisticsCollector>();

		auto list = availableMetrics();

		for(auto it = list.begin() ; it != list.end(); it ++){
			StatisticsProviderDatatypes & stat = *it;
			collector->registerStatistics(stat);
		}
	}

	virtual ComponentOptions * get_options(){
		return new StatisticsProviderPluginOptions();
	}

	virtual void shutdown(){
		// deregister plugins from the collector
		for(auto it = list.begin() ; it != list.end(); it ++){						
			StatisticsProviderDatatypes & stat = *it;
			collector->unregisterStatistics(stat);
		}
	}


	/* */
	virtual void nextTimestep() = 0;

	/* For testing purpose the two methods are public, so you can use the plugin even without collector */
	virtual list<StatisticsProviderDatatypes> availableMetrics() = 0;

	virtual void init(StatisticsProviderPluginOptions * options){
		// default implementation is empty.
	}

};

} // end namespace

#define MONITORING_STATISTICS_PLUGIN_INTERFACE "monitoring_statistics_plugin"

#endif
