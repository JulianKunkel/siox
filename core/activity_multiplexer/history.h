//@author Marc Wiedemann

// history will be multiplexers (M1,M2,M3) aggregates without sliding windowcnode
// activity multiplexer is registered a synchronous or asynchronous callabck ande
// activity multiplexer is registered a synchronous or asynchronous callabck and forwards it layerwise to processwise aggregator
// callbacks direct streams 
// Every plugin stores in its own multiplexer its own relevant Information i.e. write, sequential write (SW), sequential write
// plugin pumps information in activity table-format to collecting daemon
// Callback call -> Then transition from State 1 to State 2, z.B. Sequential read = SR
// Per file on instance 
// Filter callback can build history for each plugin itself
// If plugins are only interested in write- they only store writes. Other open,read,write,close
// Who is responsible?
// Configurable is a central history and relevant history for each plugin
// Action table when two states are true the same time
// sliding window still exists
// Two parts: storage component and decision component
// Threads
// fadvise must be synchronous and should be polled after activity start
// Layer L1,L2,L3=MPI,POSIX,GPFS (Anomaly Plugin, Optimizing) 
// activity attribute,begin,start,stop,end
// Per process threadwise L1,L2,L3, sends to optimizer
// This is because it is 
// We want the aggregate L1,L2,L3 within one thread
// First Aggregation within Prozess on L1,L2,L3
// Instance wise 
// Only one dbpump is also multiplexer and there can be registered plugins
// register plugins an M1,M2,M3
// All anomaly recognizing plugins should be stored

#ifndef __SIOX_HISTORY
#define __SIOX_HISTORY

// This history must be threaded
 # include <pthread.h>

 //Keep track of requests to thread queue
 typedef struct tag_requests2threadq 
 {
	struct tag_requests2threadq *next;
	void *data;
 } workq_linkedlist_t;

 typedef struct tag_historythreadq
 {
	pthread_mutex_t 	mutex; 					// access control to queue
	pthread_cond_t		cond; 					// waiting for work assignment
	pthread_attr_t		attr;					// detached thread creation
	threadq_linkedlist_t 	*first, *last; 			// linked list of work items in queue
	int 				valid;					// work queue is valid
	int 				quit;					// request work queue to quit
	int 				parallelthreads;		// maximum count of threads
	int 				count;					// current number of executing threads
	int 				number_of_idle_t;		// number of idle threads
	void				(*engine)(void *arg);	// client engine
 } threadq_t;

 #define TREADQ_VALID 0xdec1992

 // Definition of work queue functions
 extern int threadq_init (
	threadq_t 	*wq,				// threads queue
	int 		parallelthreads,	// maximum count of threads
	void (*engine)(void *) ); 		// engine routine
 extern int threadq_destroy (threadq_t *wq);
 extern int threadq_amend (threadq_t *wq, void *data);


// history callback can be registered 
virtual register_cb(history_cb);

virtual unregister_cb(history_cb);

// multiple callbacks can be registered at once
virtual register_cb(batched_activities_cb);

virtual unregister_cb(batched_activities_cb);

// epoch 
long double (phase);

unsigned long long number_of_activities;

unsigned long long number_of_statictics;


// What is the freqency to call updated values? 1-20s? Stepwise gathering i.e. up to eight aggregation layers with 100ms overall duration each.
// To start with: 10s
// will be called after each cycle
class history_cb{
	virtual history_cb(){
	// updated activity
	new activity_state(activity_form, activity_description);
	// updated statistics
	new statistics_state(statistics_form, metrics_description);
	}
	~history_cb();
};

class batched_activities_cb
{
	// Better through through config file and implemented as module
	// phase: call interface every t seconds
	// number of activities: call batchwise 
	virtual batched_activities_cb(){
		new batched_interface{number_of_activities, phase};
	}
	~batched_activities_cb();
	
};

class batched_statistics_cb
{
	// This is the implementation of a batched statistics package to deliver to history_cb
public:
	virtual batched_statistics_cb(arguments);
	~batched_statistics_cb();

	/* data */
};

class history
{
public:
	virtual history(arguments);
	~history();

	/* data */
};

class number_of_statistics
{
public:
	virtual number_of_statictics(metrics_description);
	~number_of_statictics(metrics_description);

	/* data */
};

class history
{
public:
	virtual history(arguments);
	~history();

	/* data */
};

