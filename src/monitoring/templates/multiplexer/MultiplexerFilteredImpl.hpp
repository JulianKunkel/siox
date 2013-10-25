/** @file
 * observer pattern, for easy notification of listeners
 *
 * @author Jakob Luettgau, Julian Kunkel
 * @date   2013
 *
 * XXX: Currently this is dead code. It appears that the original purpose was to ease the implementation of a StatisticsMultiplexer while providing a general implementation of the observer pattern. However, if it is general, it should not be used for its original purpose due to the unnecessary overhead that it would entail. On the other hand, if it is adapted to fit its original purpose, it is not a general template anymore, and its code should not be at this place. I'm leaving it now the way it is, but I strongly believe that this file should simply be discarded.
 */

/**
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
        1.  snapshot list   -> memory + time panelty

        2.  change remove to null, check when iterating,
            clean up in additional loop, might skip if time is precious

        3.  boost::shared_lock (was actually proposend and rejected for C++11)
            would implement readers/writers

        4. readers count + mutex

    C++11 does not provide a shared lock by it self, reasoning:
    http://permalink.gmane.org/gmane.comp.lib.boost.devel/211180


    //std::unique_lock<std::mutex> lock(mut);
    //std::lock_guard<std::mutex> lock(mut);

 */

#ifndef MULTIPLEXER_TEMPLATE_H
#define MULTIPLEXER_TEMPLATE_H

#include <list>

#include <atomic>
#include <mutex>

#include <core/component/Component.hpp>

namespace monitoring {

	/**
	 * ActivityMultiplexer
	 * Forwards logged activities to registered listeners (e.g. Plugins) either
	 * in an syncronised or asyncronous manner.
	 */
	template <class TYPE, class PARENTLISTENER>
	class MultiplexerFilteredTemplate : TYPE {

			// hashmap


			list<PARENTLISTENER *> listeners;

			// thread safety, kept namespace verbose for clarity
			//FIXME: This is not threadsafe! An increment and a decrement of not_invalidating can occur concurrently, causing either the increment or the decrement to be lost. Sorry, I don't have the time to fix it now...
			std::mutex inc;
			std::mutex dec;
			int not_invalidating = 0;


		public:
			/**
			 * Called by layer to report about activity, passes activities to sync listeners
			 * and enqueqes activity for async dispatch.
			 */
			// TODO sadly for the mutexes it is always needed
			virtual void Log( TYPE * element ) {
				{
					std::lock_guard<std::mutex> lock( inc );
					not_invalidating++;
				}

				for( auto it = listeners.begin(); it != listeners.end(); ++it ) {

					//XXX: Filtering statistics at this point is not a good idea from a performance persective. With 100 statistics, 10 listeners requesting 10 statistics each, and 0.1 second polling interval this would already produce 9000 unsuccessful (i. e. exhaustive) searches per second. This is unnecessary overhead.
					// if element.metric is in (*it).requiredMetrics()
					{
						( *it )->Notify( element );
					}
				}

				{
					std::lock_guard<std::mutex> lock( dec );
					not_invalidating--;
				}

				// TODO cond.notify_one();
			}

			/**
			 * Register listener to multiplexer
			 *
			 * @param   listener    listener to notify in the future
			 */
			virtual void registerListener( PARENTLISTENER * listener ) {
				// exclusive, adding multiple listerns might result in race condition
				std::lock_guard<std::mutex> lock( inc );
				while( not_invalidating != 0 ) {
					// wait
					// TODO candidate for condition vairable?
				}
				listeners.push_back( listener );
			}

			/**
			 * Unregister listener from multiplexer
			 *
			 * @param   listener    listener to remove
			 */
			virtual void unregisterListener( PARENTLISTENER * listener ) {
				// exclusive, as removing may invalidate iterator
				std::lock_guard<std::mutex> lock( inc );
				while( not_invalidating != 0 ) {
					// wait
					// TODO candidate for condition vairable?
				}
				listeners.remove( listener );
			}

			/**
			 * prepare for shutdown of component
			 */
			virtual void finalize() {};
	};


}
#endif /* MULTIPLEXER_H */
