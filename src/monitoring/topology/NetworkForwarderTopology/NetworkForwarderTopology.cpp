/**
 * Topologoy forwards it's data to another instance
 *
 * @author Roman Michel
 * @date 2014
 */
#include <sstream>

#include <util/ExceptionHandling.hpp>
#include <core/comm/CommunicationModule.hpp>
#include <core/reporting/ComponentReportInterface.hpp>
#include <monitoring/topology/TopologyImplementation.hpp>
#include <workarounds.hpp>
#include <core/comm/CommunicationModule.hpp>
#include <core/comm/DefaultCallbacks.hpp>


#include <boost/algorithm/string.hpp>
#include <boost/thread/shared_mutex.hpp>

#include "NetworkForwarderTopologyOptions.hpp"
#include "NetworkForwarderCommunication.hpp"

using namespace std;
using namespace core;
using namespace monitoring;
using namespace boost;

// TODO: MessageCallback implementieren
class NetworkForwarderTopology : public Topology, MessageCallback {
    public:
        virtual void init();
        virtual ComponentOptions * AvailableOptions();

        virtual TopologyType registerType( const string& name ) throw(); // id
        virtual TopologyType lookupTypeByName( const string& name ) throw(); // id 
        virtual TopologyType lookupTypeById( TopologyTypeId anId ) throw(); // string
        virtual TopologyObject registerObject( TopologyObjectId parent, TopologyTypeId relationType, const string& childName, TopologyTypeId objectType ) throw(); // id
        virtual TopologyObject lookupObjectById( TopologyObjectId anId ) throw(); // struct

        virtual TopologyRelation registerRelation( TopologyObjectId parent, TopologyTypeId relationType, const string& childName, TopologyObjectId child ) throw(); // id
        virtual TopologyRelation lookupRelation( TopologyObjectId parent, TopologyTypeId relationType, const string& childName ) throw(); // child ID


        virtual TopologyRelationList enumerateChildren( TopologyObjectId parent, TopologyTypeId relationType ) throw(); // nix 
        virtual TopologyRelationList enumerateParents( TopologyObjectId child, TopologyTypeId relationType ) throw(); // nix


        virtual TopologyAttribute registerAttribute( TopologyTypeId domain, const string& name, VariableDatatype::Type datatype ) throw(); // id
        virtual TopologyAttribute lookupAttributeByName( TopologyTypeId domain, const string& name ) throw(); // struct
        virtual TopologyAttribute lookupAttributeById( TopologyAttributeId attributeId ) throw(); // struct
        virtual bool setAttribute( TopologyObjectId object, TopologyAttributeId attribute, const TopologyVariable& value ) throw(); // bool
        virtual TopologyValue getAttribute( TopologyObjectId object, TopologyAttributeId attribute ) throw(); // Value (gibt serializer
        virtual TopologyValueList enumerateAttributes( TopologyObjectId object ) throw(); // nix

        virtual void messageSendCB(BareMessage * msg) throw();
        virtual void messageResponseCB(BareMessage * msg, char * buffer, uint64_t buffer_size) throw();
        virtual void messageTransferErrorCB(BareMessage * msg, CommunicationError error) throw();
        virtual uint64_t serializeMessageLen(const void * msgObject) throw();
        virtual void serializeMessage(const void * msgObject, char * buffer, uint64_t & pos) throw();

        // this module is mandatory! => it will only be stopped for fork()
        bool isMandatoryModule() override{ 
                return true;
        }

        void start() override;
        void stop() override;

    private:
        ServiceClient * client;
        ConnectionCallback connCallback;
};

void NetworkForwarderTopology::start(){
    Topology::start();
    
}

void NetworkForwarderTopology::stop(){

    Topology::stop();
}


void NetworkForwarderTopology::init() {
    NetworkForwarderTopologyOptions & o = getOptions<NetworkForwarderTopologyOptions>();
    CommunicationModule * comm =  GET_INSTANCE(CommunicationModule, o.comm);
    client = comm->startClientService(o.targetAddress, & connCallback, this);
    connCallback = DefaultReConnectionCallback();
}

ComponentOptions* NetworkForwarderTopology::AvailableOptions() {
    return new NetworkForwarderTopologyOptions();
}

TopologyType NetworkForwarderTopology::registerType( const string& name ) throw() {

    TopologyTypeRequest req;
    req.id = 0;
    req.name = name;

    NetworkForwarderRequestMessage mess;
    mess.type = NETWORK_FORWARDER_TOPOLOGY_TYPE_REGISTER;
    mess.data = &req;

    BlockingRPCMessage container;
    {
        unique_lock<mutex> lock(container.m);
        client->isend(& mess, & container);

        // wait for the response
        container.cv.wait(lock);
    }

    TopologyType returnType;
    Release<TopologyTypeImplementation> newType(new TopologyTypeImplementation(name, (size_t) container.response.data));
    returnType.setObject(newType);

    return returnType;
}

