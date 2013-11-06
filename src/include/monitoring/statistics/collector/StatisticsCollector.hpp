/**
 * @file    StatisticsCollector.hpp
 *
 * A (software) component for collecting statistical values.
 *
 * @author Julian Kunkel, Marc Wiedemann, Nathanael Hübbe
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
The StatisticsInterval can be 0.1 second and significantly longer (1s,10s,60s,600s).
Other values are computed if available at the response of the Statistics_Collecter and StatisticsCollectorThreaded

We have the deltas as a first approach defined only in the interval from t1 to t2 not in between. The slopes are the average slope of all slopes of the value's curve between t1 and t2.
SIZE "ds" = &a
TIME "dt" = &b

We compute mean values with the following:

Reduce SUM(x)/COUNT(x) = (eth0+eth1+eth2+eth3)/4 is responsibility of statistics_collector

MetricID comes from ontology

Topology instance? See lshw or similar.

Use cases:
    U1  Capture node-global OS statistics and provide them to the statistics multiplexer/plugins.
    U2  An anomaly detection plugin queries node statistics to judge if observed performance degradation is caused by a saturated network.

Requirements:
    R1  Translate abstract descriptions of statistics into ontology.
    R2* Evaluate equations to derive new statistics from existing plugins => Done by a plugin, see D2.
    R3* Conserve min/max/average values for each statistics for this specific node, allow reasoner to query utilization of statistics. => Done by a plguin, see D1
    R4  Query statistics provider periodically.
    R5  Allow the user to specify the statistics to keep and the frequency at which plugins shall be queried.
    R6  Calculate average values for several periods consisting of 10 intervals, between 100ms and 60s.
    R7  Allow to reduce intervals by querying min, max, average, sum (these can be kept internally as statistics anyway).
    R8  Provide a rolling value for the last average value. This differs from R6 because the intervals in R6 are queried at fixed timestamps (every 10s for example), so when we ask for the last 10s at timestamp 19s we will get the value for the interval 0-10.
    R9  Allows statistics provider to (un)register -- these will be queried.
    R10 Provide a method to query ALL available statistics which are actually queried (by the user configuration in R5).

Rationales & design decisions/Issues and questions:
    D1  Should R3 be realized by a specific and generic statistics plugin instead of the collector?
        It seems to a good idea to outsource it to a plugin, because it reduces the complexity of any StatisticsCollector.
        Drawback: complex system interplay.
    D2  Should R2 be realized by a specific and generic statistics plugin which also registers as provider?
        It seems to be a good idea, because it reduces the complexity of any StatisticsCollector.
    D6  As we have a fixed time base on every ten seconds we deliver for example four periods of average values: 4x 10*100ms StatisticIntervals.
        Every minute we deliver (four) periods of average values:  4x 10*1s
        Every ten minutes we deliver : 4x 10*10s
        Se we need a vector of size number of periods with the data of average values.
    D10| To query ALL AVAILABLE STATS we keep a list of StatisticsDescription

Implementation deviations and rationales:
    1   Instead of preserving min/max/average values (R3) for all statistics, one of min/max/average/sum/(count?) is preserved.
        The selection of the reduction operator depends on the statistic.
        Rationale:
            Integrating memory consumption over time (the sum operation) is nonsense.
            Likewise, computing min/max/average values for the total data sent over a network connection is nonsense.
            The set of sensible operations depends on the nature of the statistic.
            TODO: Allow the statistics to use more than one aggregation operator.
    2   The current implementation allows the user to register/unregister StatisticsProviderPlugins,
        but it does not allow him to select which Statistics should actually be collected (R5).
        Rationale:
            The current StatisticsProviderPluginInterface does not provide this functionality;
            and providing a statistic is generally much more expensive than just collecting its value and updating a history.
            So, it's the StatisticsProviderPlugins that would have to provide this functionality.
            However, should they be able to do so one fine day,
            it should be relatively easy to provide a convenience functionality in the StatisticsCollector.
    3   The current implementation does not allow the user to change the polling frequency (R5).
        Rationale:
            The expensive factor in terms of performance is the generation of the statistics, not the history keeping;
            and afaik, the provider plugins are meant to bundle a multitude of different statistics,
            each of which is likely to ask for a different polling interval.
            As such, it is not only difficult to implement different polling intervals in the StatisticsCollector,
            it also appears to be not exactly useful.
            Taken together, it seemed prudent not to implement different polling frequencies at this point,
            rather relying on the provider plugins to reduce a fixed, high polling frequency as they see fit.
 */


#ifndef STATISTICS_COLLECTOR_H
#define STATISTICS_COLLECTOR_H


#include <array>
#include <list>
#include <string>
#include <memory>


#include <core/component/Component.hpp>

#include <monitoring/statistics/StatisticsTypes.hpp>
#include <monitoring/statistics/collector/StatisticsProviderDatatypes.hpp>
#include <monitoring/statistics/Statistic.hpp>

namespace monitoring {

	class StatisticsProviderPlugin;

	class StatisticsCollector : public core::Component {
		public:

			/*
			 *  This virtual method is to register the metric, while calling it with its attribute and a special location such as "Storagedevice/SSD Blocklayer/sda"
			 */
			virtual void registerPlugin( StatisticsProviderPlugin * plugin ) = 0;

			virtual void unregisterPlugin( StatisticsProviderPlugin * plugin ) = 0;

			/* D10
			 * What are the available source metrics and available sources for metrics if they are combined ones?
			 */
			virtual std::vector<std::shared_ptr<Statistic> > availableMetrics() throw()  = 0;

			//TODO: I think, we should replace the following three methods by one method to look up a statistic from a given StatisticsDescription.
			/*
			 * The return value may be updated in the background?
			 * Double buffering of values
			 *
			 * We store statistics for the last:
			 * - 10 seconds in 1 second increments
			 * - 100 seconds in 10 second increments
			 * - 10 minutes in 1 minute increments
			 */
			virtual array<StatisticsValue, Statistic::kHistorySize> getStatistics( StatisticsReduceOperator reductionOp, StatisticsInterval interval, const StatisticsDescription & description ) throw() = 0;

			/*
			 */
			virtual StatisticsValue getRollingStatistics( StatisticsReduceOperator reductionOp, StatisticsInterval interval, const StatisticsDescription & stat ) throw() = 0;

			virtual StatisticsValue getReducedStatistics( StatisticsReduceOperator reductionOp, StatisticsInterval interval, const StatisticsDescription & stat ) throw() = 0;

	};

}


#define STATISTICS_COLLECTOR_INTERFACE "monitoring_statistics_collector"

#endif /* STATISTICS_COLLECTOR_H */
