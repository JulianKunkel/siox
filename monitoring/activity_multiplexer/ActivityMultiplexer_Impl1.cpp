#include <iostream>
#include <sstream>

#include "ActivityMultiplexer_Impl1.hpp"


// TODO: templates for notifier und queue for general use, erstmal nicht
//			Problem: templates created at compile time


/**
 * ActivityMultiplexerQueue - Implementation 1
 *
 */
ActivityMultiplexerQueue_Impl1::ActivityMultiplexerQueue_Impl1()
{

}

ActivityMultiplexerQueue_Impl1::~ActivityMultiplexerQueue_Impl1()
{		

}

bool ActivityMultiplexerQueue_Impl1::Full()
{
	return false;
}

bool ActivityMultiplexerQueue_Impl1::Empty()
{
	return false;
}

void ActivityMultiplexerQueue_Impl1::Push(Activity * activity)
{
	// handle lost
	std::unique_lock<std::mutex> lock(mut);
	activities.push(activity);
}

Activity * ActivityMultiplexerQueue_Impl1::Pull()
{
	Activity * activity = NULL;

	std::unique_lock<std::mutex> lock(mut);
	activity = activities.front();
	activities.pop();
	
	// return lost
	return activity;
}


/**
 * ActivityMultiplexerNotifier - Implementation 1
 *
 */
ActivityMultiplexerNotifier_Impl1::ActivityMultiplexerNotifier_Impl1()
{

}

ActivityMultiplexerNotifier_Impl1::ActivityMultiplexerNotifier_Impl1(
		ActivityMultiplexerQueue * queue,
		std::list<ActivityMultiplexerListener*> * list
) {
	activities = queue;	
	listeners_async = list;
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


	while(true)
	{
		//std::cout << "runs..\n";
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	// if activity null
	//		if terminate
	//			-> terminate
	// sleep
	//

	// test for reset

	
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

	// if terminate

}


void ActivityMultiplexerNotifier_Impl1::Wake()
{
	std::thread t(&ActivityMultiplexerNotifier_Impl1::Run, this);
	t.detach();
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

void ActivityMultiplexer_Impl1::Log(Activity * activity)
{
	// logic
	if ( overloaded )
	{
		if (activities->Empty() ) {
			overloaded = false;
			// SIGNAL
		} else {
			dropped_count++;
		}
	} 
	
	if ( !activities->Full() ) 
	{
		std::list<ActivityMultiplexerListener*>::iterator listener = listeners_sync.begin();
		
		for ( ; listener != listeners_sync.end(); ++listener) 
		{
			(*listener)->Notify(activity);	
		}

		activities->Push(activity);

		notifier->Wake();
	} else {
		overloaded = true;
		dropped_count = 1;
	}
}

void ActivityMultiplexer_Impl1::registerListener(ActivityMultiplexerListener * listener)
{
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
	std::unique_lock<std::mutex> lock(mut);
	if ( dynamic_cast<ActivityMultiplexerListenerAsync*>(listener) ) {
		async_is_not_empty.wait(lock, [=] { return listeners_async.size() > 0; });
		listeners_async.remove(listener);
	} else {
		sync_is_not_empty.wait(lock, [=] { return listeners_sync.size() > 0; });
		listeners_sync.remove(listener);
	}

}


