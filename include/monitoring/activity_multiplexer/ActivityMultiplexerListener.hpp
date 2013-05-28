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

/**
 * Syncronous Multiplexer Listener
 *
 * This kind of Listeners is handled instantly by the Log() call to an multiplexer
 * by a layer. However most plugins should probably prefere a asyncronous subscription
 * as this less intrusive to the actual application.
 */
class ActivityMultiplexerListenerSync : public ActivityMultiplexerListener
{
	virtual void Notify(Activity * activity) =0;
};


/**
 * Asyncronous Multiplexer Listener
 *
 * Activities are passed only as soon as possible for the dispatching notfier thread.
 *
 */
class ActivityMultiplexerListenerAsync : public ActivityMultiplexerListener
{
	virtual void Notify(Activity * activity) =0;

	/**
	 * receive reset-signal and hint about severity
	 *
	 * @param	int dropped		number of dropped
	 */
	virtual void Reset(int dropped) =0;
};

#endif /* ACTIVITYMULTIPLEXERLISTENER_H */
