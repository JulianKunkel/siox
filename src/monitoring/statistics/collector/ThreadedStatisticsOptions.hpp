#ifndef THREADED_STATISTICS_COLLECTOR_OPTIONS
#define THREADED_STATISTICS_COLLECTOR_OPTIONS

/*!
 * Here we also integrate Userderived statistics such as GBytes/s from the bytes/ms Throughput we already have.
 * R5   The options of the ThreadCollector contain the list of statistics to query, so the user can set it as options.
        The de/serialization is done using Boost.
 * D5 Done here
 */

#include <list>

#include <core/component/ComponentReference.hpp>
#include <core/component/component-options.hpp>


namespace monitoring {

	//@serializable
	class ThreadedStatisticsOptions: public core::ComponentOptions {
		public:
			core::ComponentReference ontology;
			core::ComponentReference smux;
	};

}


#endif
