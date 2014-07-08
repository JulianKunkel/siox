#ifndef INCLUDE_GUARD_NETWORK_FORWARDER_SERVER_TOPOLOGY_COMM_HPP
#define INCLUDE_GUARD_NETWORK_FORWARDER_SERVER_TOPOLOGY_COMM_HPP
#include <monitoring/topology/TopologyImplementation.hpp>

enum NetworkForwarderMessageType {

    NETWORK_FORWARDER_TOPOLOGY_TYPE_REGISTER,
    NETWORK_FORWARDER_TOPOLOGY_TYPE_LOOKUP_BY_NAME,
    NETWORK_FORWARDER_TOPOLOGY_TYPE_LOOKUP_BY_ID,
};

using namespace std;
using namespace core;
using namespace monitoring;
using namespace boost;

//@serializable
struct TopologyTypeRequest {
    TopologyTypeId id;
    string name;
};

struct NetworkForwarderRequestMessage {

    NetworkForwarderMessageType type;  
    void * data;
};

struct NetworkForwarderResponseMessage {

    void * data;
    uint64_t length = 0;
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