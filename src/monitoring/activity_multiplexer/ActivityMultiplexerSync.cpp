/**
 * @author Julian Kunkel, Nathanael Hübbe
 * @date 2014
 */

#include <assert.h>
#include <unordered_map>
#include <boost/thread/shared_mutex.hpp>

#include <monitoring/datatypes/Activity.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerImplementation.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerListener.hpp>
#include <monitoring/activity_multiplexer/Dispatcher.hpp>
#include <util/ExceptionHandling.hpp>

#include "ActivityMultiplexerSyncOptions.hpp"


using namespace core;
using namespace monitoring;

namespace {

	/**
	 * ActivityMultiplexerSync
	 * Forwards logged activities to registered listeners (e.g. Plugins) in a synchronous manner.
	 */
	class ActivityMultiplexerSyncImpl1 : public ActivityMultiplexerSync{
		public:
			void init() {}
			ComponentOptions * AvailableOptions() override { return new ActivityMultiplexerSyncOptions(); }

			void Log( const shared_ptr<Activity> & activity ) override;

			void registerForUcaid( UniqueComponentActivityID ucaid, ActivityMultiplexerListener * listener, Callback handler, bool async ) override;
			void unregisterForUcaid( UniqueComponentActivityID ucaid, ActivityMultiplexerListener * listener, bool async ) override;
			void registerCatchall( ActivityMultiplexerListener * listener, Callback handler, bool async ) override;
			void unregisterCatchall( ActivityMultiplexerListener * listener, bool async ) override;

		private:
			boost::shared_mutex dispatchersLock;
			unordered_map<UniqueComponentActivityID, Dispatcher> dispatchers;	//protected by dispatchersLock
	};

}


void ActivityMultiplexerSyncImpl1::Log( const shared_ptr<Activity> & activity ) {
	dispatchersLock.lock_shared();
	auto found = dispatchers.find(activity->ucaid());
	if (  found != dispatchers.end() ){
		found->second.dispatch( activity, 0 );
	}
	found = dispatchers.find(0);
	if ( found != dispatchers.end() ){
		found->second.dispatch( activity, 0 );
	}
	dispatchersLock.unlock_shared();
}


void ActivityMultiplexerSyncImpl1::registerForUcaid( UniqueComponentActivityID ucaid, ActivityMultiplexerListener * listener, ActivityMultiplexer::Callback handler, bool async ) {
	if( async ) return;	//XXX I'm not sure whether silently ignoring this condition is the right approach, but it makes sync/async multiplexers interchangeable.
	dispatchersLock.lock();
	dispatchers[ucaid].add( listener, handler );
	dispatchersLock.unlock();
}


void ActivityMultiplexerSyncImpl1::unregisterForUcaid( UniqueComponentActivityID ucaid, ActivityMultiplexerListener * listener, bool async ) {
	if( async ) return;	//XXX I'm not sure whether silently ignoring this condition is the right approach, but it makes sync/async multiplexers interchangeable.
	dispatchersLock.lock();
	IGNORE_EXCEPTIONS( dispatchers.at( ucaid ).remove( listener ); );
	dispatchersLock.unlock();
}


void ActivityMultiplexerSyncImpl1::registerCatchall( ActivityMultiplexerListener * listener, ActivityMultiplexer::Callback handler, bool async ) {
	registerForUcaid( 0, listener, handler, async );
}


void ActivityMultiplexerSyncImpl1::unregisterCatchall( ActivityMultiplexerListener * listener, bool async ) {
	unregisterForUcaid( 0, listener, async );
}

extern "C" {
	void * MONITORING_ACTIVITY_MULTIPLEXER_INSTANCIATOR_NAME()
	{
		return new ActivityMultiplexerSyncImpl1();
	}
}
