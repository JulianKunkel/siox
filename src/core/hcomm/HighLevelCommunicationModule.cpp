/*
 This class multiplexes any address space of an existing communication module.
 This is done by:
 	1) Injecting an additional channel type into the message 
 	2) Intercepting all calls and forwarding them.
 The existing address is altered by appending #<CHANNEL_NAME>

 Thus if different service endpoints are created on:
	Name1:8080#service1
 	Name1:8080#service2

 	Name2:8080#service1
 Then the first two service endpoints can be reached by a shared connection.

 Multiple alternative endpoints are in the form:
 	Name1:8080,Name2:8080#service1
 In this case both endpoints are alternatives (both support the #service channel @ port 8080), if one becomes unreachable the other one will be used for the connection.
 */

#include <mutex>
#include <utility>
#include <unordered_map>
#include <list>
#include <vector>


#include <core/comm/CommunicationModuleImplementation.hpp>

#include <core/comm/ServiceServer.hpp>
#include <core/comm/ServiceClient.hpp>

#include <core/hcomm/HighLevelCommunicationServer.hpp>
#include <core/container/container-binary-serializer.hpp>

using namespace core;
using namespace std;

static inline uint32_t headerLen(){
	const uint16_t type = 0;
	return j_serialization::serializeLen(type) * 2;
}

/*
 Wire protocol:
 	ChannelID uint16_t
	messageTypeID uint16_t

 The handler does not need to manage overflows as all flow control is done on the client side.
 */

class HLCommunicationModuleServerHandler : public ServerCallback{
public:
	virtual void messageReceivedCB(std::shared_ptr<ServerClientMessage> msg, const char * message_data, uint64_t buffer_size)
	{
		uint16_t mtype;
		uint16_t channelID;
		uint64_t pos = parentServer->headerSizeClientMessage();

		j_serialization::deserialize(channelID, msg->payload, pos, msg->size);
		j_serialization::deserialize(mtype, msg->payload, pos, msg->size);

		if ( channelID < channels.size() && channels[channelID] != nullptr ){
			// seems to be a valid channel specifier
		}
	}

	virtual void responseSendCB(BareMessage * response){	};

	virtual void invalidMessageReceivedCB(CommunicationError error){  };

	virtual uint64_t serializeResponseMessageLen(const ServerClientMessage * msg, const void * responseType)
	{

	}

	virtual void serializeResponseMessage(const ServerClientMessage * msg, const void * responseType, char * buffer, uint64_t & pos){

	}

	unordered_map<string, uint16_t> channelRegistry;
	vector<HighLevelServiceServer*> channels;
	ServiceServer * parentServer;
};


class HighLevelServiceClient : public HLCommServiceClient{
public:
	HighLevelServiceClient(HLCommunicationModuleImpl & parent, ServiceClient * client, const vector<string> & addressList, const string & channelName) : parent(parent), serverAddresses(addressList), channelName(channelName), client(client){

	}

	~HighLevelServiceClient(){
		parent.hasStopped(this);
	}

private:
	HLCommunicationModuleImpl & parent;
	const vector<string> serverAddresses;
	const string channelName;

	ServiceClient * client;
};

class HighLevelServiceServer : public HLCommServiceServer{
public:	
	HighLevelServiceServer(HLCommunicationModuleImpl & parent, ServiceServer * server) : parent(parent), server(server){

	}

	~HighLevelServiceServer(){
		parent.hasStopped(this);
	}

private:
	map<uint32_t, HLServerCallbackUnidirectional*> callbacks;

	HLCommunicationModuleImpl & parent;
	ServiceServer * server;
};

static pair<string, string> splitAddress(const string & address) throw(CommunicationModuleException){
	size_t pos = address.rfind ("#");
	if (pos == string::npos ){
		throw CommunicationModuleException( string("The address is invalid ") + address, CommunicationError::INVALID_ADDRESS);
	}
	return { address.substr(0, pos), address.substr(pos+1)};	
}


HLCommServiceServer * HLCommunicationModuleImpl::startServerService(const string & address) throw(CommunicationModuleException){

	pair<string, string> addressPair = splitAddress(address);

	unique_lock<mutex> lk(parentListMutex);

	auto itr = createdServers.find( addressPair.first );
	ServiceServer * parentServer;
	if( itr == createdServers.end() ){
		HighLevelCommunicationModuleOptions & o = getOptions<HighLevelCommunicationModuleOptions>();		
		CommunicationModule * parentComm =  GET_INSTANCE(CommunicationModule, o.parentComm);
		assert(parentComm);
		// create a new server to listen
		parentServer = parentComm->startServerService(addressPair.first, ServerCallback * msg_rcvd_callback);

		// add the server to the existing servers
		pair<ServiceServer*, list<HighLevelServiceServer*> > mypair;
		mypair.first = parentServer;
		createdServers[addressPair.first] = mypair;
	}else{
		parentServer = itr->second.first;
	}

	HighLevelServiceServer * server = new HighLevelServiceServer(*this, parentServer);

	createdServers[addressPair.first].second.push_back(server);
	return server;
}

HLCommServiceClient * HLCommunicationModuleImpl::startClientService(const string & server_address) throw(CommunicationModuleException){
	pair<string, string> addressPair = splitAddress(server_address);

	unique_lock<mutex> lk(parentListMutex);

	auto itr = createdClientConnections.find(addressPair.first);
	ServiceClient * parentClient;
	if(itr == createdClientConnections.end() ){
		HighLevelCommunicationModuleOptions & o = getOptions<HighLevelCommunicationModuleOptions>();		
		CommunicationModule * parentComm =  GET_INSTANCE(CommunicationModule, o.parentComm);
		assert(parentComm);

		// create a new client to connect to the target
		parentClient = parentComm->startClientService(addressPair.first, ConnectionCallback * ccb, MessageCallback * messageCallback);

		// add the client to the existing clients
		pair<ServiceClient*, list<HighLevelServiceClient*> > mypair;
		mypair.first = parentClient;
		createdClientConnections[addressPair.first] = mypair;
	}else{
		parentClient = itr->second.first;
	}

	HighLevelServiceClient * client = new HighLevelServiceClient(*this, parentClient, server_address);

	createdClientConnections[addressPair.first].second.push_back(client);
	return client;
}


extern "C" {
	void * CORE_HCOMM_INSTANCIATOR_NAME()
	{
		return new HLCommunicationModuleImpl();
	}
}
