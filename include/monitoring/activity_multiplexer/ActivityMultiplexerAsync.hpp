#ifndef ACTIVITYMULTIPLEXERASYNC_H
#define ACTIVITYMULTIPLEXERASYNC_H 

#include <monitoring/datatypes/Activity.hpp>
#include <template/monitoring/multiplexer/MultiplexerAsync.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerListener.hpp>

namespace monitoring{


/**
 * ActivityMultiplexer
 * Forwards logged activities to registered listeners (e.g. Plugins) either
 * in an syncronised or asyncronous manner.
 */
class ActivityMultiplexerAsync : public MultiplexerAsyncTemplate<Activity>
{

};


}
#endif /* ACTIVITYMULTIPLEXERASYNC_H */
