/**
 * @file ThreadedStatisticsCollector.hpp
 * @date 2013-07
 */
#include <thread>
#include <iostream>

/*! We have at least three ways of using pthreads for the ThreadedStatisticsCollector:
 * 1. Pipeline 
 * 2. Work crew
 * 3. Client/Server
 * Here we use a Work crew manager approach that creates a work queue that accepts work from a common queue. It should execute threads in parallel.
 */

/*
 * Structure to keep track of work queue requests. This is a linked list of work items
 */
typedef struct workq_internalrepres_tag {
    struct workq_internalrepres_tag         *next; //*next is a link element
    void                                    *data;
} workq_internalrepres_t;

/*
 * Structure describing a work queue.
 */
typedef struct workq_tag {
    pthread_mutex_t     mutex;
    pthread_cond_t      cv;             /* wait for work */
    pthread_attr_t      attr;           /* create detached threads */
    workq_internalrepres_t         *first, *last;  /* work queue */
    int                 valid;          /* set when valid */
    int                 quit;           /* set when workq should quit */
    int                 parallelism;    /* number of threads required */
    int                 counter;        /* current number of threads */
    int                 idle;           /* number of idle threads */
    void                (*engine)(void *arg);   /* user engine */
} workq_t;

#define WORKQ_VALID     0xdec1992

/*
 * Define work queue functions
 */
extern int workq_init (
    workq_t     *wq,
    int         threads,                /* maximum threads */
    void        (*engine)(void *));     /* engine routine */
extern int workq_destroy (workq_t *wq);
extern int workq_add (workq_t *wq, void *data);
