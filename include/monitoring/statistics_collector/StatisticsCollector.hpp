#ifndef STATISTICS_COLLECTOR_H
#define STATISTICS_COLLECTOR_H

namespace monitoring{

class StatisticsCollector {
public:

	virtual void register_metrics(String * metricname);

// Description  means local or remote metric
// Type = gauge if interval - We are converting incremental to gauge

	virtual void get_metrics(String * metricname, daemonid, SI_Unit * si_unit, description, type);

};


#define STATISTICS_INTERFACE "monitoring_statistics_collector"

}

#endif /* STATISTICS_COLLECTOR_H */
