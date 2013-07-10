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



	Lock relations:
	
	unregister
		sync		blocks sync dispatch, blocks register of sync
		async		blocks async dispatch, blocks register of async

	register
		sync		blocks unregister and register of sync
		async		blocks unregister and register of async

		does not block any dispatch, as for std::list iterator is not invalidated

	dispatch
		sync		blocks unregister
		async		blocks unregister
	
	queue
		pop	<-------> push
			  block


	//std::unique_lock<std::mutex> lock(mut);
	//std::lock_guard<std::mutex> lock(mut);

 */
#ifndef MULTIPLEXERASYNC_TEMPLATE_H
#define MULTIPLEXERASYNC_TEMPLATE_H 

#include <iostream>

#include <list>
#include <deque>

#include <mutex>
#include <condition_variable>
#include <atomic>
#include <thread>

#include <monitoring/multiplexer/MultiplexerAsync.hpp>
#include <monitoring/multiplexer/MultiplexerListener.hpp>

using namespace std;

namespace monitoring{
/**
 * Special queue used by the Multiplexer to buffer activities
 * that are notified in an asyncrnous way.
 *
 */

// forward declaration for friendship 
template <class TYPE>class MultiplexerQueueTemplate;
template <class TYPE>class MultiplexerNotifierTemplate;
template <class TYPE>class MultiplexerAsyncTemplate;

/**
 * A bounded buffer, to store elements that need to be dispatched by the 
 * Notifier
 */
template <class TYPE>
class MultiplexerQueueTemplate : public MultiplexerQueue<TYPE>
{
	//MultiplexerAsync<TYPE> * multiplexer;
	
	deque<TYPE *> queue; 
	int capacity = 1000;

	bool overloaded = false;
	int lost = 0;

	// for syncronisation
	std::mutex mut;
	std::condition_variable not_full;
	std::condition_variable not_empty;
	bool v = false; // prevents starvation

public:
	/**
	 * Check whether or not the queue has still capacity
	 *
	 * @return	bool	true = queue is full, false = not full
	 */
	virtual bool Full() {
		bool result = ( queue.size() > capacity );
		return result;
	};
	

	/* Check whether of not the queue is empty
	 * 
	 * @return bool		true = queue is empty, false = not empty
	 */
	virtual bool Empty() {
		bool result = ( queue.size() == 0 );
		return result;
	};


	/**
	 * Check if queue is in overload mode
	 *
	 */
	virtual bool Overloaded() {
		return overloaded;
	};

	/**
	 * Add an activity to the queue if there is capacity, set overload flag
	 * otherwhise.
	 *
	 * @param	TYPE *	an activity that need to be dispatched in the future
	 */
	virtual void Push(TYPE * element) {
		std::lock_guard<std::mutex> lock(mut);

		// maybe this should happen in notifier run()
		/*
		if (Overloaded() && Empty()) {	
			// TODO notifier.Reset(lost);
			lost = 0;
			overloaded = false;
		}
		*/

		if (Overloaded()) {
			lost++;
		} else {

			if (Full()) {
				overloaded = true;
				lost = 1;
			} else {
				queue.push_back(element);
			}
		}
	};
	
	/**
	 * Get an activity from queue, returned element is popped!
	 *
	 * @return	TYPE	an activity that needs to be dispatched to async listeners
	 */
	virtual TYPE * Pop() {
		// TODO actually it should be absolutely ok to have the condition in 
		//		here, as the thread should still be put to sleep 
		std::lock_guard<std::mutex> lock(mut);

		TYPE * element;
		
		element = queue.front();
		queue.pop_front();

		return element;
	};
};

/**
 * ActivityMultiplexerNotifier
 * Used by the ActivityMultiplexer to dispatch to async listeners
 */
template <class TYPE>
class MultiplexerNotifierTemplate : public MultiplexerNotifier<TYPE>
{
	bool terminate = false;

