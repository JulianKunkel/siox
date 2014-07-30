#ifndef INCLUDE_GUARD_NETWORK_FORWARDER_SERVER_TOPOLOGY_OPTIONS_HPP
#define INCLUDE_GUARD_NETWORK_FORWARDER_SERVER_TOPOLOGY_OPTIONS_HPP

#include <core/component/component-options.hpp>
#include <core/component/ComponentReference.hpp>

using namespace std;
using namespace core;

namespace monitoring {
    //@serializable
    class NetworkForwarderTopologyServerOptions : public ComponentOptions {
        public:
            string commAddress;
            ComponentReference comm;
            ComponentReference topologyBackend;
    };
}

#endif