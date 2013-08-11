/**
 * @file    ThreadedStatisticsCollector.cpp
 *
 * An implementation of the (software) component for collecting statistical values.
 *
 * @author Marc Wiedemann, Julian Kunkel
 * @date   2013
 *
 */

#include <thread> // header for threads
#include <iostream> // header that defines I/O stream objects
#include <mutex> // defines for mutex class
#include <cstdlib> //(stdlib.h) get C header stdlib
#include <ctime> //(time.h) get C header time

#include <monitoring/activity_multiplexer/ActivityPluginDereferencing.hpp>
#include <monitoring/statistics_collector/StatisticsCollectorImplementation.hpp>
#include <monitoring/statistics_collector/StatisticsCollector.hpp> // own definitions of classes functions used in this implementation

#include "ThreadedStatisticsOptions.hpp" // own options of this implementation

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
		b) ... SMUX notify - gets the tact intervals 100ms,1s,10s,60s,600s
		c) sleep (expected waittime) = time until next plugin - deltatime
		d) compute next waittime based on options ?

	Interaction with Statisticscollector SMux:
	StatisticsCollector offers intervalvalues for metrics as follows: 100ms,1s,10s,60s,600s
	StatisticsCollector calculates average values for 1s from 100ms values.
	StatisticsMultiplexer requests: \<SpecificPlugin\>(1s) i.e. quantity/block/dataWritten(1s)
	StatisticsCollector issus impulse "Do as requested" and allows the procedure.

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

Implementation details for the requirements of a StatisticsCollector:
	R4	A single thread will query the providers periodically and compute derived metrics at these timestamps.
		The thread will be started @ init() and stoped in the destructor.
		The thread function which is executed is called periodicBackgroundThreadLoop.
	R5	The options of the ThreadCollector contain the list of statistics to query, so the user can set it as options.
		The de/serialization is done using Boost.
	R6	Calculate average values for several periods consisting of 10 intervals, between 100ms and 60s.
	R9|	Use a list to keep for each intervall all plugins which should be executed for these intervalls.

	D6	As we have a fixed time base on every ten seconds we deliver for example four periods of average values: 4x 10*100ms StatisticIntervals.
		Every minute we deliver (four) periods of average values:  4x 10*1s
		Every ten minutes we deliver : 4x 10*10s
		Se we need a vector of size number of periods with the data of average values.
 */

public class 

class ThreadedStatisticsCollector: StatisticsCollector{
private:
	 ActivityPluginDereferencing * facade;
	// Statistics Multiplexer

	 list<StatisticsProviderPlugin*> plugins[INTERVALLS_NUMBER];

	 // One thread for periodic issuing
	 void PeriodicBackgroundThreadLoop(){
	 	//repeat_forever{loop for thread}
	 		//Create permanent thread
	 		std::thread PeriodicThread1();
	 		PeriodicThread1.join();
	 		return 0;


	 }

	 void CalculateAverageValues(){

	 	std::vector<VariableDatatype> AverageValues; // Size = number of periods //first try 4
	 }

public:
	virtual void registerPlugin(StatisticsProviderPlugin * plugin){		
		StatisticsIntervall minIntervall = plugin->minPollInterval();
		// the configInterval must be at least minIntervall.

		// TODO use the options as set by the user.
		StatisticsIntervall configInterval = minIntervall;

		// make sure that the plugin is only registered once, by iterating through the list.
		if (plugins.find(plugin) != plugins.end()) {
			assert(false);
		}

		// Add the plugin
		plugins[configInterval].push_back(plugin);
    }

	virtual void unregisterPlugin(StatisticsProviderPlugin * plugin){

	}

	virtual array<StatisticsValue,10> getStatistics(StatisticsIntervall intervall, StatisticsDescription & stat){

	}

	virtual StatisticsValue getRollingStatistics(StatisticsIntervall intervall, StatisticsDescription & stat){

	}


	virtual StatisticsValue getReducedStatistics(StatisticsIntervall intervall, StatisticsDescription & stat, StatisticsReduceOperator op){

	}



	virtual list<StatisticsProviderDatatypes> availableMetrics(){
		auto lst = list<StatisticsProviderDatatypes>();

		return lst;
	}

	virtual ~ThreadedStatisticsCollector(){

	}

	/**
	 * this method initiates first the options for threaded statitistics and second the facade of the ActivityPlugin
	 */
	virtual void getOptions(ThreadedStatisticsOptions * options){
		ThreadedStatisticsOptions * o = (ThreadedStatisticsOptions*) options;
	}
		
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
};


CREATE_SERIALIZEABLE_CLS(ThreadedStatisticsOptions)
//CREATE_SERIALIZEABLE_CLS(ThreadedCollectorStatistics)

COMPONENT(ThreadedStatisticsCollector)
