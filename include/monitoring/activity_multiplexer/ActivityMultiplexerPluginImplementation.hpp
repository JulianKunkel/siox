#ifndef SIOX_ACTIVITY_MULTIPLEXER_MODULE_PLUGIN_
#define SIOX_ACTIVITY_MULTIPLEXER_MODULE_PLUGIN_
#include <core/component/ComponentReferenceSerializable.hpp>

#include <monitoring/activity_multiplexer/ActivityMultiplexerPlugin.hpp>

#define PLUGIN(x) \
extern "C"{\
void * get_instance_monitoring_activitymultiplexer_plugin() { return new x(); }\
}

#endif
