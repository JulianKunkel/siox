#ifndef SIOX_STATISTICS_COLLECTOR_MODULE
#define SIOX_STATISTICS_COLLECTOR_MODULE

#include <monitoring/statistics_collector/StatisticsCollector.hpp>

#define COMPONENT(x) \
extern "C"{\
void * get_instance_monitoring_statistics_collector() { return new x(); }\
}

#endif

