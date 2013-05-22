#ifndef ACTIVITYMULTIPLEXER_H
#define ACTIVITYMULTIPLEXER_H 

//TODO: replace by non dummy activity
#include "../../../monitoring/activity_multiplexer/test/Activity.hpp"
#include "ActivityMultiplexerListener.hpp"

/**
 * Special queue used by the ActivityMultiplexer to buffer activities
 * that are notified in an asyncrnous way.
 *
 */

// could implement async buffer that is used by various components
//
class ActivityMultiplexerQueue
{
public:
	/**
	 * Check whether or not the queue has still capacity
	 *
	 * @return	bool	true = queue is full, false = not full
	 */
	virtual bool Full() =0;
	

	/* Check whether of not the queue is empty
	 * 
	 * @return bool		true = queue is empty, false = not empty
	 */
	virtual bool Empty() =0;


	/**
	 * Add an activity to the queue
	 *
	 * @param	Activity *	an activity that need to be dispatched in the future
	 */
	virtual void Push(Activity * activity) =0;
	
	/**
	 * Get an activity from queue, returned element is popped!
	 *
	 * @return	Activity	an activity that needs to be dispatched to async listeners
	 */
	virtual Activity * Pull() =0;
};


/**
 * ActivityMultiplexerNotifier
 * Used by the ActivityMultiplexer to dispatch to async listeners
 */
class ActivityMultiplexerNotifier
{
public:
//	virtual void setQueue(ActivityMultiplexerQueue * queue) =0;
//	virtual void setListenerList(std::list<ActivityMultiplexerListener*> * list) =0;

	// signal upstream to Deamons others

	/**
	 * Called by the owning multiplexer to start dispatching activities to async
	 * listeners in case Notifier was not wokring already.
	 */
	// queue und notifier wake each other
	virtual void Wake() =0;
};


/**
 * ActivityMultiplexer
 * Forwards logged activities to registered listeners (e.g. Plugins) either
 * in an syncronised or asyncronous manner.
 */
class ActivityMultiplexer
{
public:
	/**
	 * Called by layer to report about activity, passes activities to sync listeners
	 * and enqueqes activity for async dispatch.
	 */
	virtual void Log(Activity * activity) =0;

	/**
	 * Register listener to multiplexer
	 *
	 * @param	ActivityMultiplexerListener *	listener	listener to notify in the future
	 * @param	bool							asyc		async = true, sync = false
	 */
	// TODO: typecast check for async/sync
	virtual void registerListener(ActivityMultiplexerListener * listener) =0;

	/**
	 * Unregister listener from multiplexer
	 *
	 * @param	ActivityMultiplexerListener *	listener	listener to remove
	 * @param	bool							asyc		async = true, sync = false
	 */
	// TODO: typecast check for async/sync
	virtual void unregisterListener(ActivityMultiplexerListener * listener) =0;
};

#endif /* ACTIVITYMULTIPLEXER_H */
