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
	virtual void Log(TYPE * element) {
		// readers and adding are alike (valid iterator), share ability to dec/inc
		{
			std::lock_guard<std::mutex> lock(inc);
			not_invalidating++;
		}

		for (auto listener=listeners.begin(); listener!=listeners.end(); ++listener) {
			(*listener)->Notify(element);
		}

		{
			std::lock_guard<std::mutex> lock(dec);
			not_invalidating--;
		}	
	}

	/**
	 * Register listener to multiplexer
	 *
	 * @param	ActivityMultiplexerListener *	listener	listener to notify in the future
	 */
	virtual void registerListener(MultiplexerListener<TYPE> * listener) {
		// readers and adding are alike (valid iterator), share ability to dec/inc
		{
			std::lock_guard<std::mutex> lock(inc);
			not_invalidating++;
		}

		listeners.push_back(listener);	
		
		{
			std::lock_guard<std::mutex> lock(dec);
			not_invalidating--;
		}
	}

	/**
	 * Unregister listener from multiplexer
	 *
	 * @param	ActivityMultiplexerListener *	listener	listener to remove
	 */
	virtual void unregisterListener(MultiplexerListener<TYPE> * listener) {
		// exclusive, as removing may invalidate iterator
		std::lock_guard<std::mutex> lock(inc);
		while( not_invalidating != 0 ) {
			// wait, candidate for condition vairable?
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
