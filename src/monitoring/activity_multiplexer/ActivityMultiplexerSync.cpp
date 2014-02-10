#include <assert.h>
#include <list>
#include <boost/thread/shared_mutex.hpp>

#include <monitoring/datatypes/Activity.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerImplementation.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerListener.hpp>

#include "ActivityMultiplexerSyncOptions.hpp"


using namespace core;
using namespace monitoring;

namespace {

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
//			assert( activity != nullptr );
//
//			for(auto l = listeners.begin(); l != listeners.end() ; l++){
//				(*l)->Notify(activity);
//			}
		}


		void registerForUcaid( UniqueComponentActivityID ucaid, ActivityMultiplexerListener * listener, Callback handler, bool async ) override;
		void unregisterForUcaid( UniqueComponentActivityID ucaid, ActivityMultiplexerListener * listener, bool async ) override;
		void registerCatchall( ActivityMultiplexerListener * listener, Callback handler, bool async ) override;
		void unregisterCatchall( ActivityMultiplexerListener * listener, bool async ) override;

		ComponentOptions * AvailableOptions() override {
			return new ActivityMultiplexerSyncOptions();
		}

		void init() {
		}
	};

}


void ActivityMultiplexerSyncImpl1::registerForUcaid( UniqueComponentActivityID ucaid, ActivityMultiplexerListener * listener, ActivityMultiplexer::Callback handler, bool async ) {
	assert( 0 && "TODO" ), abort();	///@todo
}


void ActivityMultiplexerSyncImpl1::unregisterForUcaid( UniqueComponentActivityID ucaid, ActivityMultiplexerListener * listener, bool async ) {
	assert( 0 && "TODO" ), abort();	///@todo
}


void ActivityMultiplexerSyncImpl1::registerCatchall( ActivityMultiplexerListener * listener, ActivityMultiplexer::Callback handler, bool async ) {
	assert( 0 && "TODO" ), abort();	///@todo
}


void ActivityMultiplexerSyncImpl1::unregisterCatchall( ActivityMultiplexerListener * listener, bool async ) {
	assert( 0 && "TODO" ), abort();	///@todo
}

extern "C" {
	void * MONITORING_ACTIVITY_MULTIPLEXER_INSTANCIATOR_NAME()
	{
		return new ActivityMultiplexerSyncImpl1();
	}
}
