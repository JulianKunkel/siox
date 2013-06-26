#ifndef MULTIPLEXERASYNC_H
#define MULTIPLEXERASYNC_H 

#include <monitoring/multiplexer/Multiplexer.hpp>
#include <monitoring/multiplexer/MultiplexerListener.hpp>

namespace monitoring{
/**
 * Special queue used by the Multiplexer to buffer activities
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
	 * @param	TYPE *	an activity that need to be dispatched in the future
	 */
	virtual void Push(TYPE * element) =0;
	
	/**
	 * Get an activity from queue, returned element is popped!
	 *
	 * @return	TYPE	an activity that needs to be dispatched to async listeners
	 */
	virtual TYPE * Pull() =0;
};


/**
 * ActivityMultiplexerNotifier
 * Used by the ActivityMultiplexer to dispatch to async listeners
 */
class MultiplexerNotifier
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
class MultiplexerAsync : public Multiplexer
{

};


}
#endif /* MULTIPLEXERASYNC_H */
