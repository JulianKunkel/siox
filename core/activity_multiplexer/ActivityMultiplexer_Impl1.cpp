//@author Jakob Luettgau
//TODO Code Documentation - We will set up automatic recursive doc creation

#include "ActivityMultiplexer_Impl1.hpp"

#include "ActivityMultiplexer_Listener.hpp"

ActivityMultiplexer_Impl1:: ActivityMultiplexer_Impl1()
{

}

/**
 * Register listener in notify list
 *
 * @param[in]	pointer	ActivityMultiplexer_Impl1_Listener
 * @param[out]	Status	int
 */
void ActivityMultiplexer_Impl1::register_listener(ActivityMultiplexer_Listener * listener, bool async)
{
	if ( async )
	{
		listeners_mutex.lock();
		if ( listeners_in_use == 0 )
			listeners_async.push_back(listener);
		listeners_mutex.unlock();
	} else {
		listeners_mutex.lock();
		listeners.push_back(listener);
		listeners_mutex.unlock();
	}
}


/**
 * Unregister listener from notify list
 *
 * @param[in]	pointer	ActivityMultiplexer_Impl1_Listener
 * @param[out]	Status	int
 */
void ActivityMultiplexer_Impl1::unregister_listener(ActivityMultiplexer_Listener * listener)
{
	listeners_mutex.lock();
	if ( listeners_in_use == 0 )
		listeners.remove(listener);
		listeners_async.remove(listener);
	else 
		listeners_mutex.unlock();
}


/**
 * Notify Listeners
 *
 * @param[in]	Activity	Activity Object 
 */
void ActivityMultiplexer_Impl1::Log(Activity)
{
	listeners_mutex.lock();
	listeners_in_use++;
	listeners_mutex.unlock();

	if ( !has_capicity ) {
		if (!overload_mode) { 
			overload_mode = true;
			activity_buffer.push_back(RESET); // possible overloaded push?
		}
		activities_dropped++;
	} else {
		// what if we collect more data than we can handle
		// signal needed to not confuse plugins (might think system is slow)
		//
		// find solution to give throttle signal
		// 
		// TODO: check mutable iterators !!!
		std::list<ActivityMultiplexer_Listener>::iterator it;	
		for (it=listeners.begin(); it!=listeners.end(); ++it)
		{
			// TODO: measure overhead for each callback and aggregate
			// handle overload -> reset function to let plugins know about dropped logs
			//  -> Goals:
			//		keep siox profile low
			//
			// TODO: maybe in the future, priority for listeners
			it.Notify(Activity);
		}


		// TODO: async dispatch yet to be determined
		activity_buffer.push_back(Activity);
	}


	listeners_mutex.lock();
	listeners_in_use--;
	listeners_mutex.unlock();
}


