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
class InMemoryServiceClient;
static map<string, InMemoryServiceServer*> servers;

class InMemoryServiceServer : public ServiceServer{
public:
	// simulates transient errors
	bool available = true;

	map<uint32_t, ServerCallback*> callbacks;
	map<std::shared_ptr<Message>, InMemoryServiceClient*> pendingResponses;

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

	void isend( std::shared_ptr<Message> msg, std::shared_ptr<CreatedMessage> response );

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

	void isend( std::shared_ptr<CreatedMessage> msg ){
		InMemoryServiceServer * server = servers[address];
		if (server == nullptr){
			// we have an error here!
			msg->mcb.messageTransferErrorCB(msg, ConnectionError::SERVER_NOT_ACCESSABLE);
			return;
		}

		if(! server->available){
			msg->mcb.messageTransferErrorCB(msg, ConnectionError::CONNECTION_LOST);
			return;	
		}

		// notify server about reception of message
		if (server->callbacks.find(msg->type) != server->callbacks.end()){
			ServerCallback * cb = server->callbacks[msg->type];			
			cb->messageReceivedCB(server, msg);
		}else{
			msg->mcb.messageTransferErrorCB(msg, ConnectionError::MESSAGE_TYPE_NOT_AVAILABLE);
			return;
		}

		// message completion
		msg->mcb.messageSendCB(msg);
	}


	~InMemoryServiceClient(){
		cout << "Shutting down client \"" << getAddress() << "\"" << endl;
	}	
};


void InMemoryServiceServer::isend( std::shared_ptr<Message> msg, std::shared_ptr<CreatedMessage> response ){
		auto  it = pendingResponses.find(msg);

		assert( it != pendingResponses.end());

		pendingResponses.erase(it);

		InMemoryServiceClient * client = it->second;

		CreatedMessage * clientMessage = ((CreatedMessage*)(&*msg));

		if(! client->available){
			clientMessage->mcb.messageTransferErrorCB(response, ConnectionError::CONNECTION_LOST);
			return;	
		}

		// the server-side message has been sent
		response->mcb.messageSendCB(response);


		// deliver the response to the client
		clientMessage->mcb.messageResponseCB(*(std::shared_ptr<CreatedMessage> *) & msg, *(std::shared_ptr<Message> * ) & response);
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
