#include <assert.h>
#include <list>
#include <boost/thread/shared_mutex.hpp>

#include <monitoring/datatypes/Activity.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerImplementation.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerListener.hpp>

#include "ActivityMultiplexerSyncOptions.hpp"


using namespace core;
using namespace monitoring;

namespace monitoring {

	/**
	 * ActivityMultiplexerSync
	 * Forwards logged activities to registered listeners (e.g. Plugins) in a synchronous manner.
	 */
	class ActivityMultiplexerSyncImpl1 : public ActivityMultiplexerSync{

	private:
		list<ActivityMultiplexerListener *> listeners;
	public:


		/**
		 * hand over activity to registered listeners
		 *
		 * @param	activity	logged activity
		 */
		void Log( const shared_ptr<Activity> & activity ) override{
			assert( activity != nullptr );

			for(auto l = listeners.begin(); l != listeners.end() ; l++){
				(*l)->Notify(activity);
			}
		}


		void registerListener( ActivityMultiplexerListener * listener ) override{
			listeners.push_back(listener);
		}

		void unregisterListener( ActivityMultiplexerListener * listener ) override{
			listeners.remove(listener);
		}

		ComponentOptions * AvailableOptions() override {
			return new ActivityMultiplexerSyncOptions();
		}

		void init() {
		}
	};

}


extern "C" {
	void * MONITORING_ACTIVITY_MULTIPLEXER_INSTANCIATOR_NAME()
	{
		return new ActivityMultiplexerSyncImpl1();
	}
}
