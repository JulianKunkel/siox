/**
 * @file    ThreadedStatisticsCollector.cpp
 *
 * An implementation of the (software) component for collecting statistical values.
 *
 * @author Nathanael Hübbe, Julian Kunkel, Marc Wiedemann
 * @date   2013
 *
 */

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

    TODO: The following two points should  be moved to Statistics.hpp

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

#include <core/component/Component.hpp>
#include <monitoring/statistics/collector/StatisticsCollectorImplementation.hpp>
#include <monitoring/statistics/collector/StatisticsCollector.hpp>
#include <monitoring/statistics/StatisticsCollection.hpp>
#include "ThreadedStatisticsOptions.hpp"

#include <thread>
#include <atomic>
#include <iostream>
#include <boost/thread/shared_mutex.hpp>
#include <condition_variable>
#include <cstdlib>
#include <ctime>
#include <unordered_map>
#include <string>
#include <utility>
#include <unistd.h>
#include <limits.h>

#include <workarounds.hpp>

#include <util/ExceptionHandling.hpp>
#include <core/reporting/ComponentReportInterface.hpp>

#include <monitoring/statistics/Statistic.hpp>
#include <monitoring/statistics/collector/StatisticsProviderDatatypes.hpp>
#include <monitoring/statistics/collector/StatisticsProviderPlugin.hpp>
#include <monitoring/ontology/Ontology.hpp>
#include <monitoring/statistics/multiplexer/StatisticsMultiplexer.hpp>
#include <monitoring/datatypes/ids.hpp>
        
#include <util/time.h>


using namespace std;
using namespace core;
using namespace monitoring;

class ThreadedStatisticsCollector : public StatisticsCollector, public ComponentReportInterface {
	public:

		virtual void init() throw();

		virtual void registerPlugin( StatisticsProviderPlugin * plugin ) throw();
		//TODO: XXX: Do we really need to be able to unregister plugins?
		//I raise the question because of the Statistic objects, to which we provide access here:
		//We can delete them when their plugin is unregistered, this leaves external references dangling, likely invoking undefined behaviour.
		//We can keep them around, but that would insert stale statistics into our list of Statistics.
		//We can use shared_ptr<>, but that does not deal with the external references becoming stale.
		//Then again, I don't really see a use case for unregistering a plugin, so why bother with this functionality?
		virtual void unregisterPlugin( StatisticsProviderPlugin * plugin ) throw();

		virtual vector<shared_ptr<Statistic> > availableMetrics() throw();
		virtual std::shared_ptr<Statistic> getStatistic( const std::string& ontologyAttribute, const std::string& topologyPath ) throw();

		virtual void registerCollection( StatisticsCollection* collection ) throw();
		virtual void unregisterCollection( StatisticsCollection* collection ) throw();

		virtual array<StatisticsValue, Statistic::kHistorySize> getStatistics( StatisticsReduceOperator reductionOp, StatisticsInterval interval, const StatisticsDescription & stat ) throw();
		virtual StatisticsValue getRollingStatistics( StatisticsReduceOperator reductionOp, StatisticsInterval interval, const StatisticsDescription & stat ) throw();
		virtual StatisticsValue getReducedStatistics( StatisticsReduceOperator reductionOp, StatisticsInterval interval, const StatisticsDescription & stat ) throw();

		virtual ~ThreadedStatisticsCollector() throw();

		virtual ComponentReport prepareReport();

		/**
		 * get Available ThreadedStatisticsOptions
		 */
		virtual ComponentOptions * AvailableOptions() throw();

		void stop() override;
		void start() override;

	private:
		Topology* topology = 0;
		Ontology* ontology = 0;
		StatisticsMultiplexer* smux = 0;

		vector<StatisticsProviderPlugin*> plugins;	//protected by sourcesLock
		vector<StatisticsCollection*> collections;	//protected by sourcesLock
		vector<shared_ptr<Statistic> > statistics;	//protected by sourcesLock
		vector<StatisticsProviderPlugin*> pluginPerStatistics; //protected by sourcesLock
		unordered_map<pair<OntologyAttributeID, TopologyObjectId>, shared_ptr<Statistic> > index;	//protected by sourcesLock
		bool statisticsAdded = true, statisticsRemoved = true;
		boost::shared_mutex sourcesLock;

