/**
 * @file    StatisticsCollector.hpp
 *
 * A (software) component for collecting statistical values.
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

Use cases:
	U1	Capture node-global OS statistics and provide them to the statistics multiplexer/plugins.
	U2	An anomaly detection plugin queries node statistics to judge if observed performance degradation is caused by a saturated network.

Requirements:
	R1 	Translate abstract descriptions of statistics into ontology.
	R2*	Evaluate equations to derive new statistics from existing plugins => Done by a plugin, see D2.
	R3*	Conserve min/max/average values for each statistics for this specific node, allow reasoner to query utilization of statistics. => Done by a plguin, see D1
	R4	Query statistics provider periodically.
	R5	Allow the user to specify the statistics to keep and the frequency at which plugins shall be queried.
	R6	Calculate average values for several periods consisting of 10 intervals, between 100ms and 60s.
	R7	Allow to reduce intervals by querying min, max, average, sum (these can be kept internally as statistics anyway).
	R8	Provide a rolling value for the last average value. This differs from R6 because the intervals in R6 are queried at fixed timestamps (every 10s for example), so when we ask for the last 10s at timestamp 19s we will get the value for the interval 0-10.
	R9 	Allows statistics provider to (un)register -- these will be queried.
	R10	Provide a method to query ALL available statistics which are actually queried (by the user configuration in R5).

Rationales & design decisions/Issues and questions:
	D1	Should R3 be realized by a specific and generic statistics plugin instead of the collector?
		It seems to a good idea to outsource it to a plugin, because it reduces the complexity of any StatisticsCollector.
		Drawback: complex system interplay.
	D2	Should R2 be realized by a specific and generic statistics plugin which also registers as provider?
		It seems to be a good idea, because it reduces the complexity of any StatisticsCollector.
	D6	As we have a fixed time base on every ten seconds we deliver for example four periods of average values: 4x 10*100ms StatisticIntervals.
		Every minute we deliver (four) periods of average values:  4x 10*1s
		Every ten minutes we deliver : 4x 10*10s
		Se we need a vector of size number of periods with the data of average values.
	D10| To query ALL AVAILABLE STATS we keep a list of StatisticsDescription
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

	/*
	 */
	virtual StatisticsValue getRollingStatistics(StatisticsIntervall intervall, StatisticsDescription & stat) = 0;

	virtual StatisticsValue getReducedStatistics(StatisticsIntervall intervall, StatisticsDescription & stat, StatisticsReduceOperator op) = 0;

	/* D10
 	 * What are the available source metrics and available sources for metrics if they are combined ones?
 	 */
	virtual list<StatisticsDescription> availableMetrics() = 0;
};

}


#define STATISTICS_INTERFACE "monitoring_statistics_collector"

#endif /* STATISTICS_COLLECTOR_H */
