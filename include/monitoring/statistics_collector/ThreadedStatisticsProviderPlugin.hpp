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
 The ThreadedStatisticsProviderPlugin gets a reference to the ThreadedStatisticsCollector.
 It registers itself in the Collector.
*/

} // end namespace

#define MONITORING_STATISTICS_PLUGIN_INTERFACE "monitoring_statistics_plugin"

#endif
