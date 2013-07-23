#ifndef STATISTICS_PLUGIN_DEREFERENCING_FACADE_IMPL_H_
#define STATISTICS_PLUGIN_DEREFERENCING_FACADE_IMPL_H_


#include <monitoring/statistics_multiplexer/StatisticsPluginDereferencing.hpp>

#define PLUGIN(x) \
extern "C"{\
void * get_instance_monitoring_statistics_plugin_dereferencing() { return new x(); }\
}

#endif


