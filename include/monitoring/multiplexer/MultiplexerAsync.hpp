/*!
 * @description
 * observer pattern, for easy notification of listeners with event buffer
 *
 * @author Jakob Luettgau, Julian Kunkel
 * @date   2013
 */

/*!
 Software Structure
 1 Requirements
	- same as for the normal Multiplexer, concurrent Log() calls while keeping
	  iterator intact when adding or removing listeners
	- buffer events that are passed to async listeners

 2 Use Cases
	- pass a message/element to multiple registered listeners
	
 3 Design and Text
	Removing a listener might invalidate iterator
	Inserting is usually fine (for lists uneffected, vectors until resize)

	Possible solutions:
		1.	snapshot list	
			-> memory + time panelty

		2.	change remove to null, check when iterating, 
			clean up in additional loop, might skip if time is precious
			-> management overhead (additional loops somewhere)

		3.	boost::shared_lock (was actually proposend and rejected for C++11)
			would implement readers/writers 
			-> greater dependency on BOOST

		4.	readers count + mutexes
			-> quite fair, but in current implemantion busy wait when registering

	C++11 does not provide a shared lock by it self (as needed for a RW-Scheme)
	reasoning: 
	http://permalink.gmane.org/gmane.comp.lib.boost.devel/211180

	//std::unique_lock<std::mutex> lock(mut);
	//std::lock_guard<std::mutex> lock(mut);

 */
#ifndef MULTIPLEXERASYNC_H
#define MULTIPLEXERASYNC_H 

#include <iostream>

#include <list>
#include <deque>

#include <mutex>
#include <condition_variable>
#include <atomic>
#include <thread>

#include <monitoring/multiplexer/Multiplexer.hpp>
#include <monitoring/multiplexer/MultiplexerListener.hpp>

using namespace std;

namespace monitoring{
/**
 * Special queue used by the Multiplexer to buffer activities
 * that are notified in an asyncrnous way.
 *
 */

// forward declaration for friendship 
template <class TYPE>class MultiplexerQueue;
template <class TYPE>class MultiplexerNotifier;
template <class TYPE>class MultiplexerAsync;


/**
 * A bounded buffer, to store elements that need to be dispatched by the 
 * Notifier
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
	virtual TYPE * Pop() =0; 
};


/**
 * ActivityMultiplexerNotifier
 * Used by the ActivityMultiplexer to dispatch to async listeners
 */
template <class TYPE>
class MultiplexerNotifier
{
	MultiplexerAsync<TYPE> * multiplexer;


public:
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
template <class TYPE>
class MultiplexerAsync : Multiplexer<TYPE>
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
	 * @param	MultiplexerListener *	listener	listener to notify in the future
	 */
	virtual void registerListener(MultiplexerListener<TYPE> * listener) =0; 

	/**
	 * Unregister listener from multiplexer
	 *
	 * @param	MultiplexerListener *	listener	listener to remove
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
