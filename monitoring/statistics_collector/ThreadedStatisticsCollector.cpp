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

#include <thread> // header for threads
#include <chrono> // header for periodic timing
#include <iostream> // header that defines I/O stream objects
#include <mutex> // defines for mutex class
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

    The Statisticsplugin(Intervall x) can then decide which interval it wants.

Implementation details for the requirements of a StatisticsCollector:
    R4  A single thread will query the providers periodically and compute derived metrics at these timestamps.
        The thread will be started @ init() and stoped in the destructor.
        The thread function which is executed is called periodicBackgroundThreadLoop.
    R5| The options of the ThreadCollector contain the list of statistics to query, so the user can set it as options.
        The de/serialization is done using Boost.
    R6  Calculate average values for several periods consisting of 10 intervals, between 100ms and 60s.
    R8  Provide a rolling value for the last average value. This differs from R6 because the intervals in R6 are queried
        at fixed timestamps (every 10s for example), so when we ask for the last 10s at timestamp 19s we will get the value
        for the interval 0-10.
    R9| Use a list to keep all plugins for each intervall. The plugins should be executed for these intervalls.

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


class ThreadedStatisticsCollector: StatisticsCollector {
	private:
		ActivityPluginDereferencing * facade;
		// Statistics Multiplexer

		thread                  PeriodicThread1;
		mutex                   RecentEvents_lock;
		condition_variable      ConditiontoRun;

		bool terminated = false;

		ComponentOptions * AvailableOptions() {
			return new ComponentOptions();
		}

		list<StatisticsProviderPlugin *> plugins[INTERVALLS_NUMBER];

		void sleep_ms( unsigned int ms ) {
			using std::chrono::high_resolution_clock;
			using std::chrono::milliseconds;
			auto t0 = high_resolution_clock::now();
			std::this_thread::sleep_for( milliseconds( ms ) );
			auto t1 = high_resolution_clock::now();
			milliseconds total_ms = std::chrono::duration_cast<milliseconds>( t1 - t0 );

			std::cout << "This_thread_sleep (" << ms << ") milliseconds: " << total_ms.count() << "ms\n";
		}

		// One thread for periodic issuing
		virtual void PeriodicThreadLoop() {
			while( ! terminated ) {
				//Create permanent thread
				PeriodicThread1.join();
				//repeat_forever{loop for thread}

				//Do something
				sleep_ms( 98 );
				//for(;;){
				//}
				return 0;

				unique_lock<mutex> lock( RecentEvents_lock );
				if( terminated ) {
					break;
				}
				ConditiontoRun.wait_until( lock, timeout );
			}
		}

		void CalculateAverageValues() {

			std::vector<VariableDatatype> AverageValues; // Size = number of periods //first try 4
		}

	public:

		virtual void registerPlugin( StatisticsProviderPlugin * plugin ) {
			assert( plugin != nullptr );
			StatisticsIntervall minIntervall = plugin->minPollInterval();
			// the configInterval must be at least minIntervall.

			// TODO use the options as set by the user.
			StatisticsIntervall configInterval = minIntervall;

			// make sure that the plugin is only registered once, by iterating through the list.
			if( plugins.find( plugin ) != plugins.end() ) {
				assert( false );
			}

			// Add the plugin
			plugins[configInterval].push_back( plugin );
		}

		virtual void unregisterPlugin( StatisticsProviderPlugin * plugin ) {
			plugin[configInterval].erase( plugin );
		}

		virtual array<StatisticsValue, 10> getStatistics( StatisticsIntervall intervall, StatisticsDescription & stat ) {
		}

		// D8 - These statistics are up to date values for different intervals
		virtual StatisticsValue getRollingStatistics( StatisticsIntervall intervall, StatisticsDescription & stat ) {
			uint64_t hms = 0, s = 0, ts = 0, m = 0, tm = 0;
			vector <uint64_t> vRS {hms, s, ts, m, tm};

			return vRS;
		}

		// These are D6
		virtual StatisticsValue getReducedStatistics( StatisticsIntervall intervall, StatisticsDescription & stat, StatisticsReduceOperator op ) {

		}



		virtual list<StatisticsProviderDatatypes> availableMetrics() {
			auto lst = list<StatisticsProviderDatatypes>();

			return lst;
		}

		virtual ~ThreadedStatisticsCollector() {

		}

		~Thread1StandardImplementation() {
			RecentEvents_lock.lock();
			terminated = true;
			ConditiontoRun.notify_one();
			RecentEvents_lock.unlock();

			PeriodicThread1.join();
		}

		virtual void init() {
			ThreadedStatisticsOptions & options = getOptions<ThreadedStatisticsOptions>();
			poll_interval_ms = options.poll_interval_ms;

			//ActivityPluginDereferencing * facade = o->dereferingFacade.instance<ActivityPluginDereferencing>();

			periodicThread = thread( & Thread1StandardImplementation::PeriodicThreadLoop, this );
		}

		/**
		 * this method initiates first the options for threaded statitistics and second the facade of the ActivityPlugin
		 */
		virtual void getOptions( ThreadedStatisticsOptions * options ) {
			ThreadedStatisticsOptions * o = ( ThreadedStatisticsOptions * ) options;
		}



		/**
		 * get Available ThreadedStatisticsOptions
		 */
		virtual ComponentOptions * AvailableOptions() {
			return new ThreadedStatisticsOptions();
		}
};


CREATE_SERIALIZEABLE_CLS( ThreadedStatisticsOptions )
//CREATE_SERIALIZEABLE_CLS(ThreadedCollectorStatistics)

COMPONENT( ThreadedStatisticsCollector )
