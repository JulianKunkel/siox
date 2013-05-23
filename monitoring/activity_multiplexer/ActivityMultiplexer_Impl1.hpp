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
friend class ActivityMultiplexerNotifier_Impl1;
public:


	bool Full() { return activities.size() >= capacity; };
	bool Empty() { return activities.size() == 0; };
	bool Overloaded() { return overloaded; };

	void Push(Activity * activity);
	Activity * Pull();



private:
	std::queue<Activity*> activities;

	// bounds
	unsigned int capacity = 10;  // TODO: offer other bounds (e.g. percentage of ram)

	// states
	bool overloaded;
	int dropped_count = 0;
	int dropped_before = 0;

	// multi-threading
	std::mutex mut;

	std::condition_variable is_not_full;
	std::condition_variable is_not_empty;

};



class ActivityMultiplexerNotifier_Impl1 : public ActivityMultiplexerNotifier
{
public:
	ActivityMultiplexerNotifier_Impl1( 
			ActivityMultiplexerQueue * queue, 
			std::list<ActivityMultiplexerListener*> * list);

	virtual ~ActivityMultiplexerNotifier_Impl1 ();

	void setQueue(ActivityMultiplexerQueue * queue); 
	void setListenerList(std::list<ActivityMultiplexerListener*> * list);
	void Run();

private:	
	ActivityMultiplexerQueue * activities;	
	std::list<ActivityMultiplexerListener*> * listeners_async;

	// state
	bool terminate;

	int max_wait_time;

};



/**
 * Activity Multiplexer Implementation 1
 * 
 * Implementation features:
 *  - immediate sync dispatch
 *  - threaded async dispatch
 *  - overload handling -> bounded buffer -> reset signal to listeners
 *
 */
class ActivityMultiplexer_Impl1 : public ActivityMultiplexer
{
public:
	ActivityMultiplexer_Impl1 ();
	virtual ~ActivityMultiplexer_Impl1 ();
	
	void Log(Activity * activity);

	void registerListener(ActivityMultiplexerListener * listener);
	void unregisterListener(ActivityMultiplexerListener * listener);

	// used for test purposed
	void unregisterRandom();

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

	// boundaries
	unsigned int max_sync_listeners = 20;
	unsigned int max_async_listeners = 20;

	// thread safety
	std::mutex mut;

	std::condition_variable sync_is_not_full;
	std::condition_variable sync_is_not_empty;

	std::condition_variable mod_sync;
	std::condition_variable mod_async;

	std::condition_variable async_is_not_full;
	std::condition_variable async_is_not_empty;
};


#endif /* ACTIVITYMULTIPLEXER_IMPL1_H */
