#ifndef ACTIVITYFORWARDERTHREADED_H
#define ACTIVITYFORWARDERTHREADED_H

#include <monitoring/datatypes/Activity.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexer.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerListener.hpp>


/**
 * Forward an Activity from one Multiplexer to another one but in a threadsafe
 * manner, so the receiving multiplexer does not need to be thread safe.
 */
class ActivityForwarderThreaded : ActivityMultiplexerListener
{
public:
	ActivityForwarder(const ActivityMultiplexer & in, const ActivityMultiplexer & out) =0;


	void Notify(Activity * element) =0;

private:
	/* data */
};

#endif /* ACTIVITYFORWARDERTHREADED_H */
