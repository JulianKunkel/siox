#ifndef SIOX_STATISTICS_MODULE
#define SIOX_STATISTICS_MODULE

#include <monitoring/statistics/Statistics.hpp>

#define PLUGIN(x) \
extern "C"{\
void * get_instance_monitoring_statistics() { return new x(); }\
}

#endif

