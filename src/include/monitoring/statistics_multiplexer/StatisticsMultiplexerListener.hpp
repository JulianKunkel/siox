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

#include <monitoring/datatypes/StatisticsTypes.hpp>
#include <monitoring/datatypes/ids.hpp>
#include <monitoring/topology/Topology.hpp>
#include <monitoring/statistics_collector/Statistic.hpp>

#include <memory>

namespace monitoring {

	//TODO: remove the dependency on the Ontology from the listener, requiring him to just supply an ontology attribute name and domain, and a topology path.
	class StatisticsMultiplexerListener {
		public:
			//virtual const std::vector<std::pair<OntologyAttributeID, Topology::ObjectId> >& requiredMetrics() throw() = 0;
			virtual const std::vector<std::pair<OntologyAttributeID, std::vector< std::pair< std::string, std::string> > > >& requiredMetrics() throw() = 0;
			virtual void notify(const std::vector<std::shared_ptr<Statistic> >& statistics) throw() = 0;	//The supplied vector is synchronous to the vector that was returned by requiredMetrics(). Pointers may be NULL when the corresponding statistic is not provided by the statistics collector.
	};


}

#endif
