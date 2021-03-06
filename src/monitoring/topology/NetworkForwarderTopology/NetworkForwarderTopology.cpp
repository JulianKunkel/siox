/**
 * This topology forwards it's data to another topolgy over TCP/IP.
 * Blocks all the way (because we need to wait for an answer) and does not cache
 * at all. Please refer to cacheTopology if you need fast response times.
 *
 * @author Roman Michel
 * @date 2014
 */
#include <sstream>

#include <util/ExceptionHandling.hpp>
#include <core/comm/CommunicationModule.hpp>
#include <core/reporting/ComponentReportInterface.hpp>
#include <core/datatypes/VariableDatatypeJBinarySerialization.hpp>
#include <monitoring/topology/TopologyImplementation.hpp>
#include <workarounds.hpp>
#include <core/comm/CommunicationModule.hpp>
#include <core/comm/DefaultCallbacks.hpp>
#include <monitoring/topology/NetworkForwarderTopology/NetworkForwarderCommunicationBinarySerializable.hpp>


#include <boost/algorithm/string.hpp>
#include <boost/thread/shared_mutex.hpp>

#include "NetworkForwarderTopologyOptions.hpp"
#include "NetworkForwarderCommunication.hpp"

using namespace std;
using namespace ::core;
using namespace monitoring;

class MyCallback: public ConnectionCallback{
public:
    void connectionErrorCB(ServiceClient & connection, CommunicationError error){
        printf("Error connecting.");
        fflush(stdout);
        assert(false);
    }

    void connectionSuccessfullCB(ServiceClient & connection){
        // TODO: This never gets called. Maybe we should look into this.
    }
};

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

    connCallback = MyCallback();
    NetworkForwarderTopologyOptions & o = getOptions<NetworkForwarderTopologyOptions>();
    CommunicationModule * comm =  GET_INSTANCE(CommunicationModule, o.comm);

    assert(comm != nullptr);

    client = comm->startClientService(o.targetAddress, & connCallback, this);
}

ComponentOptions* NetworkForwarderTopology::AvailableOptions() {
    return new NetworkForwarderTopologyOptions();
}

TopologyType NetworkForwarderTopology::registerType( const string& name ) throw() {

    NetworkForwarderRequestMessage mess;
    mess.request_type = NETWORK_FORWARDER_TOPOLOGY_TYPE_REGISTER;
    mess.id = 0;
    mess.name = name;

    BlockingRPCMessage container;
    {
		boost::unique_lock<boost::mutex> lock(container.m);
        client->isend(& mess, & container);

        // wait for the response
        container.cv.wait(lock);
    }

    TopologyType returnType;

    if (container.response.id != -1) {
        Release<TopologyTypeImplementation> newType(new TopologyTypeImplementation(name, (size_t) container.response.id));
        returnType.setObject(newType);
    }

    return returnType;
}

TopologyType NetworkForwarderTopology::lookupTypeByName( const string& name ) throw() {

    NetworkForwarderRequestMessage mess;
    mess.request_type = NETWORK_FORWARDER_TOPOLOGY_TYPE_LOOKUP_BY_NAME;
    mess.id = 0;
    mess.name = name;

    BlockingRPCMessage container;
    {
		boost::unique_lock<boost::mutex> lock(container.m);
        client->isend(& mess, & container);

        // wait for the response
        container.cv.wait(lock);
    }

    TopologyType returnType;

    if (container.response.id != -1) {
        Release<TopologyTypeImplementation> newType(new TopologyTypeImplementation(name, (size_t) container.response.id));
        returnType.setObject(newType);
    }

    return returnType;
}

TopologyType NetworkForwarderTopology::lookupTypeById( TopologyTypeId anId ) throw() {

    NetworkForwarderRequestMessage mess;
    mess.request_type = NETWORK_FORWARDER_TOPOLOGY_TYPE_LOOKUP_BY_ID;
    mess.id = anId;
    mess.name = "";

    BlockingRPCMessage container;
    {
		boost::unique_lock<boost::mutex> lock(container.m);
        client->isend(& mess, & container);

        // wait for the response
        container.cv.wait(lock);
    }

    TopologyType returnType;

    if (container.response.name != "-1") {
        Release<TopologyTypeImplementation> newType(new TopologyTypeImplementation( container.response.name, anId));
        returnType.setObject(newType);
    }

    return returnType;
}

