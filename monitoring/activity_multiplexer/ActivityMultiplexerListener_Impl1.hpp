/*!
 * @file    ActivityMultiplexer_Impl1.hpp
 *
 * @description Synchronous and asynchronous listeners for the ActivityMultiplexer.
 * @standard    Preferred standard is C++11
 *
 * @author Jakob Luettgau, Marc Wiedemann, Julian Kunkel
 * @date   2013
 *
 */

/*!
 Software Structure
 1 Requirements

 2 UseCases

 3 Design und Text

 4 Testimplementation
 
 */


#ifndef ACTIVITYMULTIPLEXERLISTENER_IMPL1_H
#define ACTIVITYMULTIPLEXERLISTENER_IMPL1_H 



#include "../../include/monitoring/activity_multiplexer/ActivityMultiplexerListener.hpp"

/**
 * Sync Dummy Implementation of ActivityMultiplexer Sync Listener
 */
class ActivityMultiplexerListenerSync_Impl1 : public ActivityMultiplexerListenerSync
{
public:
	ActivityMultiplexerListenerSync_Impl1 ();
	virtual ~ActivityMultiplexerListenerSync_Impl1();
	void Notify(Activity * activity);

	void print();

private:
	static int nextID;
	int ID;
};

/**
 * Async Dummy Implementation of ActivityMultiplexer Async Listener
 */
class ActivityMultiplexerListenerAsync_Impl1 : public ActivityMultiplexerListenerAsync
{
public:
	ActivityMultiplexerListenerAsync_Impl1 ();
	virtual ~ActivityMultiplexerListenerAsync_Impl1();
	void Notify(Activity * activity);
	void Reset(int dropped);

	void print();
private:
	static int nextID;
	int ID;
};

#endif /* ACTIVITYMULTIPLEXERLISTENER_IMPL1_H */
