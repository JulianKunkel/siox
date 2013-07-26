#ifndef STATISTICS_PROVIDER_PLUGIN_OPTIONS
#define STATISTICS_PROVIDER_PLUGIN_OPTIONS

#include <list>

#include <core/component/component-macros.hpp>


using namespace core;
using namespace std;

namespace monitoring{

class StatisticOptions: Container{
public:
	StatisticsEntity entity;
	StatisticsScope  scope;

	string metrics;

	string topology;
	string topology_instance;

	uint32_t poll_interval_ms;

	SERIALIZE_CONTAINER(MEMBER(poll_interval_ms) ... )
};


class ThreadedStatisticsOptions: public ComponentOptions{
public:
	ComponentReference facade;
	list<StatisticOptions> statistics;

	// put all options here ...

	SERIALIZE_CONTAINER(MEMBER(facade) MEMBER(statistics))
};

}


#endif
