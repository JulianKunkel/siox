/** @file
 * observer pattern, for easy notification of listeners with event buffer and
 * async dispatch to listeners (while also preserving the sync path to sync
 * listeners)
 *
 * @author Jakob Luettgau, Julian Kunkel
 * @date   2013
 */

/**
 Software Structure
 1 Requirements
	- same as for the normal Multiplexer, concurrent Log() calls while keeping
	  iterator intact when adding or removing listeners
	- buffer events that are passed to async listeners

 2 Use Cases
	- pass a message/element to multiple registered listeners
	- when order of alements hold no meaning, the async path allows for
	  a lower impact of siox on the application
 */
#ifndef MULTIPLEXERASYNC_H
#define MULTIPLEXERASYNC_H 

#include <monitoring/multiplexer/Multiplexer.hpp>
#include <monitoring/multiplexer/MultiplexerAsyncOptions.hpp>
	
#include <monitoring/multiplexer/MultiplexerListener.hpp>
#include <core/component/Component.hpp>

using namespace std;

namespace monitoring{
/**
 * Special queue used by the Multiplexer to buffer activities
 * that are notified in an asyncrnous way.
 *
 */

// forward declaration for friendship 
template <class TYPE>class MultiplexerQueue;
template <class TYPE, class PARENT>class MultiplexerNotifier;
//template <class TYPE, class PARENT>class MultiplexerAsync;
template <class TYPE, class PARENT>class MultiplexerAsync;


/**
 * A bounded buffer, to store elements that need to be dispatched by the 
 * Notifier
 */
template <class TYPE>
class MultiplexerQueue 
{
public:
	virtual ~MultiplexerQueue(){}

	/**
	 * Check whether or not the queue has still capacity
	 *
	 * @return		true = queue is full, false = not full
	 */
	virtual bool Full() =0; 
	

	/* Check whether of not the queue is empty
	 * 
	 * @return 		true = queue is empty, false = not empty
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
	 * @param	element 	an activity that need to be dispatched in the future
	 */
	virtual void Push(TYPE * element) =0; 
	
	/**
	 * Get an activity from queue, returned element is popped!
	 *
	 * @return		an activity that needs to be dispatched to async listeners
	 */
	virtual TYPE * Pop() =0; 
};


/**
 * ActivityMultiplexerNotifier
 * Used by the ActivityMultiplexer to dispatch to async listeners
 */
template <class TYPE, class PARENT>
class MultiplexerNotifier
{
	MultiplexerAsync<TYPE, PARENT> * multiplexer;
public:
	virtual ~MultiplexerNotifier(){}

	// TODO: signal upstream to Deamons others
	virtual void Reset(int lost) =0;

	/**
	 * cleanup data structures and finish immediately
	 */
	virtual void shutdown() =0;
	
	/**
	 * set terminate flag for Notifier, terminates as soon queue is emptied 
	 */
	virtual	void finalize() =0;
};


/**
 * ActivityMultiplexer
 * Forwards logged activities to registered listeners (e.g. Plugins) either
 * in an syncronised or asyncronous manner.
 */
//template <class TYPE, class PARENT>
//class MultiplexerAsync  : public PARENT

template <class TYPE, class PARENT>
class MultiplexerAsync : Multiplexer<TYPE, PARENT>
{
public:

	/**
	 * Called by layer to report about activity, passes activities to sync listeners
	 * and enqueqes activity for async dispatch.
	 */
	// TODO sadly for the mutexes it is always needed
	virtual void Log(TYPE * element) =0;

	/**
	 * Register listener to multiplexer
	 *
	 * @param	listener	listener to notify in the future
	 */
	virtual void registerListener(MultiplexerListener<TYPE> * listener) =0; 

	/**
	 * Unregister listener from multiplexer
	 *
	 * @param	listener	listener to remove
	 */
	virtual void unregisterListener(MultiplexerListener<TYPE> * listener) =0;



	virtual void finalize() =0;
	
	//
	// satisfy abstract parents
	//
	/*
	// TODO propose possible option parameters
	void init(core::ComponentOptions * options) {}; 
	// TODO clarify if return an empty ComponentOptions has no side effects
	core::ComponentOptions * get_options() { return new core::ComponentOptions(); };
	void shutdown() {};	
	*/

};


}
#endif /* MULTIPLEXERASYNC_H */
