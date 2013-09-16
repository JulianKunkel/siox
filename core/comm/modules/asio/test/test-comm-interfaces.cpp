#include <assert.h>
#include <iostream>

#include <map>

#include <core/comm/CommunicationModule.hpp>
#include <core/module/ModuleLoader.hpp>

using namespace core;
using namespace std;

class MyConnectionCallback: public ConnectionCallback{
public:
	int retries = 0;

	void connectionErrorCB(Connection & connection, ConnectionError error){
		cout << "connectionErrorCB " << ((int) error) << " in  " << connection.getAddress() << endl;
		if(retries >= 2){
			cout << "Retry count reached, stopping.";		
			return;
		}

		retries++;
		// normally we would add a sleeping time here...
		connection.ireconnect();
	}

	void connectionSuccessfullCB(Connection & connection){
		retries = 0;
	}
};

class MyClientMessageCallback: public MessageCallback{
public:
	ConnectionError error; 
	std::shared_ptr<Message> errMsg;
	Connection * errconnection;

	std::shared_ptr<Message> msg;
	std::shared_ptr<Message> response;

	enum class State: uint8_t{
		Init,
		MessageSend,
		MessageResponseReceived,
		Error
	};

	State state = State::Init;

	void messageSendCB(std::shared_ptr<CreatedMessage> msg){
		state = State::MessageSend;
		this->msg = msg;
	}

	void messageResponseCB(std::shared_ptr<CreatedMessage> msg, std::shared_ptr<Message> response){
		state = State::MessageResponseReceived;

		this->msg = msg;
		this->response = response;
	}

	void messageTransferErrorCB(Connection * connection, std::shared_ptr<CreatedMessage> msg, ConnectionError error){
		this->error = error;
		this->errMsg = msg;
		this->errconnection = connection;

		state = State::Error;
	}	
};

class MyServerCallback: public ServerCallback{
public:
	int messagesReceived = 0;
	std::shared_ptr<Message> lastMessage;
	std::shared_ptr<Connection> lastConnection;

	 void messageReceivedCB(std::shared_ptr<Connection> connection, std::shared_ptr<Message> msg){
	 	this->messagesReceived++;
	 	this->lastMessage = msg;
	 	this->lastConnection = connection;
	 }
};

int main(){
	CommunicationModule * comm = core::module_create_instance<CommunicationModule>( "", "siox-core-comm-asio", CORE_COMM_INTERFACE );

	assert(comm != nullptr);

	ServiceServer * s1 = comm->startServerService("tcp://localhost:8080");
	ServiceServer * s2 = comm->startServerService("ipc://localhost:8080");

	try{
		ServiceServer * s3 = comm->startServerService("local:1");
		assert(s3 == nullptr);
	}catch(CommunicationModuleException & e){
		// we expect that this server address is already occupied.
	}

	MyConnectionCallback myCCB;

	// try to connect to a port which does not exist, so we should retry connecting (here blocking):
	ServiceClient * c1 = comm->startClientService("local:3", myCCB);
	assert(c1 != nullptr);
	assert(myCCB.retries == 2);


	ServiceClient * c2 = comm->startClientService("local:2", myCCB);

	// produce an error because the target message type is not registered.
	MyClientMessageCallback mycCB;
	auto msg = std::shared_ptr<CreatedMessage>(new CreatedMessage(mycCB, 1, "test", 4));
	c2->isend(msg);
	
	assert(mycCB.state == MyClientMessageCallback::State::Error);
	assert(mycCB.error == ConnectionError::MESSAGE_TYPE_NOT_AVAILABLE);


	// register a message type on the server and re-send the previous message:
	MyServerCallback mySCB;
	s2->register_message_callback(1, & mySCB);

	c2->isend(msg);

	// check if the message has been received
	assert(mySCB.messagesReceived == 1);
	assert(mySCB.lastMessage == msg);
	assert(mySCB.lastConnection->getAddress() == c2->getAddress());

	// check if the message has been send properly
	assert(mycCB.state == MyClientMessageCallback::State::MessageSend);

	// send the response
	MyClientMessageCallback myServerResponseCB;
	auto response = std::shared_ptr<CreatedMessage>(new CreatedMessage(myServerResponseCB, 0, "response", 8));	
	mySCB.lastConnection->isend(response);

	// check if the message response has been properly received
	assert(mycCB.state == MyClientMessageCallback::State::MessageResponseReceived);
	assert(mycCB.response == response);
	assert(mycCB.msg == msg);

	// check if the proper reception is marked on the server-side
	assert(myServerResponseCB.state == MyClientMessageCallback::State::MessageSend);
	assert(myServerResponseCB.msg == response);

	// close connections:
	delete(c1);
	delete(comm);

	return 0;
}