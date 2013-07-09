#ifndef ACTIVITY_PLUGIN_DEREFERENCING_FACADE_IMPL_H_
#define ACTIVITY_PLUGIN_DEREFERENCING_FACADE_IMPL_H_


#include <monitoring/activity_multiplexer/ActivityPluginDereferencing.hpp>

#define PLUGIN(x) \
extern "C"{\
void * get_instance_monitoring_activity_plugin_dereferencing() { return new x(); }\
}

#endif

#define ACTIVITY_DEREFERENCING_FACADE_INTERFACE "monitoring_activity_plugin_dereferencing"

