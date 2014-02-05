/** @file ActivityMultiplexerAsync.cpp
 * @date 2013, 2014
 * @author Jakob Luettgau, Julian Kunkel, Nathanael Hübbe
 *
 * Implementation of the Activity Multiplexer Interface
 *
 * Possible Improvements
 * TODO global flags to discard activities
 * TODO measure response time of plugins (e.g. how fast does l->Notify() return)
 */

#include <assert.h>

#include <list>
#include <deque>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include <thread>

#include <boost/thread/shared_mutex.hpp>

#include <monitoring/datatypes/Activity.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerImplementation.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerListener.hpp>
#include <core/reporting/ComponentReportInterface.hpp>


#include "ActivityMultiplexerAsyncOptions.hpp"

using namespace core;
using namespace monitoring;

namespace {

	// forward declarations
	class ActivityMultiplexerNotifier;
	class ActivityMultiplexerAsync;

	/**
	 * A threadsafe (many producers, one consumer) queue implementation for the multiplexer to use
	 * The queue is also responsible for counting discarded activities.
	 */
	class ActivityMultiplexerQueue {
		public:
			ActivityMultiplexerQueue () : writeIndex(0), readIndex(0), lost(0), notified(false), terminate(false), terminated(false) {};

			bool isEmpty() { return !(indexMask & (writeIndex - readIndex)); };

			void Push( shared_ptr<Activity> activity );
			shared_ptr<Activity> Pop();
			uint64_t checkOverflowMode();	//Returns a count of lost activities.

			void finalize();

		private:
			std::mutex lock;
			std::condition_variable not_empty;

			const static int capacityExponent = 5;
			const static int capacity = 1 << capacityExponent;
			const static int indexMask = capacity - 1;
			shared_ptr<Activity> buffer[capacity];	//Writing is protected by `lock`, reading is protected by the values of `writeIndex` and `readIndex`.
			std::atomic<uint32_t> writeIndex, readIndex;	//Writing of `writeIndex` is protected by `lock`. Bits outside of the indexMask are undefined, and must be ignored when these variables are interpreted.

			bool overloaded = false;
			std::atomic<uint64_t> lost;
			std::atomic<bool> notified;
			std::atomic<bool> terminate;
			std::atomic<bool> terminated;
	};

	/**
	 * Just an encapsulation of the worker thread that waits on the ActivityMultiplexerQueue.
	 */
	class ActivityMultiplexerNotifier {
		public:
			ActivityMultiplexerNotifier( ActivityMultiplexerAsync * dispatcher, ActivityMultiplexerQueue * queue ) :
				dispatcher( dispatcher ),
				queue( queue ),
				worker( &ActivityMultiplexerNotifier::Run, this )
			{};

			void Run();

			void finalize() { queue->finalize(); }
			~ActivityMultiplexerNotifier () { worker.join(); };

		private:
			ActivityMultiplexerAsync * dispatcher = nullptr;
			ActivityMultiplexerQueue * queue = nullptr;

			std::thread worker;
			uint64_t lostActivities = 0;
	};

	/**
	 * ActivityMultiplexer
	 * Forwards logged activities to registered listeners (e.g. Plugins) either
	 * in an syncronised or asyncronous manner.
	 */
	class ActivityMultiplexerAsync : public ActivityMultiplexer, public ComponentReportInterface {
		public:
			ActivityMultiplexerAsync() : processed_activities(0) {}
			void init();
			ComponentOptions * AvailableOptions() { return new ActivityMultiplexerAsyncOptions(); }

			void registerListener( ActivityMultiplexerListener * listener ) override;
			void unregisterListener( ActivityMultiplexerListener * listener ) override;

			void Log( const shared_ptr<Activity> & activity ) override;
			void dispatch(int lost, const shared_ptr<Activity>& work);

			ComponentReport prepareReport();
			~ActivityMultiplexerAsync();

		private:
			vector<ActivityMultiplexerListener *> listeners;

			ActivityMultiplexerQueue * queue = nullptr;
			ActivityMultiplexerNotifier * notifier = nullptr;

			boost::shared_mutex  asyncQueueMutex;

			// statistics about operation:
			uint64_t lost_events = 0;
			std::atomic<uint64_t> processed_activities;
			uint64_t processed_events_in_async = 0;
	};

}	//namespace

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Add an activity to the queue if there is capacity, set overload flag
 * otherwhise.
 *
 * @param   activity     an activity that need to be dispatched in the future
 */
void ActivityMultiplexerQueue::Push( shared_ptr<Activity> activity ) {
	if( overloaded ) {
		lost++;
		return;
	}
	lock.lock();
		size_t curSize = indexMask & ( writeIndex - readIndex );
		//We cannot distinguish between the case that the two pointers are equal due to an empty or a full buffer, so we need to keep one slot empty.
		if( curSize < capacity - 1 ) {
			buffer[indexMask & writeIndex] = activity;
			std::atomic_thread_fence(std::memory_order_release);
			writeIndex++;
		}else{
			overloaded = true;
			lost++;
		}
	lock.unlock();
	if( curSize > capacity/2 && !notified ){
		//Signaling a condition variable is extremely expensive. Don't do this for every Activity we get.
		not_empty.notify_one();
	}
}


/**
 * Get an activity from queue, returned element is popped.
 * If this returns a null pointer, the caller must immediately forget that this queue ever existed - it might already be destructed.
 *
 * @return	Activity	an activity that needs to be dispatched to async listeners, or NULL if the queue has been finalized.
 */
