/*
 This class contains an in-memory communication server which allows to InMemory inter-module communication.
 */
#include <assert.h>
#include <iostream>

#include <map>

#include <core/comm/CommunicationModuleImplementation.hpp>

#include "CommInMemoryOptions.hpp"

using namespace core;
using namespace std;

class InMemoryServiceServer;
class InMemoryServiceClient;
static map<string, InMemoryServiceServer*> servers;



class InMemoryServiceServer : public ServiceServer{
public:
	// simulates transient errors
	bool available = false;
	const string address;

	InMemoryServiceServer(const string & address) throw (CommunicationModuleException) : address(address) {
		if (servers.find(address) != servers.end()){
			throw CommunicationModuleException("The address \"" + address + "\" is already occupied!");
		}
		servers[address] = this;
	}

	inline ServerCallback * getMessageCallback(){
		return messageCallback;
	}

	void ipublish( void * msg ){
		assert(false);
	}

	void listen() throw(CommunicationModuleException){
		available = true;
	}

	uint32_t headerSizeClientMessage(){
		return 0;
	}

	~InMemoryServiceServer(){
		cout << "Shutting down server" << endl;
		auto itr = servers.find(address);
		servers.erase(itr);
	}
};


class InMemoryServerClientMessage : public ServerClientMessage{
public:
	BareMessage * msg;
	InMemoryServiceClient * client;
	InMemoryServiceServer * server;

	InMemoryServerClientMessage(BareMessage * msg, InMemoryServiceClient * client, InMemoryServiceServer * server) : ServerClientMessage( (char*) malloc(msg->size), msg->size), msg(msg), client(client), server(server) { 
		memcpy((void*) payload, msg->payload, size);
	}	

	void isendResponse(void * object);
	void isendErrorResponse(CommunicationError error);
};

class InMemoryServiceClient : public ServiceClient{
public:	
	string address;
	bool available = true;

	uint32_t headerSize(){
		return 0;
	}

	void serializeHeader(char * buffer, uint64_t & pos, uint64_t size){

	}

	InMemoryServiceClient(const string & address){
		this->address = address;
	}

	void ireconnect(){
		InMemoryServiceServer * server = servers[address];
		if (server == nullptr){
			// we have an error here!
			connectionCallback->connectionErrorCB(*this, CommunicationError::SERVER_NOT_ACCESSABLE);
			return;
		}

		connectionCallback->connectionSuccessfullCB(*this);
	}

	const string & getAddress() const{
		return this->address;
	}

	BareMessage * isend( void * object ){
		// we have to serialize the object
		uint64_t msg_size = messageCallback->serializeMessageLen(object);
		char * payload = (char*) malloc(msg_size);
		uint64_t pos = 0;

		messageCallback->serializeMessage(object, payload, pos);

		assert(pos == msg_size);

		BareMessage * msg = new BareMessage(payload, msg_size);

		isend(msg);
		return msg;
	}
	
	void isend( BareMessage * msg ){
		InMemoryServiceServer * server = servers[address];

		if (server == nullptr){
			// we have an error here!
			messageCallback->messageTransferErrorCB(msg, CommunicationError::SERVER_NOT_ACCESSABLE);			
			return;
		}

		if(! server->available){
			messageCallback->messageTransferErrorCB(msg, CommunicationError::CONNECTION_LOST);
			return;	
		}

		// message completion on the client
		messageCallback->messageSendCB(msg);

		shared_ptr<ServerClientMessage>  smsg = shared_ptr<ServerClientMessage>(new InMemoryServerClientMessage(msg, this, server));

		// notify server about reception of message
		server->getMessageCallback()->messageReceivedCB(smsg, smsg->payload, msg->size );
		//	msg->mcb.messageTransferErrorCB(msg, ConnectionError::MESSAGE_TYPE_NOT_AVAILABLE);
	}

	inline MessageCallback * getMessageCallback(){
		return messageCallback;
	}


	~InMemoryServiceClient(){
		cout << "Shutting down client \"" << getAddress() << "\"" << endl;
	}	
};




void InMemoryServerClientMessage::isendResponse(void * object){

	uint64_t len = server->getMessageCallback()->serializeResponseMessageLen(this, object);
	char * payload = (char *) malloc(len);
	uint64_t pos = 0;
	server->getMessageCallback()->serializeResponseMessage(this, object, payload, pos);
	assert(len == pos);

	BareMessage * msg = new BareMessage(payload, len);
	if(! client->available){
		// notify the server that the connection has been lost
		delete(msg);
		return;
	}

	// the server-side message has been sent
	server->getMessageCallback()->responseSendCB(msg);

	// deliver the response to the client
	client->getMessageCallback()->messageResponseCB(this->msg, payload, len);

	delete(msg);

}

void InMemoryServerClientMessage::isendErrorResponse(CommunicationError error){
	if(client->available){
		client->getMessageCallback()->messageTransferErrorCB(msg, error);
	}
	delete(msg);
}



class CommInMemory : public CommunicationModule {
public:
	//virtual void setWorkProcessor() = 0; 
	virtual ServiceServer * startServerService(const string & address) throw(CommunicationModuleException){
		return new InMemoryServiceServer(address);
	}

	virtual ServiceClient * startClientService(const string & server_address) throw(CommunicationModuleException){
		return new InMemoryServiceClient(server_address);
	}

	virtual void init(){

	}

	virtual ComponentOptions* AvailableOptions(){
		return new CommInMemoryOptions();
	}
};

extern "C" {
	void * CORE_COMM_INSTANCIATOR_NAME()
	{
		return new CommInMemory();
	}
}
