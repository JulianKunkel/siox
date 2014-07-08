#ifndef INCLUDE_GUARD_NETWORK_FORWARDER_TOPOLOGY_OPTIONS_HPP
#define INCLUDE_GUARD_NETWORK_FORWARDER_TOPOLOGY_OPTIONS_HPP

#include <core/component/component-options.hpp>
#include <core/component/ComponentReference.hpp>

using namespace std;
using namespace core;

namespace monitoring {
	//@serializable
	class NetworkForwarderTopologyOptions : public ComponentOptions {
		public:
			ComponentReference comm;
            string targetAddress;
	};
}

#endif