	MultiplexerAsync<TYPE> * multiplexer;

	
	int dispatcher = 1;
	list<std::thread> t;

public:
	MultiplexerNotifierTemplate() {
	//MultiplexerNotifierTemplate(MultiplexerAsync<TYPE> * multiplexer) {
		//this->multiplexer = multiplexer;
		
		for (int i = 0; i < dispatcher; ++i) {
			t.push_back(std::thread(&MultiplexerNotifierTemplate::Run, this));
			
		}
	}

	/**
	 * dispatch protocol with overload handling 
	 */
	virtual void Run() {

		// if queue.empty() && terminate
			// shutdown
		// else
			// if queue.overloaded() && queue.empty()
				// Reset();
			// else
				// Dispatch();
		while(true){
			cout << "Run();\n";
		}
	}

	/**
	 * dispatch an Activity to async listeners
	 */
	virtual void Dispatch() {
		
	}


	// TODO: signal upstream to Deamons others
	virtual void Reset(int lost) {}

	/**
	 * free ressources and finish immediately
	 */
	virtual void shutdown() {
		// wait for threads to finish
		for(auto it = t.begin(); it != t.end(); ++it ) {
			(*it).join();
		}	
	};
	
	/**
	 * set terminate flag for Notifier, terminates as soon queue is emptied 
	 */
	virtual	void finalize() {
		terminate = true;
	};
};

/**
 * ActivityMultiplexer
 * Forwards logged activities to registered listeners (e.g. Plugins) either
 * in an syncronised or asyncronous manner.
 */
template <class TYPE>
class MultiplexerAsyncTemplate : MultiplexerAsync<TYPE>
{
	list<MultiplexerListener<TYPE> *> listeners;
	list<MultiplexerListener<TYPE> *> listeners_sync;
	list<MultiplexerListener<TYPE> *> listeners_async;

	// thread safety, kept namespace verbose for clarity
	std::mutex inc;
	std::mutex dec;
	int not_invalidating = 0;

	MultiplexerQueue<TYPE> * queue;
	MultiplexerNotifier<TYPE> * notifier;

public:

	MultiplexerAsyncTemplate () {
		queue = new MultiplexerQueueTemplate<TYPE>;	
		notifier = new MultiplexerNotifierTemplate<TYPE>;		
	}

	
	~MultiplexerAsyncTemplate() {
		// TODO remove(queue);
		// TODO remove(notifier);
	}
	
	

	/**
	 * Called by layer to report about activity, passes activities to sync listeners
	 * and enqueqes activity for async dispatch.
	 */
	// TODO sadly for the mutexes it is always needed
	virtual void Log(TYPE * element) {
		{
			std::lock_guard<std::mutex> lock(inc);
			not_invalidating++;
		}

		for (auto it=listeners.begin(); it!=listeners.end(); ++it) {
			(*it)->Notify(element);
		}

		{
			std::lock_guard<std::mutex> lock(dec);
			not_invalidating--;
		}	
		
		// TODO cond.notify_one();
	}

	/**
	 * Register listener to multiplexer
	 *
	 * @param	MultiplexerListener *	listener	listener to notify in the future
	 */
	virtual void registerListener(MultiplexerListener<TYPE> * listener) {
		// exclusive, adding multiple listerns might result in race condition
		std::lock_guard<std::mutex> lock(inc);
		while( not_invalidating != 0 ) {
			// wait
			// TODO candidate for condition vairable?
		}
		listeners.push_back(listener);
	}

	/**
	 * Unregister listener from multiplexer
	 *
	 * @param	MultiplexerListener *	listener	listener to remove
	 */
	virtual void unregisterListener(MultiplexerListener<TYPE> * listener) {
		// exclusive, as removing may invalidate iterator
		std::lock_guard<std::mutex> lock(inc);
		while( not_invalidating != 0 ) {
			// wait
			// TODO candidate for condition vairable?
		}
		listeners.remove(listener);
	}

	//
	// satisfy abstract parents
	//
	virtual void finalize() {};

	// TODO propose possible option parameters
	void init(core::ComponentOptions * options) {}; 
	// TODO clarify if return an empty ComponentOptions has no side effects
	core::ComponentOptions * get_options() { return new core::ComponentOptions(); };
	void shutdown() {};	

};


}
#endif /* MULTIPLEXERASYNC_TEMPLATE_H */
