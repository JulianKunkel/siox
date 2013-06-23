#ifndef MULTIPLEXER_H
#define MULTIPLEXER_H 

#include <monitoring/multiplexer/MultiplexerListener.hpp>
#include <core/component/Component.hpp>

namespace monitoring{


/**
 * ActivityMultiplexer
 * Forwards logged activities to registered listeners (e.g. Plugins) either
 * in an syncronised or asyncronous manner.
 */
template <class TYPE>
class Multiplexer : core::Component // TODO: inherit component
//class Multiplexer
{
public:
	/**
	 * Called by layer to report about activity, passes activities to sync listeners
	 * and enqueqes activity for async dispatch.
	 */
	virtual void Log(TYPE * element) {};

	/**
	 * Register listener to multiplexer
	 *
	 * @param	ActivityMultiplexerListener *	listener	listener to notify in the future
	 */
	virtual void registerListener(MultiplexerListener<TYPE> * listener) {};

	/**
	 * Unregister listener from multiplexer
	 *
	 * @param	ActivityMultiplexerListener *	listener	listener to remove
	 */
	virtual void unregisterListener(MultiplexerListener<TYPE> * listener) {};


	/**
	 * pass termination to notifiers 
	 */
	virtual void terminate() {};

	// TODO: uncomment when component inheritance can work
	
	void init(core::ComponentOptions * options) {};
	core::ComponentOptions * get_options() { return new core::ComponentOptions(); };
	void shutdown() {};
	
};


}
#endif /* MULTIPLEXER_H */
