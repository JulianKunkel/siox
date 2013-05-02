//@author Jakob Luettgau
//TODO Code Documentation - We will set up automatic recursive doc creation

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
