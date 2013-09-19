/**
 * @file    ThreadedStatisticsCollector.cpp
 *
 * An implementation of the (software) component for collecting statistical values.
 *
 * @author Marc Wiedemann, Julian Kunkel
 * @date   2013
 *
 */


#include <core/component/Component.hpp>
#include <monitoring/statistics_collector/StatisticsCollectorImplementation.hpp>
#include <monitoring/statistics_collector/StatisticsCollector.hpp> // own definitions of classes functions used in this implementation
#include "ThreadedStatisticsOptions.hpp" // own options of this implementation
#include "StatisticsProviderPluginData.hpp"

#include <thread> // header for threads
#include <atomic>
#include <chrono> // header for periodic timing
#include <iostream> // header that defines I/O stream objects
#include <boost/thread/shared_mutex.hpp> // defines for mutex class
#include <condition_variable> // for conditions
#include <cstdlib> //(stdlib.h) get C header stdlib
#include <ctime> //(time.h) get C header time

#include <knowledge/activity_plugin/ActivityPluginDereferencing.hpp>


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

                    Ontologie (Global standards)
                        |
                    Collector (node local with config)
            /           |       \       ...
    Plugin OSMem    cpustat     iostat

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

    The Statisticsplugin(Interval x) can then decide which interval it wants.

Implementation details for the requirements of a StatisticsCollector:
    R4  A single thread will query the providers periodically and compute derived metrics at these timestamps.
        The thread will be started @ init() and stoped in the destructor.
        The thread function which is executed is called pollingThreadMain().
    R5| The options of the ThreadCollector contain the list of statistics to query, so the user can set it as options.
        The de/serialization is done using Boost.
    R6  Calculate average values for several periods consisting of 10 intervals, between 100ms and 60s.
    R8  Provide a rolling value for the last average value. This differs from R6 because the intervals in R6 are queried
        at fixed timestamps (every 10s for example), so when we ask for the last 10s at timestamp 19s we will get the value
        for the interval 0-10.
    R9| Use a list to keep all plugins for each interval. The plugins should be executed for these intervals.

    D6  As we have a fixed time base on every ten seconds we deliver for example four periods of average values: 4x 10*100ms StatisticIntervals.
        Every minute we deliver (four) periods of average values:  4x 10*1s
        Every ten minutes we deliver : 4x 10*10s
        Se we need a vector of size number of periods with the data of average values.
        Periodic Handling:
        1. Provide enough threads that can invoke a method
        2. Handle timer in dedicated thread
        Timer is in time.h or thread that sleeps for certain in a loop as workaround
        Sleep has drawback: I.e. precision is 10ms Accuracy can be 200ms actual sleep.

        Implement math with doubles here and may use the general core/MathematicalEquation/MathValueType

    D8  This static value has to be exchanged every new interval time 10s for the last ten seconds, 1s for the last 1s and so on. So we may provide
        We need HUNDRED_MILLISECONDS, SECOND, TEN_SECONDS, MINUTE, TEN_MINUTES, so five values updated at different times.
        So keep a small vector with six entries.
*/

using namespace std;

class ThreadedStatisticsCollector: StatisticsCollector {
	private:
		ActivityPluginDereferencing * facade;
		// Statistics Multiplexer

		vector<shared_ptr<StatisticsProviderPluginData> > sources;
		boost::shared_mutex sourcesLock;

		thread pollingThread;
		size_t pollCount;
		volatile bool terminated = false;	//This flag is only raised once to signal the polling thread to terminate.

		int64_t getMicroSeconds() throw() {
			struct timeval curTime;
			gettimeofday(&curTime, NULL);
			return 1000*1000ll*curTime.tv_sec + curTime.tv_usec;
		}

		void microSecondSleep(int64_t microSeconds) throw() {
			struct timespec sleepTime;
			sleepTime.tv_sec = microSeconds/1000/1000;
			sleepTime.tv_nsec = (microSeconds - 1000*1000*sleepTime.tv_sec)*1000;
			nanosleep(&sleepTime, NULL);
		}

		// One thread for periodic issuing
		virtual void pollingThreadMain() throw() {
			using namespace std::chrono;
			int64_t nextPollTime = getMicroSeconds();
			while( ! terminated ) {
				sourcesLock.lock_shared();
				for( size_t i = sources.size(); i--; ) {
					sources[i]->doPolling();
				}
				sourcesLock.unlock_shared();
				// Sleep until it's time to poll again.
				// I have moved this from its own function, because we have to check `terminated` after every call to sleep_for.
				nextPollTime += 100*1000;
				int64_t curTime = getMicroSeconds();
				while(nextPollTime - curTime >= 0) {
					printf("sleeping for %ld micro seconds\n", nextPollTime - curTime);
					microSecondSleep(nextPollTime - curTime);
					curTime = getMicroSeconds();
					// Check whether we were awoken to terminate.
					atomic_thread_fence( memory_order_acquire );	//Make sure that the value of terminated is up to date.
					if( terminated ) return;
				}
			}
		}

