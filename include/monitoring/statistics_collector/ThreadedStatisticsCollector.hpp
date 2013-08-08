/**
 * @file    ThreadedStatisticsCollector.hpp
 *
 * A (software) component for collecting statistical values.
 *
 * @author Marc Wiedemann, Julian Kunkel
 * @date   2013
 *
 */

#ifndef THREADED_STATISTICS_COLLECTOR_H
#define THREADED_STATISTICS_COLLECTOR_H


#include <thread>
#include <iostream>
#include <array>
#include <list>
#include <string>


#include <core/component/Component.hpp>

#include <monitoring/datatypes/StatisticsTypes.hpp>
#include <monitoring/statistics_collector/StatisticsProviderDatatypes.hpp>

using namespace std;

/*! We have at least three ways of using threads for the ThreadedStatisticsCollector:
 * 1. Pipeline 
 * 2. Work crew
 * 3. Client/Server
 * Here we use a Work crew manager approach that creates a work queue that accepts work from a common queue. It should execute threads in parallel.
 */

/*
 * Structure to keep track of work queue requests. This is a linked list of work items
 */

namespace monitoring{

class ThreadedStatisticsProviderPlugin;

class ThreadedStatisticsCollector : public core::Component {
public:

    /*
     *  This virtual method is to register the metric, while calling it with its attribute and a special location such as "Storagedevice/SSD Blocklayer/sda"
     */
    virtual void registerPlugin(ThreadedStatisticsProviderPlugin * plugin) = 0;

    virtual void unregisterPlugin(ThreadedStatisticsProviderPlugin * plugin) = 0;

    /* 
     * The return value may be updated in the background?
     * Double buffering of values
     * 
     * We store statistics for the last:
     * - 10 seconds in 1 second increments
     * - 100 seconds in 10 second increments
     * - 10 minutes in 1 minute increments
     */
    virtual array<StatisticsValue,10> getStatistics(StatisticsIntervall intervall, StatisticsDescription & stat) = 0;

    virtual StatisticsValue getStatistics(StatisticsIntervall intervall, StatisticsDescription & stat, StatisticsReduceOperator op) = 0;

    // virtual StatisticsDescription & queryStatistics(StatisticsToQuery & stat) = 0;

    /*
     * What are the available source metrics and available sources for metrics if they are combined ones?
     */
    virtual list<StatisticsDescription> availableMetrics() = 0;
};

}


/*

typedef struct workq_internalrepres_tag {
    struct workq_internalrepres_tag         *next; //*next is a link element
    void                                    *data;
} workq_internalrepres_t;


// Structure describing a work queue.
/
typedef struct workq_tag {
    pthread_mutex_t     mutex;
    pthread_cond_t      cv;             // wait for work
    pthread_attr_t      attr;           // create detached threads
    workq_internalrepres_t         *first, *last;  // work queue
    int                 valid;          // set when valid
    int                 quit;           // set when workq should quit
    int                 parallelism;    // number of threads required
    int                 counter;        // current number of threads 
    int                 idle;           // number of idle threads 
    void                (*engine)(void *arg);   // user engine 
} workq_t;

#define WORKQ_VALID     0xdec1992


 // Define work queue functions

extern int workq_init (
    workq_t     *wq,
    int         threads,                // maximum threads
    void        (*engine)(void *));     // engine routine
extern int workq_destroy (workq_t *wq);
extern int workq_add (workq_t *wq, void *data);

*/


#define STATISTICS_INTERFACE "monitoring_statistics_collector"

#endif /* STATISTICS_COLLECTOR_H */






