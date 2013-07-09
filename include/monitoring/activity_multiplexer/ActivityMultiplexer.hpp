#ifndef ACTIVITYMULTIPLEXER_H
#define ACTIVITYMULTIPLEXER_H 

#include <monitoring/datatypes/Activity.hpp>
#include <monitoring/multiplexer/Multiplexer.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerListener.hpp>

namespace monitoring{


/**
 * ActivityMultiplexer
 * Forwards logged activities to registered listeners (e.g. Plugins) either
 * in an syncronised or asyncronous manner.
 */
class ActivityMultiplexer : public Multiplexer<Activity>
{

};


#define ACTIVITY_MULTIPLEXER_INTERFACE "monitoring_activitymultiplexer"

#define AMUX(x) \
extern "C"{\
void * get_instance_monitoring_activitymultiplexer() { return new x(); }\
}

}
#endif /* ACTIVITYMULTIPLEXER_H */
