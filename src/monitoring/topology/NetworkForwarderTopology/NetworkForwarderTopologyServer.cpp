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
        topologyBackend = GET_INSTANCE(Topology, o.topologyBackend);

        assert(topologyBackend != nullptr);
    }

    virtual void messageReceivedCB(std::shared_ptr<ServerClientMessage> msg, const char * message_data, uint64_t buffer_size) {

        NetworkForwarderRequestMessage reqMess;
        NetworkForwarderResponseMessage resMess;
        TopologyType type;
        TopologyObject object;
        TopologyRelation relation;
        uint64_t pos = 0;

        j_serialization::deserialize(reqMess, message_data, pos, buffer_size);

        switch(reqMess.request_type) {
            case NETWORK_FORWARDER_TOPOLOGY_TYPE_REGISTER: 
                type = topologyBackend->registerType(reqMess.name);

                if(type) {
                    resMess.id = (size_t) type.id();
                }
                else {
                    resMess.id = -1;
                }

                msg->isendResponse(&resMess);
                break;
            case NETWORK_FORWARDER_TOPOLOGY_TYPE_LOOKUP_BY_NAME: 
                type = topologyBackend->lookupTypeByName(reqMess.name);

                if(type) {
                    resMess.id = (size_t) type.id();
                }
                else {
                    resMess.id = -1;
                }

                msg->isendResponse(&resMess);
                break;
            case NETWORK_FORWARDER_TOPOLOGY_TYPE_LOOKUP_BY_ID: 
                type = topologyBackend->lookupTypeById(reqMess.id);

                if(type) {
                    resMess.name = type.name();
                }
                else {
                    //TODO: -1 als reserved keyword okay?
                    resMess.name = "-1";
                }

                msg->isendResponse(&resMess);
                break;

            case NETWORK_FORWARDER_TOPOLOGY_OBJECT_REGISTER:
                object = topologyBackend->registerObject(reqMess.id, reqMess.type, reqMess.name, reqMess.other);

                if(object) {
                    resMess.id = object.id();
                }
                else {
                    resMess.id = -1;
                }

                msg->isendResponse(&resMess);
                break;
            case NETWORK_FORWARDER_TOPOLOGY_OBJECT_LOOKUP:
                object = topologyBackend->lookupObjectById(reqMess.id);

                if(object) {
                    resMess.id = object.id();
                    resMess.type = object.type();
                }
                else {
                    resMess.id = -1;
                }

                msg->isendResponse(&resMess);
                break;

            case NETWORK_FORWARDER_TOPOLOGY_RELATION_REGISTER:
                relation = topologyBackend->registerRelation(reqMess.id, reqMess.type, reqMess.name, reqMess.other);

                if(relation) {
                    // Wont be read, but we'll send a success signal anyway
                    resMess.id = 0;
                }
                else {
                    resMess.id = -1;
                }

                msg->isendResponse(&resMess);
                break;       
            case NETWORK_FORWARDER_TOPOLOGY_RELATION_LOOKUP:
                relation = topologyBackend->lookupRelation(reqMess.id, reqMess.type, reqMess.name);

                if(relation) {
                    resMess.id = relation.child();
                }
                else {
                    resMess.id = -1;
                }

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
