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
};

using namespace std;
using namespace core;
using namespace monitoring;
using namespace boost;

//@serializable
struct NetworkForwarderRequestMessage {

    NetworkForwarderMessageType request_type;  
    int32_t id;
    int32_t type;
    int32_t other;
    string name;
    //TODO: TopologyValue value;
};

//@serializable
struct NetworkForwarderResponseMessage {

    int32_t id;
    int32_t type;
    int32_t other;
    string name;
    //TODO: TopologyValue value;
};

struct BlockingRPCMessage {

    NetworkForwarderResponseMessage response;
    mutex m;
    condition_variable cv;
};

#endif
/*oid func(x, y){
    baue struct TopologyTypeRequest => auf Stack
    baue NetworkForwarderRequestMessage => auf Stack, verlinke diese Objekte über *data
    isend()
    response = wait()
}

void serverMessageReceivedCB(){
    decode message()
    switch (type){
        call right function(); <= blockierend
        send response.
    }
}

decode message(){
    decode NetworkForwarderRequestMessage manually!
    switch type
    case(TOPOLOGY_TYPE):
        deserialize(... (TopologyTypeRequest*) buffer, an aktueller stelle);
}*/