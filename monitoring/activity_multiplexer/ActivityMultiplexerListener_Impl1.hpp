#ifndef ACTIVITYMULTIPLEXERLISTENER_IMPL1_H
#define ACTIVITYMULTIPLEXERLISTENER_IMPL1_H 

#include "../../include/monitoring/activity_multiplexer/ActivityMultiplexerListener.hpp"
class ActivityMultiplexerListener_Impl1: public ActivityMultiplexerListener
{
public:
	ActivityMultiplexerListener_Impl1 ();
	virtual ~ActivityMultiplexerListener_Impl1();
	void Notify(Activity * activity);

	void print();

private:
	static int nextLID;
	int LID;
	/* data */
};


#endif /* ACTIVITYMULTIPLEXERLISTENER_IMPL1_H */