TopologyObject NetworkForwarderTopology::registerObject( TopologyObjectId parentId, TopologyTypeId relationType, const string& childName, TopologyTypeId objectType ) throw() {

    NetworkForwarderRequestMessage mess;
    mess.request_type = NETWORK_FORWARDER_TOPOLOGY_OBJECT_REGISTER;
    mess.id = parentId;
    mess.type = relationType;
    mess.name = childName;
    mess.other = objectType;

    BlockingRPCMessage container;
    {
		boost::unique_lock<boost::mutex> lock(container.m);
        client->isend(& mess, & container);

        // wait for the response
        container.cv.wait(lock);
    }

    TopologyObject returnObject;

    if (container.response.id != -1) {
        Release<TopologyObjectImplementation> newObject( new TopologyObjectImplementation(objectType, container.response.id) );
        returnObject.setObject(newObject);
    }

    return returnObject;
}

TopologyObject NetworkForwarderTopology::lookupObjectById( TopologyObjectId anId ) throw() {
    
    NetworkForwarderRequestMessage mess;
    mess.request_type = NETWORK_FORWARDER_TOPOLOGY_OBJECT_LOOKUP;
    mess.id = anId;

    BlockingRPCMessage container;
    {
		boost::unique_lock<boost::mutex> lock(container.m);
        client->isend(& mess, & container);

        // wait for the response
        container.cv.wait(lock);
    }

    TopologyObject returnObject;

    if (container.response.id != -1) {
        Release<TopologyObjectImplementation> newObject( new TopologyObjectImplementation(container.response.type, container.response.id) );
        returnObject.setObject(newObject);
    }

    return returnObject;
}

TopologyRelation NetworkForwarderTopology::registerRelation( TopologyObjectId parent, TopologyTypeId relationType, const string& childName, TopologyObjectId child ) throw() {
    
    NetworkForwarderRequestMessage mess;
    mess.request_type = NETWORK_FORWARDER_TOPOLOGY_RELATION_REGISTER;
    mess.id = parent;
    mess.type = relationType;
    mess.name = childName;
    mess.other = child;

    BlockingRPCMessage container;
    {
		boost::unique_lock<boost::mutex> lock(container.m);
        client->isend(& mess, & container);

        // wait for the response
        container.cv.wait(lock);
    }

    TopologyRelation returnRelation;

    if (container.response.id != -1) {
        Release<TopologyRelationImplementation> newRelation( new TopologyRelationImplementation( childName, parent, child, relationType ) );
        returnRelation.setObject(newRelation);
    }

    return returnRelation;
}

TopologyRelation NetworkForwarderTopology::lookupRelation( TopologyObjectId parent, TopologyTypeId relationType, const string& childName ) throw() {
   
    NetworkForwarderRequestMessage mess;
    mess.request_type = NETWORK_FORWARDER_TOPOLOGY_RELATION_LOOKUP;
    mess.id = parent;
    mess.type = relationType;
    mess.name = childName;

    BlockingRPCMessage container;
    {
		boost::unique_lock<boost::mutex> lock(container.m);
        client->isend(& mess, & container);

        // wait for the response
        container.cv.wait(lock);
    }

    TopologyRelation returnRelation;

    if (container.response.id != -1) {
        Release<TopologyRelationImplementation> newRelation( new TopologyRelationImplementation( childName, parent, container.response.id, relationType ) );
        returnRelation.setObject(newRelation);
    }

    return returnRelation;
}

TopologyRelationList NetworkForwarderTopology::enumerateChildren( TopologyObjectId parent, TopologyTypeId relationType ) throw() {
    assert(false);
    TopologyRelationList retval;
    return retval;
}

TopologyRelationList NetworkForwarderTopology::enumerateParents( TopologyObjectId child, TopologyTypeId relationType ) throw() {
    assert(false);
    TopologyRelationList retval;
    return retval;
}

TopologyAttribute NetworkForwarderTopology::registerAttribute( TopologyTypeId domain, const string& name, VariableDatatype::Type datatype ) throw() {
   
    NetworkForwarderRequestMessage mess;
    mess.request_type = NETWORK_FORWARDER_TOPOLOGY_ATTRIBUTE_REGISTER;
    mess.id = domain;
    mess.name = name;
    mess.other = (intmax_t) datatype;

    BlockingRPCMessage container;
    {
		boost::unique_lock<boost::mutex> lock(container.m);
        client->isend(& mess, & container);

        // wait for the response
        container.cv.wait(lock);
    }

    TopologyAttribute returnAttribute;

    if (container.response.id != -1) {
        Release<TopologyAttributeImplementation> newAttribute( new TopologyAttributeImplementation( name, domain, datatype ) );
        newAttribute->id = container.response.id;
        returnAttribute.setObject(newAttribute);
        fflush(stdout);
    }

    return returnAttribute;
}

