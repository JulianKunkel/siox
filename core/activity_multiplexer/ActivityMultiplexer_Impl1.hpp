//@author Jakob Luettgau
//TODO Code Documentation - We will set up automatic recursive doc creation

#ifndef ACTIVITYMULTIPLEXER_H
#define ACTIVITYMULTIPLEXER_H 

class Activity;
class ActivityMultiplexer_Listener;

#include <list>
#include <queue>
#include <mutex>

#include "ActivityMultiplexer.hpp"


class Notifier
{
public:
	Notifier ();
	virtual ~Notifier ();

	void Reset(int activities_dropped);
	void Wake();	

private:
	/* data */
};



class ActivityMultiplexer_Impl1 : ActivityMultiplexer  
{

public:
	ActivityMultiplexer ();
	virtual ~ActivityMultiplexer ();

	// TODO: queue async Activities
	void Log(Activity);

	void register_listener(ActivityMultiplexer_Listener * listener, bool async);
	void unregister_listener(ActivityMultiplexer_Listener * listener);

private:
	std::list<ActivityMultiplexer_Listener*> listeners;
	std::list<ActivityMultiplexer_Listener*> listeners_async;

	// overload handling
	bool has_capicity = true;
	bool overload_mode = false;
	int activities_dropped = 0;

	// for async notifications
	// impl1 z.B. ringbuffer
	std::queue<Activity> activity_buffer;

	// mutex and semaphore to prevent pushing listeners while notifying
	std::mutex listeners_mutex;
	int listeners_in_use = 0;
};

#endif /* ACTIVITYMULTIPLEXER_H */
