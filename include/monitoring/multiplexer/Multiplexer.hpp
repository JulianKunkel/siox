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

 3 Warnings
	Virtual functions and locks bear some risks for deadlocks or may corrupt
	data due to race conditions when implemented slopply.

	Depending on the implementation it may be very likely to create unexpected
	behaivor if only one of the three functions Log(), unregisterListener() or
	registerListener() is changed, as they may share a mutex.

 */

#ifndef MULTIPLEXER_H
#define MULTIPLEXER_H 

#include <monitoring/multiplexer/MultiplexerListener.hpp>
#include <core/component/Component.hpp>

using namespace core;

namespace monitoring{

/**
 * ActivityMultiplexer
 * Forwards logged activities to registered listeners (e.g. Plugins) either
 * in an syncronised or asyncronous manner.
 */
template <class TYPE, class PARENT>
class Multiplexer : public PARENT{
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

	/**
	 * prepare for shutdown of component 
	 */
	virtual void finalize() =0;


/*	
	// TODO propose possible option parameters
	void init(core::ComponentOptions * options) {}; 
	// TODO clarify if return an empty ComponentOptions has no side effects
	core::ComponentOptions * get_options() { return new core::ComponentOptions(); };
	void shutdown() {};	
*/

};


}
#endif /* MULTIPLEXER_H */
