#include <assert.h>
#include <iostream>

#include <map>

#include <core/comm/CommunicationModule.hpp>
#include <core/module/ModuleLoader.hpp>

#include <core/comm/MultiMessageTypeService.hpp>

#include <core/container/container-binary-serializer.hpp>

using namespace core;
using namespace std;

class MyConnectionCallback: public ConnectionCallback{
public:
	int retries = 0;

	void connectionErrorCB(ServiceClient & connection, CommunicationError error){
		cout << "connectionErrorCB " << ((int) error) << " in  " << connection.getAddress() << endl;
		if(retries >= 2){
			cout << "Retry count reached, stopping.";		
			return;
		}

		retries++;
		// normally we would add a sleeping time here...
		connection.ireconnect();
	}

	void connectionSuccessfullCB(ServiceClient & connection){
		retries = 0;
	}
};

class MyClientMessageCallback: public MessageCallback{
public:
	CommunicationError error; 
	BareMessage * errMsg;

	BareMessage * msg;
	string response;

	enum class State: uint8_t{
		Init,
		MessageSend,
		MessageResponseReceived,
		Error
	};

	State state = State::Init;

	void messageSendCB(BareMessage * msg){
		cout << "messageSendCB" << endl;

		state = State::MessageSend;
		this->msg = msg;
	}

	void messageResponseCB(BareMessage * msg, char * buffer, uint64_t buffer_size){
		cout << "messageResponseCB" << endl;

		state = State::MessageResponseReceived;

		this->msg = msg;
		uint64_t pos = 0;
		j_serialization::deserialize(this->response, buffer, pos, buffer_size);
		assert(buffer_size == this->response.length() + 4 );
	}

	void messageTransferErrorCB(BareMessage * msg, CommunicationError error){
		cout << "messageTransferErrorCB" << endl;

		this->error = error;
		this->errMsg = msg;

		state = State::Error;
	}

	uint64_t serializeMessageLen(const void * msgObject) {
		return j_serialization::serializeLen(*(string*) msgObject);
	}

	void serializeMessage(const void * msgObject, char * buffer, uint64_t & pos){
		j_serialization::serialize( ((string *) msgObject)->c_str(), buffer, pos ) ;
	}


};

class EmptyServerCallback: public ServerCallback{
public:
	// The ownership of msg is given to this function.
	void messageReceivedCB(ServerClientMessage * msg, const char * message_data, uint64_t buffer_size){
		msg->isendErrorResponse( CommunicationError::MESSAGE_TYPE_NOT_AVAILABLE );
	}

	void responseSendCB(ServerClientMessage * msg, BareMessage * response){
	}

	void responseTransferErrorCB(ServerClientMessage * msg, BareMessage * response, CommunicationError error){
		delete(msg);
		delete(response);
	};

	uint64_t serializeResponseMessageLen(const ServerClientMessage * msg, const void * responseType){
		assert(false);
	}

	void serializeResponseMessage(const ServerClientMessage * msg, const void * responseType, char * buffer, uint64_t & pos){
		assert(false);
	}

};

class MyServerCallback: public ServerCallback{
public:
	enum class State: uint8_t{
		Init,
		MessageReceived,
		MessageResponseSend,
		Error
	};

	State state = State::Init;

	int messagesReceived = 0;
	ServerClientMessage * lastMessage;
	BareMessage * response;

	string lastMessageText;


	 void messageReceivedCB(ServerClientMessage * msg, const char * message_data, uint64_t buffer_size){
	 	this->messagesReceived++;
	 	this->lastMessage = msg;
		
		this->lastMessageText = string((char*) message_data, buffer_size);
	 	state = State::MessageReceived;
	 }


	void responseSendCB(ServerClientMessage * msg, BareMessage * r){
		state = State::MessageResponseSend;
		response = r;
	}

	void responseTransferErrorCB(ServerClientMessage * msg, BareMessage * response, CommunicationError error)
	{
		state = State::Error;
	};


	uint64_t serializeResponseMessageLen(const ServerClientMessage * msg, const void * responseType){
		return j_serialization::serializeLen(*(string*) responseType);
	}

	void serializeResponseMessage(const ServerClientMessage * msg,  const void * responseType, char * buffer, uint64_t & pos){
		j_serialization::serialize( ((string *) responseType)->c_str(), buffer, pos ) ;
	}

};

int main(){
	CommunicationModule * backend = core::module_create_instance<CommunicationModule>( "", "siox-core-comm-inmemory", CORE_COMM_INTERFACE );
	MultiMessageCommunication * comm = new MultiMessageCommunication(backend);

	assert(comm != nullptr);
	EmptyServerCallback myEmptyServerCB;

	MultiMessageTypeServiceServer * s1 = comm->startServerService("local:1");
	MultiMessageTypeServiceServer * s2 = comm->startServerService("local:2");


	try{
		MultiMessageTypeServiceServer * s3 = comm->startServerService("local:1");
		assert(s3 == nullptr);
	}catch(CommunicationModuleException & e){
		// we expect that this server address is already occupied.
	}

	MyConnectionCallback myCCB;
	MyClientMessageCallback myClientMessageCB;

	// try to connect to a port which does not exist, so we should retry connecting (here blocking):
	MultiMessageTypeServiceClient * c1 = comm->startClientService("local:3", & myCCB);
	assert(c1 != nullptr);
	assert(myCCB.retries == 2);

	MultiMessageTypeServiceClient * c2 = comm->startClientService("local:2", & myCCB);

	// produce an error because the target message type is not registered.
	string data = "test";

	c2->registerMessageCallback(1, & myClientMessageCB);

	c2->isend(1, & data);
	
	assert(myClientMessageCB.state == MyClientMessageCallback::State::Error);
	assert(myClientMessageCB.error == CommunicationError::MESSAGE_TYPE_NOT_AVAILABLE);

	delete(s2);
	s2 = nullptr;

	MyServerCallback mySCB;	
	MultiMessageTypeServiceServer * s3 = comm->startServerService("local:2");

	// register a message type on the server and re-send the previous message:
	s3->registerMessageCallback(1, & mySCB);
	c2->isend(1, & data);

	// check if the message has been received
	assert(mySCB.messagesReceived == 1);
	// check if the message has been send properly
	assert(myClientMessageCB.state == MyClientMessageCallback::State::MessageSend);


	c2->registerMessageCallback(2, & myClientMessageCB);
	c2->isend(2, & data);
	assert(myClientMessageCB.state == MyClientMessageCallback::State::Error);
	assert(myClientMessageCB.error == CommunicationError::MESSAGE_TYPE_NOT_AVAILABLE);


	// send the response
	string response("response");
	mySCB.lastMessage->isendResponse(& response);

	// check if the message response has been properly received
	assert(myClientMessageCB.state == MyClientMessageCallback::State::MessageResponseReceived);
	assert(myClientMessageCB.response == response);

	// check if the proper reception is marked on the server-side
	assert(mySCB.state == MyServerCallback::State::MessageResponseSend);
	//assert(mySCB.response == response);

	// close connections:
	delete(c1);
	delete(comm);

	return 0;
}