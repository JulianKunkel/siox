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

#include "../datatypes/Statistics.hpp"

#include <vector>
#include <string>
using namespace std;

namespace monitoring{

class StatisticsCollector {
public:

/*!
 This virtual method is to register the metric, while calling it with its attribute and a special location such as "Storagedevice/SSD Blocklayer/sda"
 */

	virtual void register_metrics(int i,MetricAttribute mattr,vector<string> list_of_specialties);

// Description  means local or remote metric
// Type = gauge if interval - We are converting incremental to gauge

	virtual void get_value(int i, vector<string> list_of_specialties);

/*!
 All the updated information of specific metrics of interest will be accessed when stepping to the next timestep;
 */
	virtual void next_timestep(int i,vector<string> list_of_specialties);

/*!
 startup the Collector;
 */
	virtual void initCollector();


/*!
 What are the available source metrics and available sources for metrics if they are combined ones?
 */

	virtual void available metrics();

};


#define STATISTICS_INTERFACE "monitoring_statistics_collector"

}

#endif /* STATISTICS_COLLECTOR_H */
