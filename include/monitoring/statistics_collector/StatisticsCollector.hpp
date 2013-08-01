/**
 * @file    StatisticsCollector.hpp
 *
 * @description A (software) component for collecting statistical values.
 * @standard    Preferred standard is C++11
 *
 * @author Marc Wiedemann, Julian Kunkel
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

Metrics such as Throughput rely on the smallest units bytes, microseconds (smallest unit when collecting /proc/lock_stat output as INT),
System Temperature in °C,  
The StatisticsInterval can be 1 second and significantly longer (10s,100s,60s,600s). 
Other values are computed if available at the response of the Statistics_Collecter and StatisticsCollectorThreaded

We have the deltas as a first approach defined only in the interval from t1 to t2 not in between. The slopes are the average slope of all slopes of the value's curve between t1 and t2.
SIZE "ds" = &a
TIME "dt" = &b

We compute mean values with the following:

Reduce SUM(x)/COUNT(x) = (eth0+eth1+eth2+eth3)/4 is responsibility of statistics_collector

MetricID comes from ontology

Topology instance? See lshw or similar.
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
	HUNDRED_MILLISECONDS,
	SECOND,
	TEN_SECONDS,
	MINUTE,
	TEN_MINUTES
};

enum StatisticsReduceOperator{
	MIN,
	MAX,
	COUNT,
	AVERAGE,
	SUM
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
