#include <monitoring/datatypes/Activity.hpp>
#include <monitoring/multiplexer/MultiplexerAsync.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerImplementation.hpp>
#include <template/monitoring/multiplexer/MultiplexerAsync.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerListener.hpp>

using namespace core;
using namespace monitoring;

namespace monitoring{


/**
 * ActivityMultiplexer
 * Forwards logged activities to registered listeners (e.g. Plugins) either
 * in an syncronised or asyncronous manner.
 */
//class ActivityMultiplexerAsyncThreaded : public MultiplexerAsync<Activity, ActivityMultiplexer>
class ActivityMultiplexerAsync : public MultiplexerAsyncTemplate<Activity>
{

};

}


CREATE_SERIALIZEABLE_CLS(MultiplexerAsyncOptions)

//PLUGIN(monitoring::ActivityMultiplexerAsyncThreaded)
