/**
 * @file    activity.cpp
 *
 * @description Implementation of the Activity class.
 * @standard    Preferred standard is C++11
 *
 * @author Michaela Zimmer, Marc Wiedemann
 * @date   2013
 *
 */

#include <activity.hpp>


using std::string;


namespace monitoring
{

    Activity(void)
    {
    };

    ~Activity(void)
    {
    };

    uint64_t Activity::get_aid()
    {
        return aid;
    };

    uint64_t Activity::get_paid()
    {
        return paid;
    };

    uint64_t Activity::get_unid()
    {
        return unid;
    };

    Timestamp Activity::get_time_start()
    {
        return time_start;
    };

    Timestamp Activity::get_time_stop()
    {
        return time_stop;
    };

    string Activity::get_comment()
    {
        return comment;
    };


    static Activity Activity::getDummy()
    {
        Activity a = new Activity();

        a.aid = 42;
        a.paid = 1;
        a.unid = 13;
        a.time_start = 1234567;
        a.time_stop = 1234678;
        a.comment = "Dummy Activity; DO NOT FEED AFTER MIDNIGHT!!!";

        return a;
    };


}