#include <iostream>
#include <sstream>

#include "ActivityMultiplexer_Impl1.hpp"



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

void ActivityMultiplexerQueue_Impl1::Push(Activity * activity)
{
	
	activities.push(activity);
    
}

Activity * ActivityMultiplexerQueue_Impl1::Pull()
{
	Activity * activity = NULL;

	activity = activities.front();
	activities.pop();
	
		
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


void ActivityMultiplexerNotifier_Impl1::Wake()
{
	// debug
	//std::cout << "Notifier -> Wake()" << std::endl;
	
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

void ActivityMultiplexer_Impl1::print()
{
	std::cout << "Muliplexer-" << ID;
}


void ActivityMultiplexer_Impl1::Log(Activity * activity)
{
	// debug
	std::cout << std::endl;
	this->print();
	std::cout << " -> Log(";
	activity->print();
	std::cout << ")" <<  std::endl;

	// logic
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
		// switch to overload mode
		// TODO
		std::cout << "MPL switch to overload mode!" << std::endl;
	}

}

void ActivityMultiplexer_Impl1::registerListener(ActivityMultiplexerListener * listener, bool async)
{
	// debug
	std::cout << std::endl;
	this->print();
	std::cout << " -> registerListener(";
	dynamic_cast<ActivityMultiplexerListener_Impl1*>(listener)->print();
	std::cout << ", " << async;
	std::cout << ")" <<  std::endl;

	// logic
	if ( async ) {
		listeners_async.push_back(listener);
	} else {
		listeners_sync.push_back(listener);
	}
}

void ActivityMultiplexer_Impl1::unregisterListener(ActivityMultiplexerListener * listener, bool async)
{
	// debug
	std::cout << std::endl;
	this->print();
	std::cout << " -> unregisterListener(";
	dynamic_cast<ActivityMultiplexerListener_Impl1*>(listener)->print();
	std::cout << ", " << async;
	std::cout << ")" <<  std::endl;

	// logic
	if ( async ) {
		listeners_async.remove(listener);
	} else {
		listeners_sync.remove(listener);
	}

}



