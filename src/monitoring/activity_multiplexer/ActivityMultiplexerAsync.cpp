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
	 * A threadsafe queue implementation for the multiplexer to use
	 * The queue is also responsible for counting discarded activities.
	 */
	class ActivityMultiplexerQueue
	{
		friend class ActivityMultiplexerNotifier;

		std::mutex lock;
		std::condition_variable not_empty;

		std::deque< shared_ptr<Activity> > queue;
		unsigned int capacity = 10; // TODO specify by options

		bool overloaded = false;
		int lost = 1;

		// old mutex
		std::mutex mut;

		bool terminate = false;


	public:
			ActivityMultiplexerQueue () {};
			virtual ~ActivityMultiplexerQueue () {};

			/**
			 * Check whether or not the queue has still capacity
			 *
			 * @return  bool	true = queue is full, false = not full
			 */
			virtual bool Full() {
				return ( queue.size() > capacity );
			};



			/* Check whether of not the queue is empty
			 *
			 * @return bool     true = queue is empty, false = not empty
			 */
			virtual bool Empty() {
				return queue.empty();
			};


			/**
			 * Check if queue is in overload mode
			 *
			 */
			virtual bool Overloaded() {
				return overloaded;
			};

			/**
			 * Add an activity to the queue if there is capacity, set overload flag
			 * otherwhise.
			 *
			 * @param   activity     an activity that need to be dispatched in the future
			 */
			virtual void Push(shared_ptr<Activity> activity) {
				//std::lock_guard<std::mutex> lock( mut );

				std::unique_lock<std::mutex> l(lock);

				if ( Overloaded() ) {
					lost++;
					return;
				}

				//printf("push %p\n", activity);

				if( ! Full() ) {
					queue.push_back( activity );
					if( queue.size() == 1 ){
						//cout << "NOTIFYING" << endl;
						not_empty.notify_one();
					}
				}else{
					//cout << "OVERLOADED!" << endl;
					overloaded = true;
					lost = 1;
				}				
			};


			/**
			 * Get an activity from queue, returned element is popped!
			 *
			 * @return	Activity	an activity that needs to be dispatched to async listeners
			 */
			virtual shared_ptr<Activity> Pop() {
				std::unique_lock<std::mutex> l(lock);

				if ( Empty() && ! terminate ){
					not_empty.wait(l, [=](){ return ( ! this->Empty() || terminate); });
				}

				if ( terminate ) {
					return nullptr;
				}

				auto activity = queue.front();
				queue.pop_front();
				return activity;
			};

			virtual void finalize() {
				//printf("Queue:  Finalizing\n");
				{
				std::unique_lock<std::mutex> l(lock);
				terminate = true;
				}
				
				// this is important to wake up waiting pop/notifier
				not_empty.notify_one();
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

				while( true ) {

					shared_ptr<Activity> activity = queue->Pop();

					if (! activity )
					{
					    break;
					}
					
					int lost = 0;

					if( queue->overloaded && queue->Empty() ){
					
					    //	cout << "Overloading finished lost: " << queue->lost << endl;
					    lost = queue->lost;
					    // TODO small race condition here
					    queue->lost = 0;
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
			list<ActivityMultiplexerListener *> 		listeners;

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

				// quick sync dispatch
				{
					//unique_lock<mutex> onlyOne( sync_mutex );

					for(auto l = listeners.begin(); l != listeners.end() ; l++){
						(*l)->Notify(activity);
					}
				}
				// add to async patch
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
				// 		boost::upgrade_to_unique_lock<boost::shared_mutex> lock( asyncQueueMutex );
				// }
			}


			/**
			 * Unegister Listener from sync path
			 *
			 * @param	listener	listener to be unregistered
			 */
			void unregisterListener( ActivityMultiplexerListener * listener ) override {
				boost::unique_lock<boost::shared_mutex> lock( asyncQueueMutex );
				listeners.remove(listener);
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
