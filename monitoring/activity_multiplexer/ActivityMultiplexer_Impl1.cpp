#include <iostream>
#include <sstream>

#include <stdlib.h> // only used by unregisterRandom() 

#include "ActivityMultiplexer_Impl1.hpp"

// TODO: Testcase: chain of multiplexer TEST!

// TODO: Exceptions?

// TODO: templates for notifier und queue for general use, erstmal nicht
//		 Problem: templates created at compile time

// TODO: ??? merge queue and notifier,to simplefy template creation?
//		 it seems like they only exist together anyways

// TODO: maybe Notifier should manage its async listeners?

/**
 * ActivityMultiplexerQueue - Implementation 1
 * Features: overload handling, thread-safe
 */
void ActivityMultiplexerQueue_Impl1::Push(Activity * activity)
{
	// handle overloaded buffer
	if ( overloaded )
	{
		std::cout << "overloaded!\n";

		if ( Empty() ) {
			std::cout << "recovered!\n";
			overloaded = false;
			// issue SIGNAL
		} else {
			dropped_count++;
		}
	} else {

		if ( !Full() )
		{
			std::unique_lock<std::mutex> lock(mut);
			
			is_not_full.wait(lock, [=] { return activities.size() != capacity; });
			activities.push(activity);
			is_not_empty.notify_all();

		} else {
			overloaded = true;
			dropped_count = 1;
		}
	} 
	
}

Activity * ActivityMultiplexerQueue_Impl1::Pull()
{
	//Activity * activity = NULL;

	std::unique_lock<std::mutex> lock(mut);
	is_not_empty.wait(lock, [=] { return activities.size() > 0; });
	
	Activity * activity = activities.front();
	activities.pop();
	is_not_full.notify_all();
	
	// return lost
	return activity;
}


/**
 * ActivityMultiplexerNotifier - Implementation 1
 *
 */


ActivityMultiplexerNotifier_Impl1::ActivityMultiplexerNotifier_Impl1(
		ActivityMultiplexerQueue * queue,
		std::list<ActivityMultiplexerListener*> * list
) {

	activities = queue;	
	listeners_async = list;

	std::thread t(&ActivityMultiplexerNotifier_Impl1::Run, this);
	t.detach();
}

ActivityMultiplexerNotifier_Impl1::~ActivityMultiplexerNotifier_Impl1()
{

}

void ActivityMultiplexerNotifier_Impl1::setQueue(ActivityMultiplexerQueue * queue)
{
	activities = queue;	
}


void ActivityMultiplexerNotifier_Impl1::setListenerList(std::list<ActivityMultiplexerListener*> * list)
{
	listeners_async = list;
}


void ActivityMultiplexerNotifier_Impl1::Run()
{
	std::cout << "Notifier running.. \n";

	ActivityMultiplexerQueue_Impl1 * as = dynamic_cast<ActivityMultiplexerQueue_Impl1*>(activities); 

	bool delay_startup = false;

	while( !terminate_flag )
	{

		
		// TODO: handle starvation of activity



		//std::cout << "runs..\n";
		if (delay_startup)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(300));
			delay_startup = false;
		}
		
		//std::cout << "runs..\n";
		
		// if activities empty
		//		if terminate
		//			-> terminate
		// sleep
		//
		// test for reset
		// sleep until queue not empty

		//std::unique_lock<std::mutex> lock(as->mut);
		//as->is_not_empty.wait(lock, [=] { return activities->Empty(); });

		if ( !as->Overloaded() )
		{
			Activity * activity = activities->Pull();

			if ( activity != NULL )
			{
				// dispatch
				std::list<ActivityMultiplexerListener*>::iterator listener = listeners_async->begin();
				for ( ; listener != listeners_async->end(); ++listener) 
				{
					(*listener)->Notify(activity);	
				}
			}
		}


		// TODO: nice doku
		// design choice: plugins do not need to complete queue
		// problem is,finalize would get dependend..


	}

		

	// call to plugin finalize


	// if terminate

}




/**
 * ActivityMultiplexer - Implementation 1
 *
 */
int ActivityMultiplexer_Impl1::nextID = 0;

ActivityMultiplexer_Impl1::ActivityMultiplexer_Impl1()
{
	ID = nextID;
	nextID++;

	activities = new ActivityMultiplexerQueue_Impl1();
	// add Notifier and introduce to ActivityQueue and ListenersList
	notifier = new ActivityMultiplexerNotifier_Impl1(activities, &listeners_async);
}

ActivityMultiplexer_Impl1::~ActivityMultiplexer_Impl1() 
{

}


// caller should ensure threadsafety!!
//
void ActivityMultiplexer_Impl1::Log(Activity * activity)
{
	// Problem sync path is not threadsafe on the plugin side


	// readlock

	mut.lock();
	std::list<ActivityMultiplexerListener*>::iterator listener = listeners_sync.begin();
	
	for ( ; listener != listeners_sync.end(); ++listener) 
	{
		(*listener)->Notify(activity);	
	}
	mut.unlock();

	activities->Push(activity);

}

void ActivityMultiplexer_Impl1::registerListener(ActivityMultiplexerListener * listener)
{
	// TODO: actually iterators for std::list should not be invalidated, mutex might be omitted 
	std::unique_lock<std::mutex> lock(mut);
	if ( dynamic_cast<ActivityMultiplexerListenerAsync*>(listener) ) {
		async_is_not_full.wait(lock, [=] { return listeners_async.size() != max_async_listeners; });
		listeners_async.push_back(listener);
		async_is_not_empty.notify_all();
	} else {
		sync_is_not_full.wait(lock, [=] { return listeners_async.size() != max_sync_listeners; });
		listeners_sync.push_back(listener);
		sync_is_not_empty.notify_all();
	}
}

void ActivityMultiplexer_Impl1::unregisterListener(ActivityMultiplexerListener * listener)
{
	// list iterator is invalidated on erase therefor mutex
	std::unique_lock<std::mutex> lock(mut);
	if ( dynamic_cast<ActivityMultiplexerListenerAsync*>(listener) ) {
		async_is_not_empty.wait(lock, [=] { return listeners_async.size() > 0; });
		listeners_async.remove(listener);
		async_is_not_full.notify_all();
	} else {
		sync_is_not_empty.wait(lock, [=] { return listeners_sync.size() > 0; });
		listeners_sync.remove(listener);
		sync_is_not_full.notify_all();
	}

}



void ActivityMultiplexer_Impl1::unregisterRandom()
{	
	std::unique_lock<std::mutex> lock(mut);
	async_is_not_empty.wait(lock, [=] { return listeners_async.size() > 0; });

	//ActivityMultiplexerListener * listener = 

	//unregisterListener();
	
	async_is_not_empty.wait(lock, [=] { return listeners_async.size() > 0; });
}

