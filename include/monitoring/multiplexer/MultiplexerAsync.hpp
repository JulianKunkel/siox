#ifndef MULTIPLEXERASYNC_H
#define MULTIPLEXERASYNC_H 

#include <deque>

#include <mutex>
#include <condition_variable>
#include <thread>

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
	// for syncronisation
	std::mutex mut;
	std::condition_variable not_full;
	std::condition_variable not_empty;
	bool v = false;

public:
	/**
	 * Check whether or not the queue has still capacity
	 *
	 * @return	bool	true = queue is full, false = not full
	 */
	virtual bool Full() {
		return false;
	};
	

	/* Check whether of not the queue is empty
	 * 
	 * @return bool		true = queue is empty, false = not empty
	 */
	virtual bool Empty() {
		return false;
	};


	/**
	 * Check if queue is in overload mode
	 *
	 */
	virtual bool Overloaded() {
		return false;
	};


	/**
	 * Add an activity to the queue
	 *
	 * @param	TYPE *	an activity that need to be dispatched in the future
	 */
	virtual void Push(TYPE * element) {};
	
	/**
	 * Get an activity from queue, returned element is popped!
	 *
	 * @return	TYPE	an activity that needs to be dispatched to async listeners
	 */
	virtual TYPE * Pull() { return new TYPE; };
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
template <class TYPE>
class MultiplexerAsync : protected Multiplexer<TYPE>
{

	
};


}
#endif /* MULTIPLEXERASYNC_H */