shared_ptr<Activity> ActivityMultiplexerQueue::Pop() {
	if( isEmpty() ) {
		//The queue is empty. Sleep until it's not.
		notified = false;	//But first tell the writers that they must wake us again!
		std::unique_lock<std::mutex> l(lock);	//Only needed for the condition variable.

		if ( isEmpty() && ! terminate ){
			not_empty.wait(l, [=](){ return ( ! this->isEmpty() || terminate); });
		}
	}
	//We might just be woken up to be able to die...
	if ( terminate ) {
		terminated = true;
		return nullptr;
	}
	assert( !isEmpty() );

	std::atomic_thread_fence( std::memory_order_acquire );
	shared_ptr<Activity> result = buffer[indexMask & readIndex];
	buffer[indexMask & readIndex];	//This has two effects: a) it ensures that the Activity can be destructed, and b) it ensures that `result` is actually read before `readIndex` is incremented.
	std::atomic_thread_fence( std::memory_order_release );
	readIndex++;
	return result;
}

uint64_t ActivityMultiplexerQueue::checkOverflowMode() {
	if( !overloaded || !isEmpty() ) return 0;
	uint64_t result = lost;
	lost -= result;	//not `= 0`, since we might have lost additional activities in the meantime.
	overloaded = false;
	return result;
}

void ActivityMultiplexerQueue::finalize() {
	terminate = true;
	// this is important to wake up waiting pop/notifier
	not_empty.notify_one();
	while( !terminated ) ;	//Spin until we can safely destruct the object!
}

void ActivityMultiplexerNotifier::Run() {
	assert(queue);
	// call dispatch of ActivityMultiplexerAsync
	static uint64_t events = 0;

	// sleep(8); // for testing of overloading mode!

	while( shared_ptr<Activity> activity = queue->Pop() ) {
		uint64_t lost = queue->checkOverflowMode();
		lostActivities += lost;
		events++;
		dispatcher->dispatch( lost, activity );
	}
	//cout << "Caught: " << events << endl;
}

ActivityMultiplexerAsync::~ActivityMultiplexerAsync() {
	if ( notifier ) {
		notifier->finalize();
		delete notifier;
	}

	if ( queue )
		delete queue;

}

ComponentReport ActivityMultiplexerAsync::prepareReport(){
	ComponentReport rep;

	rep.addEntry("ASYNC_DROPPED_ACTIVITIES", {ReportEntry::Type::SIOX_INTERNAL_CRITICAL, lost_events});
	rep.addEntry("PROCESSED_ACTIVITIES", {ReportEntry::Type::SIOX_INTERNAL_INFO, (uint64_t)processed_activities});
	rep.addEntry("PROCESSED_ACTIVITIES_ASYNC", {ReportEntry::Type::SIOX_INTERNAL_INFO, processed_events_in_async});

	return rep;
}

/**
 * hand over activity to registered listeners
 *
 * @param	activity	logged activity
 */
void ActivityMultiplexerAsync::Log( const shared_ptr<Activity> & activity ) {
	processed_activities++;
	assert( activity != nullptr );

	if ( queue ) {
		queue->Push(activity);
	}
}

/**
 * Notify async listeners of activity
 *
 * @param	lost	lost activtiy count
 * @param	work	activtiy as void pointer to support abstract notifier
 */
void ActivityMultiplexerAsync::dispatch(int lost, const shared_ptr<Activity>& work) {
	shared_ptr<Activity> activity = work;
	assert( activity != nullptr );
	//boost::shared_lock<boost::shared_mutex> lock( asyncQueueMutex );
	for(auto l = listeners.begin(); l != listeners.end() ; l++){
		(*l)->NotifyAsync(lost, activity);
	}
	processed_events_in_async++;
	lost_events += lost;
}

/**
 * Register Listener to sync path
 *
 * @param	listener	listener to be registered
 */
void ActivityMultiplexerAsync::registerListener( ActivityMultiplexerListener * listener ) {
	boost::unique_lock<boost::shared_mutex> lock( asyncQueueMutex );
	listeners.push_back(listener);

	// snipped conserved for later use
	//boost::upgrade_lock<boost::shared_mutex> lock( asyncQueueMutex );
	// if () {
	//     boost::upgrade_to_unique_lock<boost::shared_mutex> lock( asyncQueueMutex );
	// }
}


/**
 * Unegister Listener from sync path
 *
 * @param	listener	listener to be unregistered
 */
void ActivityMultiplexerAsync::unregisterListener( ActivityMultiplexerListener * listener ) {
	boost::unique_lock<boost::shared_mutex> lock( asyncQueueMutex );
	for(size_t i = listeners.size(); i--; ) {
		if(listeners[i] == listener) {
			listeners[i] = listeners.back();
			listeners.pop_back();
		}
	}
}

void ActivityMultiplexerAsync::init() {
//		ActivityMultiplexerAsyncOptions & options = getOptions<ActivityMultiplexerAsyncOptions>();

	queue = new ActivityMultiplexerQueue();
	notifier = new ActivityMultiplexerNotifier( this, queue );
}


extern "C" {
	void * MONITORING_ACTIVITY_MULTIPLEXER_INSTANCIATOR_NAME()
	{
		return new ActivityMultiplexerAsync();
	}
}
