/** @file ActivityMultiplexerAsync.cpp
 *
 * Implementation of the Activity Multiplexer Interface
 *
 *
 *
 *
 * Possible Improvements
 * TODO global flags to discard activities
 * TODO measure response time of plugins (e.g. how fast does l->Notify() return)
 *
 *
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

namespace monitoring {

	// forward declarations
	class ActivityMultiplexerNotifier;
	class ActivityMultiplexerAsync;

	class Dispatcher
	{
	public:
		virtual void Dispatch(int lost, void * work) {	 printf("Dispatch(Dummy): %p", work); }
		//virtual void Dispatch(int lost, shared_ptr<Activity> work) {	 /* printf("Dispatch(Dummy): %p", work); */ }
	};


	/**
	 * A threadsafe (many producers, one consumer) queue implementation for the multiplexer to use
	 * The queue is also responsible for counting discarded activities.
	 */
	class ActivityMultiplexerQueue
	{
		friend class ActivityMultiplexerNotifier;

		std::mutex lock;
		std::condition_variable not_empty;

		const static int capacityExponent = 5;
		const static int capacity = 1 << capacityExponent;
		const static int indexMask = capacity - 1;
		shared_ptr<Activity> buffer[capacity];	//Writing is protected by `lock`, reading is protected by the values of `writeIndex` and `readIndex`.
		std::atomic<uint32_t> writeIndex, readIndex;	//Writing of `writeIndex` is protected by `lock`. Bits outside of the indexMask are undefined, and must be ignored when these variables are interpreted.
//		std::deque< shared_ptr<Activity> > queue;
//		unsigned int capacity = 10; // TODO specify by options

		bool overloaded = false;
		std::atomic<int> lost;
		std::atomic<bool> notified;
		std::atomic<bool> terminate;
		std::atomic<bool> terminated;


	public:
			ActivityMultiplexerQueue () : writeIndex(0), readIndex(0), lost(0), notified(false), terminate(false), terminated(false) {};
			virtual ~ActivityMultiplexerQueue () {};

			virtual bool isEmpty() {
				return !(indexMask & (writeIndex - readIndex));
			};


			/**
			 * Add an activity to the queue if there is capacity, set overload flag
			 * otherwhise.
			 *
			 * @param   activity     an activity that need to be dispatched in the future
			 */
			virtual void Push( shared_ptr<Activity> activity ) {
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
			};


			/**
			 * Get an activity from queue, returned element is popped.
			 * If this returns a null pointer, the caller must immediately forget that this queue ever existed - it might already be destructed.
			 *
			 * @return	Activity	an activity that needs to be dispatched to async listeners, or NULL if the queue has been finalized.
			 */
			virtual shared_ptr<Activity> Pop() {
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
			};

			virtual void finalize() {
				terminate = true;
				// this is important to wake up waiting pop/notifier
				not_empty.notify_one();
				while( !terminated ) ;	//Spin until we can safely destruct the object!
			}
	};



	class ActivityMultiplexerNotifier
	{
		friend class ActivityMultiplexerQueue;

	public:
			ActivityMultiplexerNotifier (Dispatcher * dispatcher, ActivityMultiplexerQueue * queue) {
				this->dispatcher = dispatcher;
				this->queue = queue;

				for( int i = 0; i < num_dispatcher; ++i ) {
					t.push_back( std::thread( &ActivityMultiplexerNotifier::Run, this ) );
				}
			};

			virtual ~ActivityMultiplexerNotifier () {
				for( auto it = t.begin(); it != t.end(); ++it ) {
					( *it ).join();
				}
			};

			virtual void Run() {
				assert(queue);
				// call dispatch of Dispatcher
				static uint64_t events = 0;

				// sleep(8); // for testing of overloading mode!

				while( shared_ptr<Activity> activity = queue->Pop() ) {
					int lost = 0;

					if( queue->overloaded && queue->isEmpty() ){
					
					    //	cout << "Overloading finished lost: " << queue->lost << endl;
					    lost = queue->lost;
					    queue->lost -= lost;	//not `= 0`, since we might have lost additional activities in the meantime.
					    queue->overloaded = false;
					}

					events++;

					// reasoning for void pointer?
					dispatcher->Dispatch( lost, (void *) &activity );
					//dispatcher->Dispatch(queue->lost, activity);
				}
				//cout << "Caught: " << events << endl;
			}

			virtual void finalize() {
				//printf("Notifier:  Finalizing\n");
				queue->finalize();
			}

	private:
			Dispatcher * dispatcher = nullptr;
			ActivityMultiplexerQueue * queue = nullptr;

			// notifier currently maintains own thread pool
			int num_dispatcher = 1;
			list<std::thread> t;
	};



	/**
	 * ActivityMultiplexer
	 * Forwards logged activities to registered listeners (e.g. Plugins) either
	 * in an syncronised or asyncronous manner.
	 */
	class ActivityMultiplexerAsync : public ActivityMultiplexer, public Dispatcher, public ComponentReportInterface {

	private:
			vector<ActivityMultiplexerListener *> listeners;

			ActivityMultiplexerQueue * queue = nullptr;
			ActivityMultiplexerNotifier * notifier = nullptr;

			boost::shared_mutex  asyncQueueMutex;

			// we only permit a single thread to call log() at a time
			//mutex sync_mutex;

			// statistics about operation:
			uint64_t lost_events = 0;
			uint64_t processed_activities = 0;

			uint64_t processed_events_in_async = 0;
	public:

			~ActivityMultiplexerAsync() {
				if ( notifier ) {
					notifier->finalize();
					delete notifier;
				}

				if ( queue )
					delete queue;

			}

		ComponentReport prepareReport(){
			ComponentReport rep;

			rep.addEntry("ASYNC_DROPPED_ACTIVITIES", {ReportEntry::Type::SIOX_INTERNAL_CRITICAL, lost_events});
			rep.addEntry("PROCESSED_ACTIVITIES", {ReportEntry::Type::SIOX_INTERNAL_INFO, processed_activities});
			rep.addEntry("PROCESSED_ACTIVITIES_ASYNC", {ReportEntry::Type::SIOX_INTERNAL_INFO, processed_events_in_async});

			return rep;
		}

			/**
			 * hand over activity to registered listeners
			 *
			 * @param	activity	logged activity
			 */
			void Log( const shared_ptr<Activity> & activity ) override {
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
			void Dispatch(int lost, void * work) override {
				shared_ptr<Activity> activity = *(shared_ptr<Activity>*) work;
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
			void registerListener( ActivityMultiplexerListener * listener ) override {
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
			void unregisterListener( ActivityMultiplexerListener * listener ) override {
				boost::unique_lock<boost::shared_mutex> lock( asyncQueueMutex );
				for(size_t i = listeners.size(); i--; ) {
					if(listeners[i] == listener) {
						listeners[i] = listeners.back();
						listeners.pop_back();
					}
				}
			}

			// Satisfy Component Requirements

			ComponentOptions * AvailableOptions() {
				return new ActivityMultiplexerAsyncOptions();
			}

			void init() {
//				ActivityMultiplexerAsyncOptions & options = getOptions<ActivityMultiplexerAsyncOptions>();

				queue = new ActivityMultiplexerQueue();
				notifier = new ActivityMultiplexerNotifier(dynamic_cast<Dispatcher *>(this), queue);
			}
	};

}


extern "C" {
	void * MONITORING_ACTIVITY_MULTIPLEXER_INSTANCIATOR_NAME()
	{
		return new ActivityMultiplexerAsync();
	}
}
