#ifndef ACTIVITYMULTIPLEXERLISTENER_IMPL1_H
#define ACTIVITYMULTIPLEXERLISTENER_IMPL1_H 



#include "../../include/monitoring/activity_multiplexer/ActivityMultiplexerListener.hpp"

class ActivityMultiplexerListenerAsync_Impl1 : public ActivityMultiplexerListenerAsync
{
public:
	ActivityMultiplexerListenerAsync_Impl1 ();
	virtual ~ActivityMultiplexerListenerAsync_Impl1();
	void Notify(Activity * activity);

	void print();

private:
	static int nextID;
	int ID;
};


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


#endif /* ACTIVITYMULTIPLEXERLISTENER_IMPL1_H */
