/*
 This class contains an in-memory communication server which allows to InMemory inter-module communication.
 */
#include <assert.h>
#include <iostream>

#include <map>
#include <utility>
#include <exception> 

#include <core/comm/CommunicationModuleImplementation.hpp>

#include "GIOOptions.hpp"

using namespace core;
using namespace std;

namespace core{

class GIOServiceServer;
static map<string, GIOServiceServer*> servers;

// This function splits the string <hostname>:<port> into the tupel
static pair<string, uint16_t> splitAddress(const string & address){
	int pos = address.find(":");
	if (pos == -1){
		uint16_t port = (uint16_t) atoi(address.c_str());
		if (port == 0){
			throw CommunicationModuleException("Invalid address " + address);
		}
		return {"localhost", port };
	}

	uint16_t port = (uint16_t) atoi(address.substr(pos+1).c_str());
	if (port == 0){
		throw CommunicationModuleException("Invalid address " + address);
	}
	
	return {address.substr(0, pos), port};
}

class GIOServiceServer : public ServiceServer{
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


	~GIOServiceServer(){
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
		GIOServiceServer * server = servers[address];
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
		GIOServiceServer * server = servers[address];
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

static void printAddress(const pair<string, uint16_t> & pair){
	cout << pair.first << " " << pair.second << endl;
}

class GIOCommModule : public CommunicationModule {
public:
	//virtual void setWorkProcessor() = 0; 
	virtual ServiceServer * startServerService(const string & address) throw(CommunicationModuleException){
		if (servers.find(address) != servers.end()){
			throw CommunicationModuleException("The address " + address + " is already occupied!");
		}

		servers[address] = new GIOServiceServer();
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
		return new GIOOptions();
	}

	~GIOCommModule(){
		for(auto itr = servers.begin(); itr != servers.end() ; itr++){
			delete(itr->second);
		}
	}
};

}

extern "C" {
	void * CORE_COMM_INSTANCIATOR_NAME()
	{
		return new core::GIOCommModule();
	}
}
