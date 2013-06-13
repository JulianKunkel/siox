#ifndef ACTIVITYMULTIPLEXER_H
#define ACTIVITYMULTIPLEXER_H 

#include <monitoring/multiplexer/MultiplexerListener.hpp>

namespace monitoring{
/**
 * Special queue used by the ActivityMultiplexer to buffer activities
 * that are notified in an asyncrnous way.
 *
 */

template <class TYPE>
class MultiplexerQueue 
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
	 * Check if queue is in overload mode
	 *
	 */
	virtual bool Overloaded() =0;


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
	// TODO: signal upstream to Deamons others
	
	/**
	 * cleanup data structures and finish immediately
	 */
	virtual void shutdown() = 0;
	
	/**
	 * set terminate flag for Notifier, terminates as soon queue is emptied 
	 */
	virtual	void finalize() = 0;
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
	 */
	virtual void registerListener(ActivityMultiplexerListener * listener) =0;

	/**
	 * Unregister listener from multiplexer
	 *
	 * @param	ActivityMultiplexerListener *	listener	listener to remove
	 */
	virtual void unregisterListener(ActivityMultiplexerListener * listener) =0;


	/**
	 * pass termination to notifiers 
	 */
	virtual void terminate() =0;

};


}
#endif /* ACTIVITYMULTIPLEXER_H */
