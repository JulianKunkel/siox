#ifndef SIOX_MMULTIPLEXERASYNC_MODULE
#define SIOX_MMULTIPLEXERASYNC_MODULE

#include <monitoring/activity_multiplexer/ActivityMultiplexerAsync.hpp>

#define PLUGIN(x) \
extern "C"{\
void * get_instance_monitoring_activitymultiplexerasync() { return new x(); }\
}

#endif
