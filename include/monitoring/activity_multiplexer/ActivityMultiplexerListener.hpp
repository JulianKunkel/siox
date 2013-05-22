#ifndef ACTIVITYMULTIPLEXERLISTENER_H
#define ACTIVITYMULTIPLEXERLISTENER_H 

//TODO: change to non dummy activity
#include "../../../monitoring/activity_multiplexer/test/Activity.hpp"

/**
 * ActivityMultiplexerListener
 * Interface plugins that want to register to a multiplexer need to implement. 
 */
// TODO: add second listener type, async/sync    async need to have a signal
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



class ActivityMultiplexerListenerAsync : public ActivityMultiplexerListener
{
	virtual void Notify(Activity * activity) =0;
};


class ActivityMultiplexerListenerSync : public ActivityMultiplexerListener
{
	virtual void Notify(Activity * activity) =0;
};

#endif /* ACTIVITYMULTIPLEXERLISTENER_H */
