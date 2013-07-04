#ifndef ACTIVITYMULTIPLEXER_H
#define ACTIVITYMULTIPLEXER_H 

#include <monitoring/datatypes/Activity.hpp>
//#include <monitoring/multiplexer/Multiplexer.hpp>
#include <template/monitoring/multiplexer/Multiplexer.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerListener.hpp>

namespace monitoring{


/**
 * ActivityMultiplexer
 * Forwards logged activities to registered listeners (e.g. Plugins) either
 * in an syncronised or asyncronous manner.
 */
class ActivityMultiplexer : public MultiplexerTemplate<Activity>
{

};


}
#endif /* ACTIVITYMULTIPLEXER_H */
