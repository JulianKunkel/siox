#include <monitoring/activity_multiplexer/ActivityPluginDereferencing.hpp>

#include <monitoring/statistics_collector/StatisticsCollectorImplementation.hpp>


using namespace std;
using namespace core;
using namespace monitoring;

class ThreadedStatisticsCollector: StatisticsCollector{
private:
	ActivityPluginDereferencing * facade;

public:

	virtual void init(ActivityPluginDereferencing * facade){
		this->facade = facade;
	}

	virtual void init(ComponentOptions * options){
		StatisticsCollectorOptions * o = (StatisticsCollectorOptions*) options;

		// init facade etc.

		ActivityPluginDereferencing * facade = o->dereferingFacade.instance<ActivityPluginDereferencing>();
		init(facade);
	}

	virtual ComponentOptions * get_options(){
		return new ThreadedStatisticsOptions();
	}

	virtual void shutdown(){

	}
};

CREATE_SERIALIZEABLE_CLS(StatisticsCollectorOptions)

COMPONENT(ThreadedStatisticsCollector)
