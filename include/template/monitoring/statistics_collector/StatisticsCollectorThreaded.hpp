/** @file StatisticsCollectorThreaded.hpp
 * Template for a threaded version of the statistics collector
 *
 * @author Marc Wiedemann, Julian Kunkel
 * @date   2013
 */

/**
 Software Structure
 
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
	What plugins (OSMem,cpustat,iostat,netstat,energystat,vmstat,likwid) are called from the provided statistics?
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

	When requirements rise wa can adjust the frequencies by small factors but we are limited by bandwith of the node's systembus as well as the microseconds of the /proc system in linux.

 3) Design and Text

	Lock relations:
	


 */

#ifndef STATISTICS_COLLECTOR_ASYNC_TEMPLATE_H
#define STATISTICS_COLLECTOT_ASYNC_TEMPLATE_H 

#include <iostream>

#include <list>
#include <deque>

#include <mutex>
#include <condition_variable>
#include <atomic>
#include <thread>

#include <monitoring/statistics_collector/ThreadedStatisticsCollector.hpp>


using namespace std;



	void init() {}; 

	core::ComponentOptions * AvailableOptions() { return new core::ComponentOptions(); };

};


}
#endif /* STATISTICS_COLLECTOR_ASYNC_TEMPLATE_H */
