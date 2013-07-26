#ifndef SPLUGIN_H
#define SPLUGIN_H

/*! Splugin references statistics that StatisticsCollector registeres metrics for and gives over an object that holds metricname, daemon, unit, description and type.
    =SProvider:
        AvailableMetric: Entity (Enum)          SystemScope (Enum)      Semantics of the Metric         Abstract Context Topology (Array as strings)   Instance(Values as strings)      PointerToValue !!!



How do we map this information into Ontology Attributes?
We map Systemscope to Domain, Semantics of the Metric to the name, additional attributes are mapped as follows:
 */

/*
*********TODO*********
        Entity, Abstract Context Topology, & Instance

How can someone ask for a given Attribute, e.g. in a statistics plugin or in a amux plugin?
The easy answer: Specify the 5-Tuple:
Entity, SystemScope, Semantics of the Metric, Abstract Context Topology, & Instance

*********TODO*********
*/


#include <iostream>
#include <StatisticsCollector.hpp>

using namespace monitoring;

class ReferenceToStatisticsCollector {

	virtual void collect_fast(const MetricObject& x) { /* ... */ }

/**
 mit init in available metrics
*/

	virtual available_metrics;



}

#endif
