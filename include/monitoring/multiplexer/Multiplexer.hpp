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
class Multiplexer : core::Component
{
public:
	/**
	 * Called by layer to report about activity, passes activities to sync listeners
	 * and enqueqes activity for async dispatch.
	 */
	virtual void Log(TYPE * element) =0;

	/**
	 * Register listener to multiplexer
	 *
	 * @param	ActivityMultiplexerListener *	listener	listener to notify in the future
	 */
	virtual void registerListener(MultiplexerListener<TYPE> * listener) =0;

	/**
	 * Unregister listener from multiplexer
	 *
	 * @param	ActivityMultiplexerListener *	listener	listener to remove
	 */
	virtual void unregisterListener(MultiplexerListener<TYPE> * listener) =0;


	/**
	 * pass termination to notifiers 
	 */
	virtual void terminate() =0;

	void init(ComponentOptions * options);
	ComponentOptions * get_options();
	void shutdown();
};


}
#endif /* MULTIPLEXER_H */
