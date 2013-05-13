#ifndef ACTIVITYMULTIPLEXERLISTENER_H
#define ACTIVITYMULTIPLEXERLISTENER_H 

//TODO: change to non dummy activity
#include "test/Activity.hpp"

/**
 * ActivityMultiplexerListener
 * Interface plugins that want to register to a multiplexer need to implement. 
 */
class ActivityMultiplexerListener
{
public:
	/**
	 * Receive logged activity from a multiplexer
	 *
	 * @param	Activity	a logged Activity
	 * @return	void
	 */
	virtual void Notify(Activity * activity) =0;
};

#endif /* ACTIVITYMULTIPLEXERLISTENER_H */
