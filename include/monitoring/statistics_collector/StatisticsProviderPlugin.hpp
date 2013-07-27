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

/*
 The StatisticsProviderPlugin gets a reference to the StatisticsCollector.
 It registers itself in the Collector.
*/
class StatisticsProviderPlugin : public core::Component{
private:
	StatisticsCollector * collector;
protected:

	virtual ComponentOptions * AvailableOptions(){
		return new StatisticsProviderPluginOptions();
	}


	virtual void init(StatisticsProviderPluginOptions & options){
		// default implementation is empty. 
		// Override it to provide additional modulespecific options.
	}	
public:

	virtual void init(){
		StatisticsProviderPluginOptions o = getOptions<StatisticsProviderPluginOptions>();

		init(o);

		// now register this plugin on the collector
		collector = GET_INSTANCE(StatisticsCollector, o.statisticsCollector);

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

};

} // end namespace

#define MONITORING_STATISTICS_PLUGIN_INTERFACE "monitoring_statistics_plugin"

#endif