		thread pollingThread;
		size_t pollCount;
		volatile bool terminated = false;	//This flag is only raised once to signal the polling thread to terminate.

		void nanoSecondSleep(Timestamp nanoSeconds) throw();
		void pollingThreadMain() throw();	// One thread for periodic issuing

		// statistic counters for component reports
		uint64_t run_timesteps = 0; // number of performed iterations
		uint64_t process_time = 0;
};



ComponentReport ThreadedStatisticsCollector::prepareReport(){
	ComponentReport rep;


	rep.addEntry("AVAILABLE_STATISTICS", {ReportEntry::Type::SIOX_INTERNAL_INFO, statistics.size() });
	rep.addEntry("PROCESSED_TIMESTEPS", {ReportEntry::Type::SIOX_INTERNAL_INFO, run_timesteps});
	rep.addEntry("WAITING_TIME", {ReportEntry::Type::SIOX_INTERNAL_PERFORMANCE, (run_timesteps * 100 * 1000 * 1000ull - process_time) / 1000000000.0 });
	rep.addEntry("PROCESSING_TIME", {ReportEntry::Type::SIOX_INTERNAL_PERFORMANCE, process_time / 1000000000.0 });

	return rep;
}

void ThreadedStatisticsCollector::stop() {
	terminated = true;
	atomic_thread_fence( memory_order_release );
	pollingThread.join();
	if( !sourcesLock.try_lock() ) {
		assert(0 && "Someone tried to destruct a ThreadedStatisticsCollector while another thread is still using it!"), abort();
	}
	sourcesLock.unlock();

	StatisticsCollector::stop();
}

void ThreadedStatisticsCollector::start(){
	pollingThread = thread( &ThreadedStatisticsCollector::pollingThreadMain, this );

	StatisticsCollector::start();
}


void ThreadedStatisticsCollector::init() throw() {
	ThreadedStatisticsOptions& o = getOptions<ThreadedStatisticsOptions>();
	topology = GET_INSTANCE(Topology, o.topology);
	ontology =  GET_INSTANCE(Ontology, o.ontology);
	smux = GET_INSTANCE(StatisticsMultiplexer, o.smux);
	{
		//Setup the alias for the localhost in the topology.
		char hostname[HOST_NAME_MAX + 1];
		if( gethostname( hostname, sizeof( hostname ) ) ) {
			cerr << "Fatal error: gethostname() is not POSIX.1-2001 compliant on this system. Aborting.\n";
			abort();
		}
		string topologyPath = string( "host:" ) + string( hostname );
		if( !topology->registerObjectByPath( topologyPath ) ) {
			cerr << "SIOX internal error: Can't register the topology path \"" << topologyPath << "\". Aborting.\n";
			abort();
		}
		if( !topology->setAlias( "@localhost", topologyPath ) ) {
			cerr << "SIOX internal error: Inconsistent definitions of topology alias \"@localhost\". Aborting.\n";
			abort();
		}
	}
	pollCount = 0;
}

void ThreadedStatisticsCollector::registerPlugin( StatisticsProviderPlugin * plugin ) throw() {
	assert( plugin );
	sourcesLock.lock();
	// Check whether this plugin is already registered.
	if(find(plugins.begin(), plugins.end(), plugin) != plugins.end()) {
		assert( 0 && "Statistics Collector Plugins may only be registered with a StatisticsCollector once." ), abort();
	}

	// Add the plugin
	plugins.emplace_back( plugin );
	vector<StatisticsProviderDatatypes> metrics( plugin->availableMetrics() );
	for( size_t i = metrics.size(); i--; ) {
		OntologyAttributeID ontologyId = ontology->register_attribute( kStatisticsDomain, metrics[i].metrics , metrics[i].value.type() ).aID;
		if( TopologyObject device = topology->registerObjectByPath( metrics[i].topologyPath ) ) {
			TopologyObjectId topologyId = device.id();
			shared_ptr<Statistic> curStatistic;
			if( metrics[i].intervalType == INCREMENTAL ) {
				curStatistic = shared_ptr<Statistic>( new IncrementalStatistic( metrics[i].value, ontologyId, topologyId, metrics[i].overflow_next_value, metrics[i].overflow_max_value ) );
			} else {
				curStatistic = shared_ptr<Statistic>( new Statistic( metrics[i].value, ontologyId, topologyId ) );
			}
			pair<OntologyAttributeID, TopologyObjectId> curKey( ontologyId, topologyId );
			statistics.emplace_back( curStatistic );
			pluginPerStatistics.emplace_back( plugin );
			index[curKey] = curStatistic;
		} else {
			cerr << "SIOX error: Error registering topology path \"" << metrics[i].topologyPath << "\"\n";
			assert(false), abort();
		}
	}
	statisticsAdded = true;
	sourcesLock.unlock();
}

