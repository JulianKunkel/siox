//@author Jakob Luettgau 
//@author Marc Wiedemann
//TODO Code Documentation - We will set up automatic recursive doc creation
// In order not to exceed memory limit of 50% of available to avoid slowing the HW node,
// we make the delivery a size of activities a necessity as this an internal interface
// We give an adaptive interval for sizes of activities(attr,op-code,stat) and a maximal number of them i.e. 10bytes to 100mbytes 10^4
// C++11 std should be used

#ifndef ACTIVITYMULTIPLEXER_H
#define ACTIVITYMULTIPLEXER_H 

class Activity;
class ActivityMultiplexer_Listener;

#include <list>
#include <queue>

class ActivityQueue
{
public:
	/**
	 * Add activity to queue
	 *
	 * @param Activity
	 */
	virtual void Push(Activity);
	
	/**
	 * Get activity from queue
	 *
	 * @return	Activity 
	 */
	virtual Activity * Pull();

	/**
	 * Check capacity of activity queue
	 *
	 * @return bool  True when queue is full
	 */
	virtual bool Full();

private:
	virtual void Lock();
	virtual void Unlock();

};



class Notifier
{
public:
	/**
	 * So notifier starts working in case it did not already
	 *
	 */
	virtual void Wake();	
	
	
	/**
	 * Inform async listeners about dropped activities and reset them
	 *
	 * @param	int	dropped activities
	 */
	virtual void Reset(int activities_dropped);

};



class ActivityMultiplexer
{

public:

	/**
	 * Notify Listeners
	 *
	 * @param	Activity	Activity Object 
	 */
	virtual void Log(Activity);

	/**
	 * Register listener in notify list
	 *
	 * @param	pointer	ActivityMultiplexer_Listener
	 * @return	Status	int
	 */
	virtual void register_listener(ActivityMultiplexer_Listener * listener, bool async);

	/**
	 * Unregister listener from notify list
	 *
	 * @param	pointer	ActivityMultiplexer_Listener
	 * @return	Status	int
	 */
	virtual void unregister_listener(ActivityMultiplexer_Listener * listener);

};

#endif /* ACTIVITYMULTIPLEXER_H */
