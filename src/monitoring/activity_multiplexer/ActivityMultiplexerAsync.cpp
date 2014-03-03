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
#include <monitoring/activity_multiplexer/Dispatcher.hpp>
#include <core/reporting/ComponentReportInterface.hpp>
#include <util/ExceptionHandling.hpp>


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
			void start() {
				terminate = false;
				terminated = false;
			}
			void stop() { 
				finalize();
			};
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
			ActivityMultiplexerNotifier( ActivityMultiplexerAsync * multiplexer, ActivityMultiplexerQueue * queue ) :
				multiplexer( multiplexer ),
				queue( queue )	{};

			void Run();

			void finalize() { queue->finalize(); }

			void start() {
				worker = new std::thread( & ActivityMultiplexerNotifier::Run, this );
			}
			void stop() { worker->join(); };

		private:
			ActivityMultiplexerAsync * multiplexer = nullptr;
			ActivityMultiplexerQueue * queue = nullptr;

			std::thread * worker = nullptr;
			uint64_t lostActivities = 0;
	};

	/**
	 * ActivityMultiplexer
	 * Forwards logged activities to registered listeners (e.g. Plugins) either
	 * in an syncronised or asyncronous manner.
	 */

	class ActivityMultiplexerAsync : public ActivityMultiplexer, public ComponentReportInterface {
		public:
			ActivityMultiplexerAsync() : notifier( this, &queue ), lost_events(0), processed_activities(0) {}
			void init() {}
			ComponentOptions * AvailableOptions() { return new ActivityMultiplexerAsyncOptions(); }

			void registerForUcaid( UniqueComponentActivityID ucaid, ActivityMultiplexerListener* listener, Callback handler, bool async ) override;
			void unregisterForUcaid( UniqueComponentActivityID ucaid, ActivityMultiplexerListener* listener, bool async ) override;
			void registerCatchall( ActivityMultiplexerListener* listener, Callback handler, bool async ) override;
			void unregisterCatchall( ActivityMultiplexerListener* listener, bool async ) override;

			void Log( const shared_ptr<Activity> & activity ) override;
			void dispatch(int lost, const shared_ptr<Activity>& work);

			ComponentReport prepareReport();
			~ActivityMultiplexerAsync();

			void start() override;
			void stop() override;

		private:
			boost::shared_mutex syncDispatchersLock;
			unordered_map<UniqueComponentActivityID, Dispatcher> syncDispatchers;	//protected by syncDispatchersLock
			boost::shared_mutex asyncDispatchersLock;
			unordered_map<UniqueComponentActivityID, Dispatcher> asyncDispatchers;	//protected by asyncDispatchersLock

			ActivityMultiplexerQueue queue;
			ActivityMultiplexerNotifier notifier;

			// statistics about operation:
			std::atomic<uint64_t> lost_events;
			std::atomic<uint64_t> processed_activities;
			uint64_t processed_events_in_async = 0;
	};

}	//namespace

////////////////////////////////////////////////////////////////////////////////////////////////////

void ActivityMultiplexerAsync::start(){
	queue.start();
	notifier.start();

	ActivityMultiplexer::start();
}

void ActivityMultiplexerAsync::stop(){
	queue.stop();

	notifier.stop();

	ActivityMultiplexer::stop();
}


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

		while( isEmpty() && ! terminate ){
			not_empty.wait_for(l, std::chrono::milliseconds(100), [=](){ return ( ! this->isEmpty() || terminate); });
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
	buffer[indexMask & readIndex] = NULL;	//This has two effects: a) it ensures that the Activity can be destructed, and b) it ensures that `result` is actually read before `readIndex` is incremented.
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
	// call dispatch of ActivityMultiplexerAsync
	static uint64_t events = 0;

	// sleep(8); // for testing of overloading mode!

	while( shared_ptr<Activity> activity = queue->Pop() ) {
		uint64_t lost = queue->checkOverflowMode();
		lostActivities += lost;
		events++;
		multiplexer->dispatch( lost, activity );
	}
	//cout << "Caught: " << events << endl;
}


ActivityMultiplexerAsync::~ActivityMultiplexerAsync() {
	notifier.finalize();
}


ComponentReport ActivityMultiplexerAsync::prepareReport(){
	ComponentReport rep;

	rep.addEntry("ASYNC_DROPPED_ACTIVITIES", {ReportEntry::Type::SIOX_INTERNAL_CRITICAL, (uint64_t)lost_events});
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

	syncDispatchersLock.lock_shared();
	IGNORE_EXCEPTIONS( syncDispatchers.at( activity->ucaid() ).dispatch( activity, 0 ); );
	IGNORE_EXCEPTIONS( syncDispatchers.at( 0 ).dispatch( activity, 0 ); );
	syncDispatchersLock.unlock_shared();
	queue.Push(activity);
}

/**
 * Notify async listeners of activity
 *
 * @param	lost	lost activtiy count
 * @param	work	activtiy as void pointer to support abstract notifier
 */

void ActivityMultiplexerAsync::dispatch(int lost, const shared_ptr<Activity>& work) {
	asyncDispatchersLock.lock_shared();
	IGNORE_EXCEPTIONS( asyncDispatchers.at( work->ucaid() ).dispatch( work, lost ); );
	IGNORE_EXCEPTIONS( asyncDispatchers.at( 0 ).dispatch( work, lost ); );
	asyncDispatchersLock.unlock_shared();
	processed_events_in_async++;
	lost_events += lost;
}


void ActivityMultiplexerAsync::registerForUcaid( UniqueComponentActivityID ucaid, ActivityMultiplexerListener* listener, ActivityMultiplexer::Callback handler, bool async ) {
	if( async ) {
		asyncDispatchersLock.lock();
		asyncDispatchers[ucaid].add( listener, handler );
		asyncDispatchersLock.unlock();
	} else {
		syncDispatchersLock.lock();
		syncDispatchers[ucaid].add( listener, handler );
		syncDispatchersLock.unlock();
	}
}


void ActivityMultiplexerAsync::unregisterForUcaid( UniqueComponentActivityID ucaid, ActivityMultiplexerListener* listener, bool async ) {
	if( async ) {
		asyncDispatchersLock.lock();
		IGNORE_EXCEPTIONS( asyncDispatchers.at( ucaid ).remove( listener ); );
		asyncDispatchersLock.unlock();
	} else {
		syncDispatchersLock.lock();
		IGNORE_EXCEPTIONS( syncDispatchers.at( ucaid ).remove( listener ); );
		syncDispatchersLock.unlock();
	}
}


void ActivityMultiplexerAsync::registerCatchall( ActivityMultiplexerListener* listener, ActivityMultiplexer::Callback handler, bool async ) {
	registerForUcaid( 0, listener, handler, async );
}


void ActivityMultiplexerAsync::unregisterCatchall( ActivityMultiplexerListener* listener, bool async ) {
	unregisterForUcaid( 0, listener, async );
}


extern "C" {
	void * MONITORING_ACTIVITY_MULTIPLEXER_INSTANCIATOR_NAME()
	{
		return new ActivityMultiplexerAsync();
	}
}
