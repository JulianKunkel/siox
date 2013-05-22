#ifndef ACTIVITYMULTIPLEXER_IMPL1_H
#define ACTIVITYMULTIPLEXER_IMPL1_H 

#include <list>
#include <queue>


#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

#include "../../include/monitoring/activity_multiplexer/ActivityMultiplexer.hpp"

#include "ActivityMultiplexerListener_Impl1.hpp"


/**
 * ActivityMultiplexerQueue_Impl1 Considerations
 * 
 */
class ActivityMultiplexerQueue_Impl1 : public ActivityMultiplexerQueue
{
public:
	ActivityMultiplexerQueue_Impl1();
	virtual ~ActivityMultiplexerQueue_Impl1();

	bool Full();	
	bool Empty();	
	void Push(Activity * activity);
	Activity * Pull();

private:
	// boost::message_queue? -> has priorities already built in
	std::queue<Activity*> activities;

	int capacity;


	// multi-threading
	std::mutex mut;
};



class ActivityMultiplexerNotifier_Impl1 : public ActivityMultiplexerNotifier
{
public:
	ActivityMultiplexerNotifier_Impl1 ();
	ActivityMultiplexerNotifier_Impl1( 
			ActivityMultiplexerQueue * queue, 
			std::list<ActivityMultiplexerListener*> * list);

	virtual ~ActivityMultiplexerNotifier_Impl1 ();

	void setQueue(ActivityMultiplexerQueue * queue); 
	void setListenerList(std::list<ActivityMultiplexerListener*> * list);
	void Wake();
	void doStuff();
	void Run();

private:	
	ActivityMultiplexerQueue * activities;	
	std::list<ActivityMultiplexerListener*> * listeners_async;

	int max_wait_time;

};




class ActivityMultiplexer_Impl1 : public ActivityMultiplexer
{
public:
	ActivityMultiplexer_Impl1 ();
	virtual ~ActivityMultiplexer_Impl1 ();
	
	void Log(Activity * activity);

	void registerListener(ActivityMultiplexerListener * listener);
	void unregisterListener(ActivityMultiplexerListener * listener);

private:
	// debug
	int ID;
	static int nextID;

	// listener management
	std::list<ActivityMultiplexerListener*> listeners_sync;
	std::list<ActivityMultiplexerListener*> listeners_async;

	// async path
	ActivityMultiplexerQueue * activities;
	ActivityMultiplexerNotifier * notifier;

	// TODO: move to queue
	bool overloaded;
	int dropped_count = 0;
	int dropped_before = 0;

	// boundaries
	unsigned int max_sync_listeners = 20;
	unsigned int max_async_listeners = 20;


	// thread safety
	std::mutex mut;

	std::condition_variable sync_is_not_full;
	std::condition_variable sync_is_not_empty;

	std::condition_variable async_is_not_full;
	std::condition_variable async_is_not_empty;
};


#endif /* ACTIVITYMULTIPLEXER_IMPL1_H */