TopologyType NetworkForwarderTopology::lookupTypeByName( const string& name ) throw() {

    TopologyTypeRequest req;
    req.id = 0;
    req.name = name;

    NetworkForwarderRequestMessage mess;
    mess.type = NETWORK_FORWARDER_TOPOLOGY_TYPE_LOOKUP_BY_NAME;
    mess.data = &req;

    BlockingRPCMessage container;
    {
        unique_lock<mutex> lock(container.m);
        client->isend(& mess, & container);

        // wait for the response
        container.cv.wait(lock);
    }

    TopologyType returnType;
    Release<TopologyTypeImplementation> newType(new TopologyTypeImplementation(name, (size_t) container.response.data));
    returnType.setObject(newType);

    return returnType;
}

TopologyType NetworkForwarderTopology::lookupTypeById( TopologyTypeId anId ) throw() {

    TopologyTypeRequest req;
    req.id = anId;
    req.name = "";

    NetworkForwarderRequestMessage mess;
    mess.type = NETWORK_FORWARDER_TOPOLOGY_TYPE_LOOKUP_BY_ID;
    mess.data = &req;

    BlockingRPCMessage container;
    {
        unique_lock<mutex> lock(container.m);
        client->isend(& mess, & container);

        // wait for the response
        container.cv.wait(lock);
    }

    TopologyType returnType;
    string retName;
    uint64_t pos = 0;
    j_serialization::deserialize(retName, (char*) container.response.data, pos, container.response.length);
    Release<TopologyTypeImplementation> newType(new TopologyTypeImplementation( retName, anId));
    returnType.setObject(newType);

    return returnType;
}

TopologyObject NetworkForwarderTopology::registerObject( TopologyObjectId parentId, TopologyTypeId relationType, const string& childName, TopologyTypeId objectType ) throw() {
    assert(false);
}

TopologyObject NetworkForwarderTopology::lookupObjectById( TopologyObjectId anId ) throw() {
    assert(false);
}

TopologyRelation NetworkForwarderTopology::registerRelation( TopologyObjectId parent, TopologyTypeId relationType, const string& childName, TopologyObjectId child ) throw() {
    assert(false);
}

TopologyRelation NetworkForwarderTopology::lookupRelation( TopologyObjectId parent, TopologyTypeId relationType, const string& childName ) throw() {
    assert(false);
}

TopologyRelationList NetworkForwarderTopology::enumerateChildren( TopologyObjectId parent, TopologyTypeId relationType ) throw() {
    assert(false);
}

TopologyRelationList NetworkForwarderTopology::enumerateParents( TopologyObjectId child, TopologyTypeId relationType ) throw() {
    assert(false);
}

TopologyAttribute NetworkForwarderTopology::registerAttribute( TopologyTypeId domain, const string& name, VariableDatatype::Type datatype ) throw() {
    assert(false);
}

TopologyAttribute NetworkForwarderTopology::lookupAttributeByName( TopologyTypeId domain, const string& name ) throw() {
    assert(false);
}

TopologyAttribute NetworkForwarderTopology::lookupAttributeById( TopologyAttributeId attributeId ) throw() {
    assert(false);
}

bool NetworkForwarderTopology::setAttribute( TopologyObjectId objectId, TopologyAttributeId attributeId, const TopologyVariable& value ) throw() {
    assert(false);
}

TopologyValue NetworkForwarderTopology::getAttribute( TopologyObjectId object, TopologyAttributeId attribute ) throw() {
    assert(false);
}

TopologyValueList NetworkForwarderTopology::enumerateAttributes( TopologyObjectId object ) throw() {
    assert(false);
}

void NetworkForwarderTopology::messageSendCB(BareMessage * msg) throw() { /* do nothing */ }

void NetworkForwarderTopology::messageResponseCB(BareMessage * msg, char * buffer, uint64_t buffer_size) throw() {
    // TODO
    assert(false);
}

void NetworkForwarderTopology::messageTransferErrorCB(BareMessage * msg, CommunicationError error) throw() {
    // Should happen only if the target server is not offering this RPC service!
    // This should be really critical and kill the caller.
    assert(false);
}

uint64_t NetworkForwarderTopology::serializeMessageLen(const void * msgObject) throw() {
    // TODO
    assert(false);
}

void NetworkForwarderTopology::serializeMessage(const void * msgObject, char * buffer, uint64_t & pos) throw() {
    // TODO
    assert(false);
}

extern "C" {
    void* TOPOLOGY_INSTANCIATOR_NAME()
    {
        return new NetworkForwarderTopology();
    }
}