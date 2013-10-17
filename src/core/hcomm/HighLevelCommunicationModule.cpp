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
 */

#include <core/comm/CommunicationModuleImplementation.hpp>

#include <core/comm/ServiceServer.hpp>
#include <core/comm/ServiceClient.hpp>

#include <mutex>
#include <utility>
#include <unordered_map>
#include <list>

#include <core/hcomm/HighLevelCommunicationModule.hpp>

using namespace core;
using namespace std;

class MultiplexerServiceClient;
class MultiplexerServiceServer;

class HLCommunicationModuleImpl : public HLCommunicationModule {
public:
	virtual HLCommServiceServer * startServerService(const string & address) throw(CommunicationModuleException);

	virtual HLCommServiceClient * startClientService(const string & server_address) throw(CommunicationModuleException);

	virtual void init(){
		MultiplexerOptions & o = getOptions<MultiplexerOptions>();
		parentComm =  GET_INSTANCE(CommunicationModule, o.parentComm);
		assert(parentComm);
	}

	virtual ComponentOptions* AvailableOptions(){
		return new MultiplexerOptions();
	}

	void hasStopped(MultiplexerServiceClient * client){
		unique_lock<mutex> lk(parentListMutex);
		// TODO remove from the unordered map and list
		// Delete the client if all virtual connections are completed
	}

	void hasStopped(MultiplexerServiceServer * client){
		unique_lock<mutex> lk(parentListMutex);
		// TODO remove from the unordered map and list
		// Delete the server if all virtual connections are completed
	}

private:
	CommunicationModule * parentComm;

	// the mutex protects createdServers and createdClientConnections
	mutex parentListMutex;

	// map the real address to only one server
	unordered_map<string, pair<ServiceServer*, list<MultiplexerServiceServer*> > > createdServers;

	unordered_map<string, pair<ServiceClient*, list<MultiplexerServiceClient*> > > createdClientConnections;
};



class MultiplexerServiceClient : public ServiceClient{
public:
	virtual const string & getAddress() const {
		return address;
	}


	MultiplexerServiceClient(MultiplexerModule & parent, ServiceClient * client, const string & address) : parent(parent), address(address), client(client){

	}

	virtual void ireconnect(){

	}

	virtual void isend( BareMessage * msg ){

	}

	virtual uint32_t headerSize(){

	}

	virtual void serializeHeader(char * buffer, uint64_t & pos, uint64_t size){

	}

	~MultiplexerServiceClient(){
		parent.hasStopped(this);
	}

private:
	MultiplexerModule & parent;
	string address;
	ServiceClient * client;
};

class MultiplexerServiceServer : public ServiceServer{
public:
	virtual void listen() throw(CommunicationModuleException){

	}
	
	virtual void ipublish( void * object ){

	}

	virtual uint32_t headerSizeClientMessage(){

	}

	MultiplexerServiceServer(MultiplexerModule & parent, ServiceServer * server) : parent(parent), server(server){

	}

	~MultiplexerServiceServer(){
		parent.hasStopped(this);
	}

private:
	MultiplexerModule & parent;
	ServiceServer * server;
};

static pair<string, string> splitAddress(const string & address) throw(CommunicationModuleException){
	size_t pos = address.rfind ("#");
	if (pos == string::npos ){
		throw CommunicationModuleException( string("The address is invalid ") + address, CommunicationError::INVALID_ADDRESS);
	}
	return { address.substr(0, pos), address.substr(pos+1)};	
}


ServiceServer * MultiplexerModule::startServerService(const string & address) throw(CommunicationModuleException){

	pair<string, string> addressPair = splitAddress(address);

	unique_lock<mutex> lk(parentListMutex);

	auto itr = createdServers.find(addressPair.first);
	ServiceServer * parentServer;
	if(itr == createdServers.end() ){

		// create a new server to listen
		parentServer = parentComm->startServerService(addressPair.first, ServerCallback * msg_rcvd_callback);

		// add the server to the existing servers
		pair<ServiceServer*, list<MultiplexerServiceServer*> > mypair;
		mypair.first = parentServer;
		createdServers[addressPair.first] = mypair;
	}else{
		parentServer = itr->second.first;
	}

	MultiplexerServiceServer * server = new MultiplexerServiceServer(*this, parentServer);

	createdServers[addressPair.first].second.push_back(server);
	return server;
}

ServiceClient * MultiplexerModule::startClientService(const string & server_address) throw(CommunicationModuleException){
	pair<string, string> addressPair = splitAddress(server_address);

	unique_lock<mutex> lk(parentListMutex);

	auto itr = createdClientConnections.find(addressPair.first);
	ServiceClient * parentClient;
	if(itr == createdClientConnections.end() ){
		// create a new client to connect to the target
		parentClient = parentComm->startClientService(addressPair.first, ConnectionCallback * ccb, MessageCallback * messageCallback);

		// add the client to the existing clients
		pair<ServiceClient*, list<MultiplexerServiceClient*> > mypair;
		mypair.first = parentClient;
		createdClientConnections[addressPair.first] = mypair;
	}else{
		parentClient = itr->second.first;
	}

	MultiplexerServiceClient * client = new MultiplexerServiceClient(*this, parentClient, server_address);

	createdClientConnections[addressPair.first].second.push_back(client);
	return client;
}


extern "C" {
	void * CORE_COMM_INSTANCIATOR_NAME()
	{
		return new MultiplexerModule();
	}
}