		void CalculateAverageValues() throw() {
			vector<VariableDatatype> AverageValues; // Size = number of periods //first try 4
			assert(0), abort();	//TODO
		}

	public:

		virtual void registerPlugin( StatisticsProviderPlugin * plugin ) throw() {
			assert( plugin );
			StatisticsProviderPluginData* newSource = new StatisticsProviderPluginData( plugin );
			sourcesLock.lock();
			// Check whether this plugin is alread registered.
			for(size_t i = sources.size(); i--; ) {
				assert( sources[i]->getPlugin() != plugin && "Statistics Collector Plugins may only be registered with a StatisticsCollector once." );
			}
			// Add the plugin
			sources.emplace_back( newSource );
			sourcesLock.unlock();
		}

		virtual void unregisterPlugin( StatisticsProviderPlugin * plugin ) throw() {
			sourcesLock.lock();
			// Find the data object associated with the plugin and delete it.
			for( size_t i = sources.size(); i--; ) {
				if( sources[i]->getPlugin() == plugin ) {
					sources.erase(sources.begin() + i);
					return;
				}
			}
			sourcesLock.unlock();
		}

		virtual array<StatisticsValue, 10> getStatistics( StatisticsInterval interval, StatisticsDescription & stat ) throw() {
			assert(0), abort();	//TODO
		}

		// D8 - These statistics are up to date values for different intervals
		virtual StatisticsValue getRollingStatistics( StatisticsInterval interval, StatisticsDescription & stat ) throw() {
			uint64_t hms = 0, s = 0, ts = 0, m = 0, tm = 0;
			vector <uint64_t> vRS {hms, s, ts, m, tm};

			assert(0), abort();	//TODO
//			return vRS;
		}

		// These are D6
		virtual StatisticsValue getReducedStatistics( StatisticsInterval interval, StatisticsDescription & stat, StatisticsReduceOperator op ) throw() {
			assert(0), abort();	//TODO
		}



		virtual vector<StatisticsDescription> availableMetrics() throw() {
			sourcesLock.lock_shared();
			printf("Count of plugins: %ld\n", sources.size());
			for(size_t i = sources.size(); i--; ) printf("\tPlugin %ld: %ld metrics\n", i, sources[i]->availableMetrics().size());
			sourcesLock.unlock_shared();
			assert(0), abort();	//TODO
//			vector<StatisticsDescription> result;
//			sourcesLock.lock_shared();
//			// Make sure, we don't do any unnecessary copying...
//			size_t metricCount = 0;
//			for( size_t i = sources.size(); i--; ) {
//				metricCount += sources[i]->availableMetrics().size();
//			}
//			result.reserve(metricCount);
//			// Concatenate the lists.
//			for( size_t i = sources.size(); i--; ) {
//				const vector<StatisticsProviderDatatypes>& curMetrics = sources[i]->availableMetrics();
//				result.insert(result.back(), curMetrics.begin(), curMetrics.end());
//			}
//			sourcesLock.unlock_shared();
//			return result;
		}

		virtual ~ThreadedStatisticsCollector() throw() {
			terminated = true;
			atomic_thread_fence( memory_order_release );
			pollingThread.join();
			if( !sourcesLock.try_lock() ) {
				assert(0 && "Someone tried to destruct a ThreadedStatisticsCollector while another thread is still using it!"), abort();
			}
			sourcesLock.unlock();
		}

		virtual void init() throw() {
			//XXX I've taken out reading a polling interval from the options, because that should be determined from the StatisticsProviderPlugins.

			//ActivityPluginDereferencing * facade = o->dereferingFacade.instance<ActivityPluginDereferencing>();
			pollCount = 0;
			atomic_thread_fence( memory_order_release );
			pollingThread = thread( &ThreadedStatisticsCollector::pollingThreadMain, this );
		}

		/**
		 * this method initiates first the options for threaded statitistics and second the facade of the ActivityPlugin
		 * TODO: There is no declaration of this method in any superclass, so it can't be called from outside.
		 */
//		virtual void getOptions( ThreadedStatisticsOptions * options ) {
//			ThreadedStatisticsOptions * o = ( ThreadedStatisticsOptions * ) options;
//		}



		/**
		 * get Available ThreadedStatisticsOptions
		 */
		virtual ComponentOptions * AvailableOptions() throw() {
			return new ThreadedStatisticsOptions();
		}
};


extern "C" {
	void * STATISTICS_COLLECTOR_INSTANCIATOR_NAME()
	{
		return new ThreadedStatisticsCollector();
	}
}