void ThreadedStatisticsCollector::unregisterPlugin( StatisticsProviderPlugin * plugin ) throw() {
	sourcesLock.lock();
	// Remove the plugin from the plugin list.
	for( size_t i = plugins.size(); i--; ) {
		if( plugins[i] == plugin ) {
			plugins.erase(plugins.begin() + i);
			break;
		}
	}

	// Remove the dependent Statistics from the statistics list.
	for( int i = statistics.size(); i--; ){
		if( pluginPerStatistics[i] == plugin ){
			pair<OntologyAttributeID, TopologyObjectId> key( statistics[i]->ontologyId, statistics[i]->topologyId);
			index.erase(key);

			pluginPerStatistics.erase( pluginPerStatistics.begin() + i );
			statistics.erase( statistics.begin() + i );
			break;
		}
	}
	statisticsRemoved = true;
	sourcesLock.unlock();
}

vector<shared_ptr<Statistic> > ThreadedStatisticsCollector::availableMetrics() throw() {
	sourcesLock.lock_shared();
	vector<shared_ptr<Statistic> > result = statistics;
	sourcesLock.unlock_shared();
	return result;
}

std::shared_ptr<Statistic> ThreadedStatisticsCollector::getStatistic( const std::string& ontologyAttribute, const std::string& topologyPath ) throw() {
	#define empty std::shared_ptr<Statistic>()
	TopologyObject object = topology->lookupObjectByPath( topologyPath );
	if( !object ) return empty;
	TopologyObjectId objectId = object.id();

	OntologyAttributeID ontologyId = 0;
	IGNORE_EXCEPTIONS( ontologyId = ontology->lookup_attribute_by_name( kStatisticsDomain, ontologyAttribute ).aID; );
	if( !ontologyId ) return empty;

	sourcesLock.lock_shared();
	std::shared_ptr<Statistic> result;
	for( size_t i = statistics.size(); i--; ) {
		shared_ptr<Statistic>& curStatistic = statistics[i];
		if( curStatistic->ontologyId == ontologyId && curStatistic->topologyId == objectId ) {
			result = curStatistic;
			break;
		}
	}
	sourcesLock.unlock_shared();
	return result;
	#undef empty
}

void ThreadedStatisticsCollector::registerCollection( StatisticsCollection* collection ) throw() {
	sourcesLock.lock();
	//sanity check
	for( size_t i = collections.size(); i--; ) {
		if( collections[i] == collection ) assert( 0 && "StatisticsCollections may only be registered with a StatisticsCollector once." ), abort();
	}
	//add the collection
	collections.push_back( collection );
	sourcesLock.unlock();
}

void ThreadedStatisticsCollector::unregisterCollection( StatisticsCollection* collection ) throw() {
	sourcesLock.lock();
	for( size_t i = collections.size(); i--; ) {
		if( collections[i] == collection ) {
			collections.erase( collections.begin() + i );
			goto success;
		}
	}
	assert( 0 && "Couldn't find collection." ), abort();
success:
	sourcesLock.unlock();
}


