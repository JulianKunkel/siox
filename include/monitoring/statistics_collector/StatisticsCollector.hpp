/**
 * @file    StatisticsCollector.hpp
 *
 * @description A (software) component for collecting statistical values.
 * @standard    Preferred standard is C++11
 *
 * @author Marc Wiedemann
 * @date   2013
 *
 */

/*!
Statistics Collector

Example Abstract Context Topologies plus Instance done by the collector and defined in the configuration:
I/O             Device          Throughput/network              (Node, Device) = ("<LOCAL_NODE>", "eth*")
Into:
I/O             Node            Throughput/network              (Node, Aggregate) = ("<LOCAL_NODE>", "Ethernet")

The second one is an aggregate of all available "Ethernet" devices.

Note that a plugin could offer the node aggregated info by itself.

Node statistics should always be collected as they are of general interest. 
On certain occasions such as a deeper second search phase after the general information is gathered, 
the specific device statistics can be optionally collected.
 */


#ifndef STATISTICS_COLLECTOR_H
#define STATISTICS_COLLECTOR_H


#include <array>
#include <list>
#include <string>


#include <core/component/Component.hpp>

#include <monitoring/datatypes/StatisticsTypes.hpp>
#include <monitoring/statistics_collector/StatisticsProviderDatatypes.hpp>

using namespace std;

namespace monitoring{

class StatisticsProviderPlugin;

enum StatisticsIntervall{
	SECOND,
	TEN_SECONDS,
	MINUTE
};

enum StatisticsReduceOperator{
	MIN,
	MAX,
	AVERAGE
};

class StatisticsCollector : public core::Component {
public:

	/*
 	 *	This virtual method is to register the metric, while calling it with its attribute and a special location such as "Storagedevice/SSD Blocklayer/sda"
 	 */
	virtual void registerPlugin(StatisticsProviderPlugin * plugin) = 0;

	virtual void unregisterPlugin(StatisticsProviderPlugin * plugin) = 0;

	/* 
	 * The return value may be updated in the background?
	 * Double buffering of values
	 * 
	 * We store statistics for the last:
	 * - 10 seconds in 1 second increments
	 * - 100 seconds in 10 second increments
	 * - 10 minutes in 1 minute increments
	 */
	virtual array<StatisticsValue,10> getStatistics(StatisticsIntervall intervall, StatisticsDescription & stat) = 0;

	virtual StatisticsValue getStatistics(StatisticsIntervall intervall, StatisticsDescription & stat, StatisticsReduceOperator op) = 0;

	// virtual StatisticsDescription & queryStatistics(StatisticsToQuery & stat) = 0;

	/*
 	 * What are the available source metrics and available sources for metrics if they are combined ones?
 	 */
	virtual list<StatisticsDescription> availableMetrics() = 0;
};

}


#define STATISTICS_INTERFACE "monitoring_statistics_collector"

#endif /* STATISTICS_COLLECTOR_H */
