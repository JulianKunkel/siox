#ifndef STATISTICS_PROVIDER_PLUGIN_OPTIONS
#define STATISTICS_PROVIDER_PLUGIN_OPTIONS

/*!
 * Here we also integrate Userderived statistics such as GBytes/s from the bytes/ms Throughput we already have.
 * R5	The options of the ThreadCollector contain the list of statistics to query, so the user can set it as options.
		The de/serialization is done using Boost.
 * D5 Done here
 */

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
