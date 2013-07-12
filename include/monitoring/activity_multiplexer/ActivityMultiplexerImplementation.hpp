#ifndef SIOX_ACTIVITY_MULTIPLEXER_MODULE
#define SIOX_ACTIVITY_MULTIPLEXER_MODULE

#include <monitoring/activity_multiplexer/ActivityMultiplexer.hpp>

#define PLUGIN(x) \
extern "C"{\
void * get_instance_monitoring_activitymultiplexer() { return new x(); }\
}

#endif
