#ifndef STATISTICS_PROVIDER_PLUGIN_H
#define STATISTICS_PROVIDER_PLUGIN_H

#include <string>
#include <iostream>
#include <list>
#include <boost/variant.hpp>

#include <core/component/Component.hpp>
#include <monitoring/statistics_collector/StatisticsCollector.hpp>
#include <monitoring/datatypes/StatisticsTypes.hpp>
#include <monitoring/statistics_collector/StatisticsProviderPluginOptions.hpp>


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
		collector = o->statisticsCollector.instance<StatisticsCollector>();

		assert(collector != nullptr);

		collector->registerPlugin(this);
	}

	virtual void shutdown(){		
		collector->unregisterPlugin(this);
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
