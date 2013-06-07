/**
 * @file    Activity.cpp
 *
 * @description Implementation of the Activity class.
 * @standard    Preferred standard is C++11
 *
 * @author Michaela Zimmer, Marc Wiedemann
 * @date   2013
 *
 */

#include "Activity.h"

using std::string;

namespace monitoring {

Activity::Activity()
{
};


Activity::Activity(ConnectionMessage &msg)
{
	aid_ = msg.get_msg()->aid();
	aid_ = msg.get_msg()->paid();
}


Activity::Activity(const Activity &act)
{
	aid_        = act.aid();
	paid_       = act.paid();
	cid_        = act.cid();
	time_start_ = act.time_start();
	time_stop_  = act.time_stop();
}


uint64_t Activity::aid() const
{
	return aid_;
};


uint64_t Activity::paid() const
{
	return paid_;
};


uint64_t Activity::cid() const
{
	return cid_;
};


uint64_t Activity::time_start() const
{
	return time_start_;
};


uint64_t Activity::time_stop() const
{
	return time_stop_;
};

	
string Activity::comment() const
{
	return comment_;
};


}