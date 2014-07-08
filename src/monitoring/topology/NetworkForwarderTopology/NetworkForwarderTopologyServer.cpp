#include "NetworkForwarderCommunication.hpp"
#include "NetworkForwarderTopologyServerOptions.hpp"
#include <monitoring/topology/NetworkForwarderTopology/NetworkForwarderCommunicationBinarySerializable.hpp>

#include <core/comm/CommunicationModule.hpp>
#include <util/JobProcessors/DefaultProcessorQueues.cpp>
#include <string>

using namespace std;

class NetworkForwarderTopologyServer : public core::Component, ServerCallback {

    virtual ComponentOptions* AvailableOptions(){
        return new NetworkForwarderTopologyServerOptions();
    }

    virtual void init(){
        NetworkForwarderTopologyServerOptions & o = getOptions<NetworkForwarderTopologyServerOptions>();
        CommunicationModule * comm =  GET_INSTANCE(CommunicationModule, o.comm);

        assert(comm != nullptr);

        server = comm->startServerService(o.commAddress, this);
    }

    virtual void messageReceivedCB(std::shared_ptr<ServerClientMessage> msg, const char * message_data, uint64_t buffer_size) {

        NetworkForwarderResponseMessage response;
        NetworkForwarderMessageType msgType;
        TopologyTypeRequest typeRequest;
        TopologyType type;

        uint64_t pos = 0;

        uint8_t msgTypeInt;
        j_serialization::deserialize(msgTypeInt, message_data, pos, buffer_size);
        // TODO: How to get TopologyTypeRequest to serialize properly?
        //j_serialization::deserialize(typeRequest, message_data, pos, buffer_size);

        msgType = (NetworkForwarderMessageType) msgTypeInt;

        size_t tmpi;
        string tmps;

        switch(msgType) {
            case NETWORK_FORWARDER_TOPOLOGY_TYPE_REGISTER: 
                type = topologyBackend->registerType(typeRequest.name);
                tmpi = (size_t) type.id();
                response.data = &tmpi;
                response.length = sizeof(type.id());
                msg->isendResponse(&response);
                break;
            case NETWORK_FORWARDER_TOPOLOGY_TYPE_LOOKUP_BY_NAME: 
                type = topologyBackend->lookupTypeByName(typeRequest.name);
                tmpi = (size_t) type.id();
                response.data = &tmpi;
                response.length = sizeof(type.id());
                msg->isendResponse(&response);
                break;
            case NETWORK_FORWARDER_TOPOLOGY_TYPE_LOOKUP_BY_ID: 
                type = topologyBackend->lookupTypeById(typeRequest.id);
                tmps = type.name();
                // TODO: String casten?
                response.data = &tmps;
                response.length = tmps.length();
                msg->isendResponse(&response);
                break;
        }
    }

    virtual uint64_t serializeResponseMessageLen(const ServerClientMessage * msg, const void * responseType) {
        // TODO
        //return j_serialization::serializeLen(* (DataFromRPC2*) responseType);
        return 0;
    }

    virtual void serializeResponseMessage(const ServerClientMessage * msg, const void * responseType, char * buffer, uint64_t & pos) {
        // TODO
        //j_serialization::serialize(* (DataFromRPC2*) responseType, buffer, pos);
    }

    ~NetworkForwarderTopologyServer(){
        delete(server);
    }

private:
    ServiceServer * server;

    Topology* topologyBackend;
};


extern "C" {
    void * RPCSAMPLE_SERVER_INSTANCIATOR_NAME()
    {
        return new NetworkForwarderTopologyServer();
    }
}
