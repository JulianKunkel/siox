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
 Die Werte für die Metrikattribute werden für den nächsten Timestep angezeigt.
 */
    virtual void next_timestep(int i,vector<string> list_of_specialties);


};


#define STATISTICS_INTERFACE "monitoring_statistics_collector"

}

#endif /* STATISTICS_COLLECTOR_H */
