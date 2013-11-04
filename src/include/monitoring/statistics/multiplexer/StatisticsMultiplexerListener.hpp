/*
A StatisticsMultiplexerListener is an object that is registered with StatisticsMultiplexer to be notified when new statistic values become available.

A StatisticsMultiplexerListener requests a list of statistics from the StatisticsMultiplexer by setting up a vector ontology attribute / topology object pairs, which is made available to the StatisticsMultiplexer via the requiredMetrics() method.
This vector must not change after requiredMetrics() has returned.
The StatisticsMultiplexer will then setup a synchronous vector of pointers to the requested statistics, which is subsequently passed back to the StatisticsMultiplexerListener via the notify() method.
Since there may not be a StatisticsProvider registered that can provide a requested statistic, the statistics vector passed to notify() may contain NULL pointers.
These NULL pointers may subsequently be replaced by functional pointers when a corresponding StatisticsProvider is registered with the StatisticsCollector.
*/

#ifndef INCLUDE_GUARD_STATISTICS_MULTIPLEXER_LISTENER_H
#define INCLUDE_GUARD_STATISTICS_MULTIPLEXER_LISTENER_H

#include <monitoring/statistics/StatisticsTypes.hpp>
#include <monitoring/datatypes/ids.hpp>
#include <monitoring/topology/Topology.hpp>
#include <monitoring/statistics/collector/Statistic.hpp>

#include <memory>

namespace monitoring {

	class StatisticsMultiplexerListener {
		public:
			virtual void notifyAvailableStatisticsChange( const std::vector<std::shared_ptr<Statistic> > & statistics, bool addedStatistics, bool removedStatistics ) throw() = 0;
			virtual void newDataAvailable() throw() = 0;	//is never called before notifyAvailableStatisticsChange() is called at least once
	};


}

#endif