TopologyAttribute NetworkForwarderTopology::lookupAttributeByName( TopologyTypeId domain, const string& name ) throw() {
    
    NetworkForwarderRequestMessage mess;
    mess.request_type = NETWORK_FORWARDER_TOPOLOGY_ATTRIBUTE_LOOKUP_BY_NAME;
    mess.id = domain;
    mess.name = name;

    BlockingRPCMessage container;
    {
		boost::unique_lock<boost::mutex> lock(container.m);
        client->isend(& mess, & container);

        // wait for the response
        container.cv.wait(lock);
    }

    TopologyAttribute returnAttribute;

    if (container.response.id != -1) {
        Release<TopologyAttributeImplementation> newAttribute( new TopologyAttributeImplementation( name, domain, (VariableDatatype::Type) container.response.other ) );
        newAttribute->id = container.response.id;
        returnAttribute.setObject(newAttribute);
    }

    return returnAttribute;
}

TopologyAttribute NetworkForwarderTopology::lookupAttributeById( TopologyAttributeId attributeId ) throw() {

    NetworkForwarderRequestMessage mess;
    mess.request_type = NETWORK_FORWARDER_TOPOLOGY_ATTRIBUTE_LOOKUP_BY_ID;
    mess.id = attributeId;

    BlockingRPCMessage container;
    {
		boost::unique_lock<boost::mutex> lock(container.m);
        client->isend(& mess, & container);

        // wait for the response
        container.cv.wait(lock);
    }

    TopologyAttribute returnAttribute;

    if (container.response.id != -1) {
        Release<TopologyAttributeImplementation> newAttribute( new TopologyAttributeImplementation( container.response.name, container.response.id, (VariableDatatype::Type) container.response.other ) );
        newAttribute->id = attributeId;
        returnAttribute.setObject(newAttribute);
    }

    return returnAttribute;
}

bool NetworkForwarderTopology::setAttribute( TopologyObjectId objectId, TopologyAttributeId attributeId, const TopologyVariable& value ) throw() {
    
    NetworkForwarderRequestMessage mess;
    mess.request_type = NETWORK_FORWARDER_TOPOLOGY_ATTRIBUTE_SET;
    mess.id = objectId;
    mess.type = attributeId;
    mess.var = value;

    BlockingRPCMessage container;
    {
		boost::unique_lock<boost::mutex> lock(container.m);
        client->isend(& mess, & container);

        // wait for the response
        container.cv.wait(lock);
    }

    if (container.response.id != -1) {
        return true;

    }

    return false;
}

TopologyValue NetworkForwarderTopology::getAttribute( TopologyObjectId object, TopologyAttributeId attribute ) throw() {

    NetworkForwarderRequestMessage mess;
    mess.request_type = NETWORK_FORWARDER_TOPOLOGY_ATTRIBUTE_GET;
    mess.id = object;
    mess.type = attribute;

    BlockingRPCMessage container;
    {
		boost::unique_lock<boost::mutex> lock(container.m);
        client->isend(& mess, & container);

        // wait for the response
        container.cv.wait(lock);
    }

    TopologyValue retValue;

    if (container.response.id == 1) {

        Release<TopologyValueImplementation> newValue( new TopologyValueImplementation( container.response.var, object, attribute ) );
        retValue.setObject(newValue);
    }
    
    return retValue;
}

TopologyValueList NetworkForwarderTopology::enumerateAttributes( TopologyObjectId object ) throw() {
    assert(false);
    TopologyValueList retval;
    return retval;
}

void NetworkForwarderTopology::messageSendCB(BareMessage * msg) throw() { /* do nothing */ }

void NetworkForwarderTopology::messageResponseCB(BareMessage * msg, char * buffer, uint64_t buffer_size) throw() {
    // Deserialize and unlock container
    BlockingRPCMessage * container = (BlockingRPCMessage *) msg->user_ptr;

    uint64_t pos = 0;
    j_serialization::deserialize(container->response, buffer, pos, buffer_size);

	boost::unique_lock<boost::mutex> lock(container->m);
    container->cv.notify_one();
}

void NetworkForwarderTopology::messageTransferErrorCB(BareMessage * msg, CommunicationError error) throw() {
    // Should happen only if the target server is not offering this RPC service!
    // This should be really critical and kill the caller.
    assert(false);
}

uint64_t NetworkForwarderTopology::serializeMessageLen(const void * msgObject) throw() {
    return j_serialization::serializeLen(* (NetworkForwarderRequestMessage*)  msgObject);
}

void NetworkForwarderTopology::serializeMessage(const void * msgObject, char * buffer, uint64_t & pos) throw() {
    j_serialization::serialize(* (NetworkForwarderRequestMessage*) msgObject, buffer, pos);
}

extern "C" {
    void* TOPOLOGY_INSTANCIATOR_NAME()
    {
        return new NetworkForwarderTopology();
    }
}
