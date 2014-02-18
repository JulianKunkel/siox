/**
 * Dispatcher
 *
 * A simple means to dispatch an activity to a number of listeners.
 *
 * @author Nathanael Hübbe
 * @date 2014
 */

#ifndef INCLUDE_GUARD_MONITORING_ACTIVITY_MULTIPLEXER_DISPATCHER
#define INCLUDE_GUARD_MONITORING_ACTIVITY_MULTIPLEXER_DISPATCHER

#include <monitoring/activity_multiplexer/ActivityMultiplexer.hpp>
#include <vector>

namespace monitoring {

	class ActivityMultiplexerListener;

	class Dispatcher {
		public:
			void add( ActivityMultiplexerListener* listener, ActivityMultiplexer::Callback function ) {
				listeners.push_back( ( CallbackSpec ){
					.listener = listener,
					.function = function
				} );
			}

			void remove( ActivityMultiplexerListener* listener ) {
				for( size_t i = listeners.size(); i--; ) {
					if( listeners[i].listener == listener ) {
						listeners[i] = listeners.back();
						listeners.pop_back();
					}
				}
			}

			void dispatch( const shared_ptr<Activity>& activity, int lostActivityCount ) {
				for( size_t i = listeners.size(); i--; ) ( listeners[i].listener->*listeners[i].function )( activity, lostActivityCount );
			}

		private:
			typedef struct CallbackSpec {
				ActivityMultiplexerListener* listener;
				ActivityMultiplexer::Callback function;
			} CallbackSpec;
			vector<CallbackSpec> listeners;
	};

}

#endif
