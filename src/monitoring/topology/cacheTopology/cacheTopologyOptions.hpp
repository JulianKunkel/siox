#ifndef INCLUDE_GUARD_DATABASE_TOPOLOGY_OPTIONS_HPP
#define INCLUDE_GUARD_DATABASE_TOPOLOGY_OPTIONS_HPP

#include <core/component/component-options.hpp>
#include <core/component/ComponentReference.hpp>

using namespace std;
using namespace core;

namespace monitoring {
	//@serializable
	class CacheTopologyOptions : public ComponentOptions {
		public:
			ComponentReference topologyBackend;
	};
}

#endif