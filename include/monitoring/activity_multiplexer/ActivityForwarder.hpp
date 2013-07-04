#ifndef ACTIVITYFORWARDER_H
#define ACTIVITYFORWARDER_H 

#include <monitoring/datatypes/Activity.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerListener.hpp>

class ActivityForwarder : ActivityMultiplexerListener
{
public:
	void Notify(Activity * element) =0;

private:
	/* data */
};

#endif /* ACTIVITYFORWARDER_H */
