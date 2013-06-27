/*!
 * @description
 * observer pattern, for easy notification of listeners
 *
 * @author Jakob Luettgau, Julian Kunkel
 * @date   2013
 */

/*!
 Software Structure
 1 Requirements
	- multiple Log() calls shall run in parallel
	- adding and removal of listener without corrupting iterator

 2 Use Cases
	- pass a message/element to multiple registered listeners
	
 3 Design and Text
	Removing a listener might invalidate iterator
	Inserting is usually fine (for lists uneffected, vectors until resize)

	Possible solutions:
		1.	snapshot list	-> memory + time panelty

		2.	change remove to null, check when iterating, 
			clean up in additional loop, might skip if time is precious

		3.	boost::shared_lock (was actually proposend and rejected for C++11)
			would implement readers/writers 

		4. readers count + mutex

	C++11 does not provide a shared lock by it self, reasoning:
	http://permalink.gmane.org/gmane.comp.lib.boost.devel/211180


	//std::unique_lock<std::mutex> lock(mut);
	//std::lock_guard<std::mutex> lock(mut);

 */

#ifndef MULTIPLEXER_H
#define MULTIPLEXER_H 

#include <list>
#include <mutex>

#include <monitoring/multiplexer/MultiplexerListener.hpp>
#include <core/component/Component.hpp>

namespace monitoring{

/**
 * ActivityMultiplexer
 * Forwards logged activities to registered listeners (e.g. Plugins) either
 * in an syncronised or asyncronous manner.
 */
template <class TYPE>
class Multiplexer : core::Component
{

	list<MultiplexerListener<TYPE> *> listeners;

	// thread safety, kept namespace verbose for clarity
	std::mutex inc;
	std::mutex dec;
	int not_invalidating = 0;


public:
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

	/**
	 * prepare for shutdown of component 
	 */
	virtual void finalize() {};


	
	// TODO propose possible option parameters
	void init(core::ComponentOptions * options) {}; 
	// TODO clarify if return an empty ComponentOptions has no side effects
	core::ComponentOptions * get_options() { return new core::ComponentOptions(); };
	void shutdown() {};
	
};


}
#endif /* MULTIPLEXER_H */
