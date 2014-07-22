#include "NetworkForwarderCommunication.hpp"
#include "NetworkForwarderTopologyServerOptions.hpp"
#include <monitoring/topology/NetworkForwarderTopology/NetworkForwarderCommunicationBinarySerializable.hpp>

#include <core/comm/CommunicationModule.hpp>
#include <util/JobProcessors/DefaultProcessorQueues.cpp>
#include <string>
#include <core/comm/NetworkService.hpp>

using namespace std;

class NetworkForwarderTopologyServer : public NetworkService, ServerCallback {

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

        printf("1.5");
        fflush(stdout);

        NetworkForwarderRequestMessage reqMess;
        NetworkForwarderResponseMessage resMess;
        TopologyType type;
        uint64_t pos = 0;

        j_serialization::deserialize(reqMess, message_data, pos, buffer_size);

        switch(reqMess.request_type) {
            case NETWORK_FORWARDER_TOPOLOGY_TYPE_REGISTER: 
                type = topologyBackend->registerType(reqMess.name);
                resMess.id = (size_t) type.id();
                msg->isendResponse(&resMess);
                break;
            case NETWORK_FORWARDER_TOPOLOGY_TYPE_LOOKUP_BY_NAME: 
                type = topologyBackend->lookupTypeByName(reqMess.name);
                resMess.id = (size_t) type.id();
                msg->isendResponse(&resMess);
                break;
            case NETWORK_FORWARDER_TOPOLOGY_TYPE_LOOKUP_BY_ID: 
                type = topologyBackend->lookupTypeById(reqMess.id);
                resMess.name = type.name();
                msg->isendResponse(&resMess);
                break;
        }
    }

    virtual void invalidMessageReceivedCB(CommunicationError error) {
        printf("Error");
        fflush(stdout);
        assert(false);
    }

    virtual uint64_t serializeResponseMessageLen(const ServerClientMessage * msg, const void * responseType) {
        return j_serialization::serializeLen(* (NetworkForwarderResponseMessage*) responseType);
    }

    virtual void serializeResponseMessage(const ServerClientMessage * msg, const void * responseType, char * buffer, uint64_t & pos) {
        j_serialization::serialize(* (NetworkForwarderResponseMessage*) responseType, buffer, pos);
    }

    ~NetworkForwarderTopologyServer(){
        delete(server);
    }

private:
    ServiceServer * server;

    Topology* topologyBackend;
};


extern "C" {
    void * NETWORK_SERVICE_INSTANCIATOR_NAME()
    {
        return new NetworkForwarderTopologyServer();
    }
}
