#include <monitoring/datatypes/Activity.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerImplementation.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerListener.hpp>

#include <templates/multiplexer/MultiplexerAsyncImpl.hpp>


using namespace core;
using namespace monitoring;

namespace monitoring{


/**
 * ActivityMultiplexer
 * Forwards logged activities to registered listeners (e.g. Plugins) either
 * in an syncronised or asyncronous manner.
 */
//class ActivityMultiplexerAsyncThreaded : public MultiplexerAsync<Activity, ActivityMultiplexer>
class ActivityMultiplexerAsyncThreaded : public MultiplexerAsyncTemplate<Activity, ActivityMultiplexer, ActivityMultiplexerListener>
{

};

}


CREATE_SERIALIZEABLE_CLS(MultiplexerAsyncOptions)

PLUGIN(ActivityMultiplexerAsyncThreaded)
