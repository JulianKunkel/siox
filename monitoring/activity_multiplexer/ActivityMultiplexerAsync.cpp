#include <boost/thread/shared_mutex.hpp>

#include <monitoring/datatypes/Activity.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerImplementation.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerListener.hpp>

#include "ActivityMultiplexerAsyncOptions.hpp"

using namespace core;
using namespace monitoring;

namespace monitoring {


	/**
	 * ActivityMultiplexer
	 * Forwards logged activities to registered listeners (e.g. Plugins) either
	 * in an syncronised or asyncronous manner.
	 */
	class ActivityMultiplexerAsync : public ActivityMultiplexer{
	private:
			list<ActivityMultiplexerListener *> 		listeners;
			list<ActivityMultiplexerListenerAsync *> 	listeners_async;

			boost::shared_mutex  listener_change_mutex;
	public:
			virtual void Log( Activity * element ){
				boost::shared_lock<boost::shared_mutex> lock( listener_change_mutex );
				for(auto l = listeners.begin(); l != listeners.end() ; l++){
					(*l)->Notify(element);
				}
			}

			virtual void registerListener( ActivityMultiplexerListener * listener ){
				boost::unique_lock<boost::shared_mutex> lock( listener_change_mutex );

				//boost::upgrade_lock<boost::shared_mutex> lock( listener_change_mutex );
				// if () {
				// 		boost::upgrade_to_unique_lock<boost::shared_mutex> lock( listener_change_mutex );
				// }

				listeners.push_back(listener);
			}

			virtual void unregisterListener( ActivityMultiplexerListener * listener ){
				boost::unique_lock<boost::shared_mutex> lock( listener_change_mutex );
				listeners.remove(listener);
			}

			virtual void registerAsyncListener( ActivityMultiplexerListenerAsync * listener ){
				boost::unique_lock<boost::shared_mutex> lock( listener_change_mutex );
				listeners_async.push_back(listener);
			}

			virtual void unregisterAsyncListener( ActivityMultiplexerListenerAsync * listener ){
				boost::unique_lock<boost::shared_mutex> lock( listener_change_mutex );
				listeners_async.remove(listener);
			}

			ComponentOptions * AvailableOptions() {
				return new ActivityMultiplexerAsyncOptions();
			}

			void init() {
				ActivityMultiplexerAsyncOptions & options = getOptions<ActivityMultiplexerAsyncOptions>();
			}
	};

}


CREATE_SERIALIZEABLE_CLS( ActivityMultiplexerAsyncOptions )

PLUGIN( ActivityMultiplexerAsync )
