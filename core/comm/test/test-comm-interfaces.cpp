#include "test-comm-interfaces.hpp"

#include <core/comm/MultiMessageTypeService.hpp>

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