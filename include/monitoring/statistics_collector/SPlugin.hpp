#ifndef SPLUGIN_H
#define SPLUGIN_H

// Splugin references statistics that StatisticsCollector registeres metrics for and gives over an object that holds metricname, daemon, unit, description and type.

#include <iostream>
#include <StatisticsCollector.hpp>

using namespace monitoring;

class ReferenceToStatisticsCollector {

	virtual void collect_fast(const MetricObject& x) { /* ... */ }

}

#endif
