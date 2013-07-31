#include <monitoring/activity_multiplexer/ActivityPluginDereferencing.hpp>

#include <monitoring/statistics_collector/StatisticsCollectorImplementation.hpp>

#include <thread>
#include <iostream>
#include <mutex>
#include <cstdlib> //(stdlib.h)
#include <ctime> //(time.h)
#include "monitoring/statistics_collector/ThreadedStatisticsCollector.hpp"
#include "ThreadedStatisticsOptions.hpp"

using namespace std;
using namespace core;
using namespace monitoring;

/*
 * ?Single threaded? Yes, ?compute next waittime based on options.?
 */

//static const int PartsAsThreads = 10;
//std::thread t[num_threads];

//This function will be called from a thread
void call_from_thread(in Tid) {
    	     std::cout << "Launched by thread " << tid << std::endl;
    	}

class ThreadedStatisticsCollector: StatisticsCollector{
private:
	 ActivityPluginDereferencing * facade;
	// Statistics Multiplexer

public:
	/**
	 * Implementationsspezifisch
	 */
	virtual void registerPlugin(StatisticsProviderPlugin * plugin){

    	//Launch a group of threads
//        for (int i = 0; i < num_threads; ++i) {
//            t[i] = std::thread(call_from_thread, i);
        }

    	
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

// sectors are 512 bytes to see with  cat /sys/block/sda/queue/hw_sector_size
// /proc/diskstats field seven [cur 6] has total sectors written for example 35356*512bytes= 18102272 = 18,1MiB

			lst.push_back({INPUT_OUTPUT, NODE, "quantity/block/writes", {{"node", LOCAL_HOSTNAME}, {"device", name}}, cur[4], INCREMENTAL, "", "Field 5 -- # of writes completed", overflow_value, 0});
			lst.push_back({INPUT_OUTPUT, NODE, "quanity/block/writes/merged", {{"node", LOCAL_HOSTNAME}, {"device", name}}, cur[5], INCREMENTAL, "", "Field 6 -- # of writes merged", overflow_value, 0});
			lst.push_back({INPUT_OUTPUT, NODE, "quantity/block/dataWritten", {{"node", LOCAL_HOSTNAME}, {"device", name}}, cur[6], INCREMENTAL, "Byte", "Data written based on Field 7 -- # of sectors written", overflow_value, 0});
			lst.push_back({INPUT_OUTPUT, NODE, "time/block/writes", {{"node", LOCAL_HOSTNAME}, {"device", name}}, cur[7], INCREMENTAL, "ms", "Field 8 -- # of milliseconds spent writing", overflow_value, 0});
						
			lst.push_back({INPUT_OUTPUT, NODE, "quantity/block/pendingIOs", {{"node", LOCAL_HOSTNAME}, {"device", name}}, cur[8], SAMPLED, "", "Field 9 -- # of I/Os currently in progress", 0, 0});
			lst.push_back({INPUT_OUTPUT, NODE, "time/block/access", {{"node", LOCAL_HOSTNAME}, {"device", name}}, cur[9], INCREMENTAL, "ms", "Field 10 -- # of milliseconds spent doing I/Os", overflow_value, 0});
			lst.push_back({INPUT_OUTPUT, NODE, "time/block/weighted", {{"node", LOCAL_HOSTNAME}, {"device", name}}, cur[10], INCREMENTAL, "ms", "Field 11 -- weighted # of milliseconds spent doing I/Os", overflow_value, 0});
		}

// Derived Metrics = |{Metric}| + Renaming + Extra : Example quantity/block/dataRead -> throughput on node "" using device ""

		return lst;
	}

	}


	/* 
	 * Doubling ?
	 */
	virtual void init(ActivityPluginDereferencing * facade){
		this->facade = facade;

// init facade etc.
		ActivityPluginDereferencing * facade = o->dereferingFacade.instance<ActivityPluginDereferencing>();
		init(facade);

	}




	/**
	 * this method initiates first the options for threaded statitistics and second the facade of the ActivityPlugin
	 */
	virtual void getOptions(ThreadedStatisticsOptions * options){
		ThreadedStatisticsOptions * o = (ThreadedStatisticsOptions*) options;

		
	virtual void init(){
		ThreadedStatisticsOptions & o = getOptions<ThreadedStatisticsOptions>();

		//ActivityPluginDereferencing * facade = o->dereferingFacade.instance<ActivityPluginDereferencing>();
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
//CREATE_SERIALIZEABLE_CLS(ThreadedCollectorStatistics)

COMPONENT(ThreadedStatisticsCollector)
