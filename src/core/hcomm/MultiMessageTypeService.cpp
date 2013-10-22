#include <core/comm/CommunicationModule.hpp>
#include <core/hcomm/MultiMessageTypeService.hpp>
#include <core/container/container-binary-serializer.hpp>

#include <iostream>

using namespace std;
using namespace core;

namespace core{

static inline uint32_t headerLen(){
	const uint32_t type = 0;
	return j_serialization::serializeLen(type);
}

uint32_t MultiMessageTypeServiceClient::getType(const BareMessage * msg){
	uint32_t mtype;
	uint64_t pos = backend->headerSize();
	j_serialization::deserialize(mtype, msg->payload, pos, msg->size);
	return mtype;
}

uint32_t MultiMessageTypeServiceServer::getClientMessageType(const BareMessage * msg){
	uint32_t mtype;
	uint64_t pos = backend->headerSizeClientMessage();
	j_serialization::deserialize(mtype, msg->payload, pos, msg->size);
	return mtype;
}

BareMessage * MultiMessageTypeServiceClient::isend( uint32_t mtype, void * object ){
	uint64_t size = 0;
	uint64_t pos = 0;
	assert( callbacks.find(mtype) != callbacks.end() );

	size = headerLen() + backend->headerSize() + callbacks[mtype]->serializeMessageLen(object);

	char * payload = (char*) malloc(size);
	// we call the backend to serialize its header.	
	backend->serializeHeader(payload, pos, size);

	// serialize message type into the message
	j_serialization::serialize(mtype, payload, pos);

	// serialize object
	callbacks[mtype]->serializeMessage(object, payload, pos);

	BareMessage * msg = new BareMessage(payload, size);
	backend->isend(msg);
	return msg;
}

class MultiMessageClientCallback: public MessageCallback{
public:
	MultiMessageTypeServiceClient * client;

	MultiMessageClientCallback(MultiMessageTypeServiceClient * client): client(client){}

	void messageSendCB(BareMessage * msg){
		uint32_t mtype = client->getType(msg);
		client->callbacks[mtype]->messageSendCB(msg);
	}

	// this function de-serializes the buffer as well
	void messageResponseCB(BareMessage * msg, char * buffer, uint64_t buffer_size){
		uint32_t mtype = client->getType(msg);
		client->callbacks[mtype]->messageResponseCB(msg, buffer + headerLen(), buffer_size - headerLen());
	}

	void messageTransferErrorCB(BareMessage * msg, CommunicationError error){
		uint32_t mtype = client->getType(msg);
		client->callbacks[mtype]->messageTransferErrorCB(msg, error);
	}

	uint64_t serializeMessageLen(const void * msgObject){
		return 0;
	}

	void serializeMessage(const void * msgObject, char * buffer, uint64_t & pos){
		return;
	}
};


class MultiMessageServerCallback: public ServerCallback{
public:	
	MultiMessageTypeServiceServer * server;

	MultiMessageServerCallback(MultiMessageTypeServiceServer * server): server(server){}

	void messageReceivedCB(shared_ptr<ServerClientMessage> msg, const char * message_data, uint64_t buffer_size){
		uint32_t mtype = server->getClientMessageType(&*msg);

		auto itr = server->callbacks.find(mtype);
		if(itr == server->callbacks.end()){
			// message type is not registered => abort!
			msg->isendErrorResponse(CommunicationError::MESSAGE_TYPE_NOT_AVAILABLE);
			return;
		}

		itr->second->messageReceivedCB(msg, message_data + headerLen(), buffer_size - headerLen());
	}

	void responseSendCB(BareMessage * response){
		uint32_t mtype = server->getClientMessageType(response);
		server->callbacks[mtype]->responseSendCB(response);
	}

	void invalidMessageReceivedCB(CommunicationError error){
 	}


	//void responseTransferErrorCB(ServerClientMessage * msg, BareMessage * response, CommunicationError error){
	//	uint32_t mtype = server->getClientMessageType(msg);
		//server->callbacks[mtype]->responseSendCB(msg, response);
	//}

	uint64_t serializeResponseMessageLen(const ServerClientMessage * msg, const void * responseType){
		uint32_t mtype = server->getClientMessageType(msg);
		assert( server->callbacks.find(mtype) != server->callbacks.end() );
		return server->callbacks[mtype]->serializeResponseMessageLen(msg, responseType)  + headerLen();
	}

	void serializeResponseMessage(const ServerClientMessage * msg, const void * responseType, char * buffer, uint64_t & pos){

		uint32_t mtype = server->getClientMessageType(msg);
		j_serialization::serialize(mtype, buffer, pos);

		server->callbacks[mtype]->serializeResponseMessage(msg, responseType, buffer, pos);
	}
};

void MultiMessageTypeServiceClient::registerMessageCallback(uint32_t mtype, MessageCallback * msgcb){
	callbacks[mtype] = msgcb;
}



void MultiMessageTypeServiceServer::registerMessageCallback(uint32_t mtype, ServerCallback * msg_rcvd_callback){
	callbacks[mtype] = msg_rcvd_callback;
}


MultiMessageTypeServiceServer::MultiMessageTypeServiceServer(CommunicationModule * module, const string & address) throw(CommunicationModuleException) {
	cb = new MultiMessageServerCallback(this);
	try{
		backend = module->startServerService(address, cb);
	}catch(CommunicationModuleException & e){
		delete(cb);
		throw e;
	}
}


MultiMessageTypeServiceServer::~MultiMessageTypeServiceServer(){
	delete(backend);
	delete(cb);
}


MultiMessageTypeServiceClient::MultiMessageTypeServiceClient(CommunicationModule * module, const string & server_address, ConnectionCallback * ccb) throw (CommunicationModuleException) {
	cb = new MultiMessageClientCallback(this);
	try{
		backend = module->startClientService(server_address, ccb, cb);
	}catch(CommunicationModuleException & e){
		delete(cb);
		throw e;
	}
}


MultiMessageTypeServiceClient::~MultiMessageTypeServiceClient(){
	delete(backend);
	delete(cb);
}

}
