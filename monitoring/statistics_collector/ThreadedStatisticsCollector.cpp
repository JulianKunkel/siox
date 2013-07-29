#include <monitoring/activity_multiplexer/ActivityPluginDereferencing.hpp>

#include <monitoring/statistics_collector/StatisticsCollectorImplementation.hpp>

#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include "errors.h"
#include "monitoring/statistics_collector/ThreadedStatisticsCollector.hpp"

using namespace std;
using namespace core;
using namespace monitoring;

/*
 * ?Single threaded?, ?compute next waittime based on options.?
 */



class ThreadedStatisticsCollector: StatisticsCollector{
private:
	// ActivityPluginDereferencing * facade;
	// Statistics Multiplexer

public:
	/**
	 *
	 */
	virtual void registerPlugin(StatisticsProviderPlugin * plugin){

	}

	/**
	 *
	 */
	virtual void unregisterPlugin(StatisticsProviderPlugin * plugin){

	}

	/**
	 *
	 */
	virtual array<StatisticsValue,10> getStatistics(StatisticsIntervall intervall, StatisticsDescription & stat){

	}

	/**
	 *
	 */
	virtual StatisticsValue getStatistics(StatisticsIntervall intervall, StatisticsDescription & stat, StatisticsReduceOperator op){

	}

	/**
	 * Available metrics
	 */
	virtual list<StatisticsProviderDatatypes> availableMetrics(){
		auto lst = list<StatisticsProviderDatatypes>();



		for(auto itr = currentValues.begin(); itr != currentValues.end(); itr++){
			string name = itr->first;
			//cout << name << endl;
			//Add the 11 metrics
			uint64_t overflow_value =  (uint64_t) 1<<63;//TODO CHECK ME, we expect 64 Bit...

			std::array<StatisticsValue, 11> & cur = currentValues[name];
			lst.push_back({INPUT_OUTPUT, NODE, "quantity/block/reads", {{"node", LOCAL_HOSTNAME}, {"device", name}}, cur[0], INCREMENTAL, "", "Field 1 -- # of reads issued", overflow_value, 0});
			lst.push_back({INPUT_OUTPUT, NODE, "quantity/block/reads/merged", {{"node", LOCAL_HOSTNAME}, {"device", name}}, cur[1], INCREMENTAL, "", "Field 2 -- # of reads merged", overflow_value, 0});
			lst.push_back({INPUT_OUTPUT, NODE, "quantity/block/dataRead", {{"node", LOCAL_HOSTNAME}, {"device", name}}, cur[2], INCREMENTAL, "Byte", "Data read based on Field 3 -- # of sectors read", overflow_value, 0});
			lst.push_back({INPUT_OUTPUT, NODE, "time/block/reads", {{"node", LOCAL_HOSTNAME}, {"device", name}}, cur[3], INCREMENTAL, "ms", "Field 4 -- # of milliseconds spent reading", overflow_value, 0});

			lst.push_back({INPUT_OUTPUT, NODE, "quantity/block/writes", {{"node", LOCAL_HOSTNAME}, {"device", name}}, cur[4], INCREMENTAL, "", "Field 5 -- # of writes completed", overflow_value, 0});
			lst.push_back({INPUT_OUTPUT, NODE, "quanity/block/writes/merged", {{"node", LOCAL_HOSTNAME}, {"device", name}}, cur[5], INCREMENTAL, "", "Field 6 -- # of writes merged", overflow_value, 0});
			lst.push_back({INPUT_OUTPUT, NODE, "quantity/block/dataWritten", {{"node", LOCAL_HOSTNAME}, {"device", name}}, cur[6], INCREMENTAL, "Byte", "Data written based on Field 7 -- # of sectors written", overflow_value, 0});
			lst.push_back({INPUT_OUTPUT, NODE, "time/block/writes", {{"node", LOCAL_HOSTNAME}, {"device", name}}, cur[7], INCREMENTAL, "ms", "Field 8 -- # of milliseconds spent writing", overflow_value, 0});
						
			lst.push_back({INPUT_OUTPUT, NODE, "quantity/block/pendingIOs", {{"node", LOCAL_HOSTNAME}, {"device", name}}, cur[8], SAMPLED, "", "Field 9 -- # of I/Os currently in progress", 0, 0});
			lst.push_back({INPUT_OUTPUT, NODE, "time/block/access", {{"node", LOCAL_HOSTNAME}, {"device", name}}, cur[9], INCREMENTAL, "ms", "Field 10 -- # of milliseconds spent doing I/Os", overflow_value, 0});
			lst.push_back({INPUT_OUTPUT, NODE, "time/block/weighted", {{"node", LOCAL_HOSTNAME}, {"device", name}}, cur[10], INCREMENTAL, "ms", "Field 11 -- weighted # of milliseconds spent doing I/Os", overflow_value, 0});
		}

		return lst;
	}

	}

	/* 
	 * Doubling ?
	 */
	virtual void init(ActivityPluginDereferencing * facade){
		this->facade = facade;
	}

	/**
	 * this method initiates first the options for threaded statitistics and second the facade of the ActivityPlugin
	 */
	virtual void init(ThreadedStatisticsOptions * options){
		ThreadedStatisticsOptions * o = (ThreadedStatisticsOptions*) options;

		// init facade etc.
		ActivityPluginDereferencing * facade = o->dereferingFacade.instance<ActivityPluginDereferencing>();
		init(facade);

		// init workq
		int workq_init (workq_t *wq, int threads, void (*engine)(void *arg))
		{
		int status;

		status = pthread_attr_init (&wq->attr);
		if (status != 0)
		return status;
		status = pthread_attr_setdetachstate (
		&wq->attr, PTHREAD_CREATE_DETACHED);
		if (status != 0) {
		pthread_attr_destroy (&wq->attr);
		return status;
		}
		status = pthread_mutex_init (&wq->mutex, NULL);
		if (status != 0) {
		pthread_attr_destroy (&wq->attr);
		return status;
		}
		status = pthread_cond_init (&wq->cv, NULL);
		if (status != 0) {
		pthread_mutex_destroy (&wq->mutex);
		pthread_attr_destroy (&wq->attr);
		return status;
		}
		wq->quit = 0;                       /* not time to quit */
		wq->first = wq->last = NULL;        /* no queue entries */
		wq->parallelism = threads;          /* max servers */
		wq->counter = 0;                    /* no server threads yet */
		wq->idle = 0;                       /* no idle servers */
		wq->engine = engine;
		wq->valid = WORKQ_VALID;
		return 0;
		}

	}




	/**
	 * get Available ThreadedStatisticsOptions
	 */
	virtual ComponentOptions * AvailableOptions(){
		return new ThreadedStatisticsOptions();
	}

	/**
	 * Shutdown functions such as save
	 */
	virtual void shutdown(){
		save();
	}
};

CREATE_SERIALIZEABLE_CLS(ThreadedStatisticsOptions)
CREATE_SERIALIZEABLE_CLS(ThreadedCollectorStatistics)

COMPONENT(ThreadedStatisticsCollector)
