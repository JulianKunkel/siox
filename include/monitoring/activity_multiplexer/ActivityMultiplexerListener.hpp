#ifndef ACTIVITYMULTIPLEXERLISTENER_H
#define ACTIVITYMULTIPLEXERLISTENER_H 

#include <monitoring/datatypes/Activity.hpp>
#include <monitoring/multiplexer/MultiplexerListener.hpp>

namespace monitoring{
/**
 * ActivityMultiplexerListener
 * Interface plugins that want to register to a multiplexer need to implement. 
 */
// TODO: add second listener type, async/sync    async need to have a signal
typedef MultiplexerListener<Activity> ActivityMultiplexerListener;


class ActivityMultiplexerListenerSync : public ActivityMultiplexerListener
{
};

class ActivityMultiplexerListenerAsync : public ActivityMultiplexerListener
{
};


}

#endif /* ACTIVITYMULTIPLEXERLISTENER_H */
