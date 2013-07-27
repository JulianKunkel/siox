#include <monitoring/activity_multiplexer/ActivityPluginDereferencing.hpp>

#include <monitoring/statistics_collector/StatisticsCollectorImplementation.hpp>


using namespace std;
using namespace core;
using namespace monitoring;

/*
 * Single threaded, compute next waittime based on options.
 */
class ThreadedStatisticsCollector: StatisticsCollector{
private:
	// ActivityPluginDereferencing * facade;
	// Statistics Multiplexer

public:

	virtual void registerPlugin(StatisticsProviderPlugin * plugin){

	}

	virtual void unregisterPlugin(StatisticsProviderPlugin * plugin){

	}

	virtual array<StatisticsValue,10> getStatistics(StatisticsIntervall intervall, StatisticsDescription & stat){

	}

	virtual StatisticsValue getStatistics(StatisticsIntervall intervall, StatisticsDescription & stat, StatisticsReduceOperator op){

	}

	virtual list<StatisticsDescription> availableMetrics(){

	}


	virtual void init(ActivityPluginDereferencing * facade){
		this->facade = facade;
	}

	virtual void init(ThreadedStatisticsOptions * options){
		ThreadedStatisticsOptions * o = (ThreadedStatisticsOptions*) options;

		// init facade etc.

		ActivityPluginDereferencing * facade = o->dereferingFacade.instance<ActivityPluginDereferencing>();
		init(facade);
	}

	virtual ComponentOptions * AvailableOptions(){
		return new ThreadedStatisticsOptions();
	}

	virtual void shutdown(){

	}
};

CREATE_SERIALIZEABLE_CLS(ThreadedStatisticsOptions)
CREATE_SERIALIZEABLE_CLS(ThreadedCollectorStatistics)

COMPONENT(ThreadedStatisticsCollector)
