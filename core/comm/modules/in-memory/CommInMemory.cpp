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

namespace core{

class InMemoryServiceServer;
static map<string, InMemoryServiceServer*> servers;

class InMemoryServiceServer : public ServiceServer{
public:
	// simulates transient errors
	bool available = true;

	map<uint32_t, ServerCallback*> callbacks;

	void advertise(uint32_t mtype ){
		assert(false);
	}

	void ipublish( std::shared_ptr<CreatedMessage> msg ){
		assert(false);
	}

	void register_message_callback(uint32_t mtype, ServerCallback * msg_rcvd_callback){
		callbacks[mtype] = msg_rcvd_callback;
	}

	void unregister_message_callback(uint32_t mtype){
		callbacks.erase(mtype);
	}


	~InMemoryServiceServer(){
		cout << "Shutting down server" << endl;
	}
};

class InMemoryServiceClient : public ServiceClient{
public:	
	string address;
	bool available = true;

	InMemoryServiceClient(const string & address){
		this->address = address;
	}

	void ireconnect(){
		InMemoryServiceServer * server = servers[address];
		if (server == nullptr){
			// we have an error here!
			connectionCallback->connectionErrorCB(*this, ConnectionError::SERVER_NOT_ACCESSABLE);
			return;
		}

		connectionCallback->connectionSuccessfullCB(*this);
	}

	const string & getAddress() const{
		return this->address;
	}

	void isend( std::shared_ptr<CreatedMessage> msg );


	~InMemoryServiceClient(){
		cout << "Shutting down client \"" << getAddress() << "\"" << endl;
	}	
};


class InMemoryServerConnection : public Connection {
public:
	InMemoryServiceClient * client;
	std::shared_ptr<CreatedMessage> clientMsg;
	bool messageHasBeenSend = false;

	InMemoryServerConnection(InMemoryServiceClient * client, std::shared_ptr<CreatedMessage> clientMsg) : client(client), clientMsg(clientMsg){

	}

	void ireconnect(){
		// try to reconnect to the client to transmit the message.
		// not implemented here.
		assert(false);
	}

	const string & getAddress() const {
		return client->address;
	}

	void isend( std::shared_ptr<CreatedMessage> msg ){
		if(! client->available){
			msg->mcb.messageTransferErrorCB(this, msg, ConnectionError::CONNECTION_LOST);
			return;	
		}

		assert(! messageHasBeenSend);

		// the server-side message has been sent
		msg->mcb.messageSendCB(msg);

		// deliver the response to the client
		clientMsg->mcb.messageResponseCB(clientMsg, msg);

		messageHasBeenSend = true;
	}

};


void InMemoryServiceClient::isend( std::shared_ptr<CreatedMessage> msg ){
		InMemoryServiceServer * server = servers[address];
		if (server == nullptr){
			// we have an error here!
			msg->mcb.messageTransferErrorCB(this, msg, ConnectionError::SERVER_NOT_ACCESSABLE);
			return;
		}

		if(! server->available){
			msg->mcb.messageTransferErrorCB(this, msg, ConnectionError::CONNECTION_LOST);
			return;	
		}

		// notify server about reception of message
		if (server->callbacks.find(msg->type) != server->callbacks.end()){
			ServerCallback * cb = server->callbacks[msg->type];			
			cb->messageReceivedCB(std::shared_ptr<InMemoryServerConnection>(new InMemoryServerConnection(this, msg)), msg);
		}else{
			msg->mcb.messageTransferErrorCB(this, msg, ConnectionError::MESSAGE_TYPE_NOT_AVAILABLE);
			return;
		}

		// message completion
		msg->mcb.messageSendCB(msg);
	}

class CommInMemory : public CommunicationModule {
public:
	//virtual void setWorkProcessor() = 0; 
	virtual ServiceServer * startServerService(const string & address) throw(CommunicationModuleException){
		if (servers.find(address) != servers.end()){
			throw CommunicationModuleException("The address " + address + " is already occupied!");
		}

		servers[address] = new InMemoryServiceServer();
		return servers[address];
	}

	virtual ServiceClient * startClientService(const string & server_address, ConnectionCallback & ccb) throw(CommunicationModuleException){
		auto sc = new InMemoryServiceClient(server_address);
		sc->setConnectionCallback(& ccb);
		sc->ireconnect();
		return sc;
	}

	virtual void init(){

	}

	virtual ComponentOptions* AvailableOptions(){
		return new CommInMemoryOptions();
	}

	~CommInMemory(){
		for(auto itr = servers.begin(); itr != servers.end() ; itr++){
			delete(itr->second);
		}
	}
};

}

extern "C" {
	void * CORE_COMM_INSTANCIATOR_NAME()
	{
		return new core::CommInMemory();
	}
}
