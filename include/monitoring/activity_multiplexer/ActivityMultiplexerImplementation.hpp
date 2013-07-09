#ifndef SIOX_MMULTIPLEXER_MODULE
#define SIOX_MMULTIPLEXER_MODULE

#include <monitoring/activity_multiplexer/ActivityMultiplexer.hpp>

#define PLUGIN(x) \
extern "C"{\
void * get_instance_monitoring_activitymultiplexer() { return new x(); }\
}


#endif
