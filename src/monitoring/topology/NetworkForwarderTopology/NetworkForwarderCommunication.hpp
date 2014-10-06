#ifndef INCLUDE_GUARD_NETWORK_FORWARDER_SERVER_TOPOLOGY_COMM_HPP
#define INCLUDE_GUARD_NETWORK_FORWARDER_SERVER_TOPOLOGY_COMM_HPP

#include <monitoring/topology/TopologyImplementation.hpp>

//@serializable
enum NetworkForwarderMessageType {

    NETWORK_FORWARDER_TOPOLOGY_TYPE_REGISTER,
    NETWORK_FORWARDER_TOPOLOGY_TYPE_LOOKUP_BY_NAME,
    NETWORK_FORWARDER_TOPOLOGY_TYPE_LOOKUP_BY_ID,

    NETWORK_FORWARDER_TOPOLOGY_OBJECT_REGISTER,
    NETWORK_FORWARDER_TOPOLOGY_OBJECT_LOOKUP,

    NETWORK_FORWARDER_TOPOLOGY_RELATION_REGISTER,
    NETWORK_FORWARDER_TOPOLOGY_RELATION_LOOKUP,

    NETWORK_FORWARDER_TOPOLOGY_ATTRIBUTE_REGISTER,
    NETWORK_FORWARDER_TOPOLOGY_ATTRIBUTE_LOOKUP_BY_NAME,
    NETWORK_FORWARDER_TOPOLOGY_ATTRIBUTE_LOOKUP_BY_ID, 
    NETWORK_FORWARDER_TOPOLOGY_ATTRIBUTE_SET,
    NETWORK_FORWARDER_TOPOLOGY_ATTRIBUTE_GET,
};

using namespace std;
using namespace ::core;
using namespace monitoring;
using namespace boost;

//@serializable
struct NetworkForwarderRequestMessage {

    NetworkForwarderMessageType request_type;  
    int32_t id;
    int32_t type;
    int32_t other;
    string name = "";
    TopologyVariable var;
};

//@serializable
struct NetworkForwarderResponseMessage {

    int32_t id;
    int32_t type;
    int32_t other;
    string name = "";
    TopologyVariable var;
};

struct BlockingRPCMessage {

    NetworkForwarderResponseMessage response;
    mutex m;
    condition_variable cv;
};

#endif