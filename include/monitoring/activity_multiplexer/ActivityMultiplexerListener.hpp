#ifndef ACTIVITY_MULTIPLEXER_LISTENER_H
#define ACTIVITY_MULTIPLEXER_LISTENER_H

#include <monitoring/datatypes/Activity.hpp>

namespace monitoring{
/**
 * ActivityMultiplexerListener
 * Interface plugins that want to register to a multiplexer need to implement.
 */
// TODO: add second listener type, async/sync needs to have a signal for reset

class ActivityMultiplexerListener
{
public:
	/**
	 * Receive logged activity from a multiplexer
	 *
	 * @param	element	a logged Activity
	 */
	virtual void Notify(Activity * element) {};

	virtual ~ActivityMultiplexerListener(){}
};



class ActivityMultiplexerListenerSync : public ActivityMultiplexerListener
{
};

class ActivityMultiplexerListenerAsync : public ActivityMultiplexerListener
{
};


}

#endif /* ACTIVITY_MULTIPLEXER_LISTENER_H */
