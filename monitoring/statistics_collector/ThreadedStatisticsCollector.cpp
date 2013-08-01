/**
 * @file    ThreadedStatisticsCollector.cpp
 *
 * @description An implementation of the (software) component for collecting statistical values.
 * @standard    Preferred standard is C++11
 *
 * @author Marc Wiedemann, Julian Kunkel
 * @date   2013
 *
 */


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


/*! Software Structure
 
 1) Requirements
	using standard threads (c++11)
	Time and Programming Sequence
	I.Measure time now t1
	II. a) For each plugin (OSMem,cpustat,iostat) p which is called to run now
		b) p = nexttimestep
		c) Compute the derived metrics
	III. For each metric
		a) deltatime
		b) ... SMUX notify
		c) sleep (expected waittime) = time until next plugin - deltatime
		d) compute next waittime based on options ?

1.1) Design
 	Generalities can be extracted from ontology.
 	The local Computing guideline is located in a local config file as they are not standard generalities.
	
	Scheme:

					Ontologie(Global standards)
						|
					Collector(node local with config)
			/			|		\		...
	Plugin OSMem	cpustat		iostat

	- One single thread for statistics, multiple are optional 


2) Use Cases
	What plugins (OSMem,cpustats,iostats,netstats,energystats,vmstat,likwid) are called from the provided statistics?
	What is their frequency?
	How often are they called?
 	To reduce traffic we could split the plugin usage over time.
	On a time sequence for example we could incorporate at 
	t-4 plugin 3 and 2,
	t-3 plugin 3 and 4,7
	t-2 plugin 3 and 5
	t-1 plugin 3 and 1,
	t0 All plugins
	t1 plugin 3 and 1,
	t2 plugin 3 and 2,
	t3  plugin 3 and 4,6
	t4 plugin 3 and 5,
	t5 plugin 3 and 1,
	t6 plugin 3 and 2,
	t7 plugin 3 and 4,7
	t8 plugin 3 and 5
	t9 plugin 3 and 1,
	t10 All plugins
	t11 plugin 3 and 1,
	t12 plugin 3 and 2,
	t13 plugin 3 and 4,6
	t14 plugin 3 and 5,
	t15 plugin 3 and 1,
	t16 plugin 3 and 2,
	t17 plugin 3 and 4,7
	t18 plugin 3 and 5
	t19 plugin 3 and 1,
	t10 All plugins

	Between [t0;t10] = [t10;t20] ... we may use one second as interval, whereas the smaller steps [t11;t12] are 0,1s(EU) = 0.1s(US)

	When requirements rise we can adjust the frequencies by small factors but we are limited by bandwith of the node's systembus as well as the microseconds of the /proc system in linux.

 3) Design and Text

	What about multiqueuing? How is the interaction with the StatisticsPlugin?
	First we can provide for the next timestep the plugins to execute.
	They result from the required interval of each specific StatisticPlugin.

	We provide a map for each specific metric on a specific interval frequency using the StatisticPlugins as follows:

	std::map < metric,<Interval,StatisticsPlugin> >

	We have five levels of intervals the Plugins may use:

	A double-linked list with no reallocation of memory but significant overhead compared to vector.

	Code: ?
	std::list<StatisticsProviderPlugins>[5] SPP {"OSMemUsage", "cpustats", "iostats", "netstats" ...};

	Succession of Timestages x: First the highest frequency than to longer time intervals
	0,1,2,...,x

	Example: 100ms,1s,10s,1m,10m

	We allow than these five intervals that are less error prone to implement than different frequencies that are not multiples of each other.

	The Statisticsplugin(Intervall x) can then decide which interval it wants.

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
// Renaming includes computing the sectorvalues to bytevalues (*512). If we want millisecond to seconds(/1000) and want the precise value we need a datatype conversion from int to double For now it's optional.
// The term "Extra" means bytes to MiB or GiB or E3 E6 E9 bytes as users wish.
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