array<StatisticsValue, Statistic::kHistorySize> ThreadedStatisticsCollector::getStatistics( StatisticsReduceOperator reductionOp, StatisticsInterval interval, const StatisticsDescription & description ) throw() {
	array<StatisticsValue, Statistic::kHistorySize> result;
	pair<OntologyAttributeID, TopologyObjectId> key(description.ontologyId, description.topologyId);
	sourcesLock.lock_shared();
	if(Statistic* statistic = &*index[key]) {
		statistic->getHistoricValues( reductionOp, interval, &result, NULL );
	}
	sourcesLock.unlock_shared();
	return result;
}

StatisticsValue ThreadedStatisticsCollector::getRollingStatistics( StatisticsReduceOperator reductionOp, StatisticsInterval interval, const StatisticsDescription & description ) throw() {
	pair<OntologyAttributeID, TopologyObjectId> key(description.ontologyId, description.topologyId);
	Statistic* statistic = NULL;
	sourcesLock.lock_shared();
	IGNORE_EXCEPTIONS( statistic = &*index.at(key); );
	sourcesLock.unlock_shared();
	return (statistic) ? statistic->getRollingValue( reductionOp, interval ) : StatisticsValue();
}

StatisticsValue ThreadedStatisticsCollector::getReducedStatistics( StatisticsReduceOperator reductionOp, StatisticsInterval interval, const StatisticsDescription & description ) throw() {
	pair<OntologyAttributeID, TopologyObjectId> key(description.ontologyId, description.topologyId);
	Statistic* statistic = NULL;
	sourcesLock.lock_shared();
	IGNORE_EXCEPTIONS( statistic = &*index.at(key); );
	sourcesLock.unlock_shared();
	return (statistic) ? statistic->getReducedValue( reductionOp, interval ) : StatisticsValue();
}

ThreadedStatisticsCollector::~ThreadedStatisticsCollector() throw() {

}

ComponentOptions * ThreadedStatisticsCollector::AvailableOptions() throw() {
	return new ThreadedStatisticsOptions();
}

void ThreadedStatisticsCollector::nanoSecondSleep(Timestamp nanoSeconds) throw() {
	struct timespec sleepTime;
	sleepTime.tv_sec = nanoSeconds/1000/1000/1000;
	sleepTime.tv_nsec = (nanoSeconds - 1000*1000*1000ull * sleepTime.tv_sec);
	nanosleep(&sleepTime, NULL);
}

// One thread for periodic issuing
void ThreadedStatisticsCollector::pollingThreadMain() throw() {
	Timestamp nextPollTime = siox_gettime();
	while( ! terminated ) {
		//perform polling
		sourcesLock.lock_shared();
		//produce new values
		Timestamp measurementTime = siox_gettime();

		// update global statistics
		run_timesteps++;

		for( size_t i = plugins.size(); i--; ) {
			plugins[i]->nextTimestep();
		}
		//bring the histories of the Statistics up to date
		for( size_t i = statistics.size(); i--; ) {
			statistics[i]->update(measurementTime);
		}
		//bring the collections up to date
		for( size_t i = collections.size(); i--; ) {
			collections[i]->pushValues();
		}
		//reset the change flags
		bool hadAdditions = statisticsAdded, hadDeletions = statisticsRemoved;
		statisticsAdded = statisticsRemoved = false;

		//notify consumers of the statistics
		if( hadAdditions || hadDeletions ) smux->notifyAvailableStatisticsChange( statistics, hadAdditions, hadDeletions );
		sourcesLock.unlock_shared();
		smux->newDataAvailable();

		// Sleep until it's time to poll again.
		// I have moved this from its own function, because we have to check `terminated` after every call to sleep_for.
		nextPollTime += 100*1000*1000ull;
		Timestamp curTime = siox_gettime();

		// update global statistics
		process_time += curTime - measurementTime;

		while( nextPollTime >= curTime ) {
			nanoSecondSleep(nextPollTime - curTime);
			curTime = siox_gettime();

			// Check whether we were awoken to terminate.
			atomic_thread_fence( memory_order_acquire );	//Make sure that the value of terminated is up to date.
			if( terminated ) return;
		}
	}
}

extern "C" {
	void * STATISTICS_COLLECTOR_INSTANCIATOR_NAME()
	{
		return new ThreadedStatisticsCollector();
	}
}
