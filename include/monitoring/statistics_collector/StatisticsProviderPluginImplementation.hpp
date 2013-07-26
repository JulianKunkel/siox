#ifndef SIOX_STATISTICS_MODULE
#define SIOX_STATISTICS_MODULE

#include <monitoring/statistics_collector/StatisticsProviderPlugin.hpp>


#define PLUGIN(x) \
extern "C"{\
void * get_instance_monitoring_statistics_plugin() { return new x(); }\
}



#endif

