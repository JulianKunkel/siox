#ifndef STATISTICS_PROVIDER_PLUGIN_OPTIONS
#define STATISTICS_PROVIDER_PLUGIN_OPTIONS

#include <list>

#include <core/component/component-macros.hpp>


using namespace core;
using namespace std;

namespace monitoring{

class ThreadedCollectorStatistics: Container{
public:
	string entity;
	string scope;

	string metrics;

	string topology;
	string topology_instance;

	uint32_t poll_interval_ms;

	SERIALIZE_CONTAINER(MEMBER(scope) MEMBER(entity) MEMBER(metrics) MEMBER(topology) MEMBER(topology_instance) MEMBER(poll_interval_ms))
};


class ThreadedStatisticsOptions: public ComponentOptions{
public:
	ComponentReference facade;
	ComponentReference smux;

	list<ThreadedCollectorStatistics> statistics;

	SERIALIZE_CONTAINER(MEMBER(smux) MEMBER(facade) MEMBER(statistics))
};

}


#endif
