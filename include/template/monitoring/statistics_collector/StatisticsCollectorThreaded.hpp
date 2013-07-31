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
	- 
	
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
	void shutdown() {};	

};


}
#endif /* STATISTICS_COLLECTOR_ASYNC_TEMPLATE_H */
