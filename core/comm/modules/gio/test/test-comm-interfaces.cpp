#include <assert.h>
#include <iostream>

#include <map>
#include <mutex>
#include <condition_variable>

#include <core/comm/CommunicationModule.hpp>
#include <core/module/ModuleLoader.hpp>

using namespace core;
using namespace std;

class MyConnectionCallback: public ConnectionCallback{
public:
	int retries = 0;

	bool sth_happened = false;

	mutex m;
	condition_variable cv;

	void waitUntilSthHappened(){
        unique_lock<mutex> lk(m);
        if (sth_happened){
        	sth_happened = false;
        	return;
        }
        cv.wait(lk);
        sth_happened = false;
	}

	void connectionErrorCB(ServiceClient & connection, ConnectionError error){
		unique_lock<mutex> lk(m);
		sth_happened = true;
		cv.notify_one();

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
		unique_lock<mutex> lk(m);
		sth_happened = true;
		cv.notify_one();

		retries = 0;
	}
};

class MyClientMessageCallback: public MessageCallback{
public:
	ConnectionError error; 
	ClientMessage * errMsg;

	ClientMessage * msg;
	ResponseMessage * response;

	enum class State: uint8_t{
		Init,
		MessageSend,
		MessageResponseReceived,
		Error
	};

	State state = State::Init;

	void messageSendCB(ClientMessage * msg){
		cout << "messageSendCB" << endl;

		state = State::MessageSend;
		this->msg = msg;
	}

	void messageResponseCB(ClientMessage * msg, ResponseMessage * response){
		cout << "messageResponseCB" << endl;

		state = State::MessageResponseReceived;

		this->msg = msg;
		this->response = response;
	}

	void messageTransferErrorCB(ClientMessage * msg, ConnectionError error){
		cout << "messageTransferErrorCB" << endl;

		this->error = error;
		this->errMsg = msg;

		state = State::Error;
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
	ServiceServer * lastServer;
	ResponseMessage * response;


	 void messageReceivedCB(ServiceServer * server, ServerClientMessage * msg){
	 	this->messagesReceived++;
	 	this->lastMessage = msg;
	 	this->lastServer = server;

	 	state = State::MessageReceived;
	 }


	void responseSendCB(ServerClientMessage * msg, ResponseMessage * r){
		state = State::MessageResponseSend;
		response = r;
	}

	void responseTransferErrorCB(ServerClientMessage * msg, ResponseMessage * response, ConnectionError error)
	{
		state = State::Error;
	};
};

int main(){
	CommunicationModule * comm = core::module_create_instance<CommunicationModule>( "", "siox-core-comm-gio", CORE_COMM_INTERFACE );

	comm->init();

	assert(comm != nullptr);

	ServiceServer * s1 = comm->startServerService("localhost:8082");
	ServiceServer * s2 = comm->startServerService("localhost:8081");

	try{
		ServiceServer * s3 = comm->startServerService("localhost:8082");
		assert(s3 == nullptr);
	}catch(CommunicationModuleException & e){
		// we expect that this server address is already occupied.
	}

	MyConnectionCallback myCCB;

	// try to connect to a port which does not exist, so we should retry connecting (here blocking):
	ServiceClient * c1 = comm->startClientService("localhost:80844", myCCB);

	myCCB.waitUntilSthHappened();
	myCCB.waitUntilSthHappened();

	assert(c1 != nullptr);
	assert(myCCB.retries == 2);

	ServiceClient * c2 = comm->startClientService("local:2", myCCB);

	// produce an error because the target message type is not registered.
	MyClientMessageCallback mycCB;
	auto msg = new ClientMessage(& mycCB, "test", 4);
	c2->isend(msg);
	
	assert(mycCB.state == MyClientMessageCallback::State::Error);
	assert(mycCB.error == ConnectionError::MESSAGE_TYPE_NOT_AVAILABLE);


	// register a message type on the server and re-send the previous message:
	msg = new ClientMessage(& mycCB, "test", 4);

	MyServerCallback mySCB;
	s2->setMessageCallback(& mySCB);

	c2->isend(msg);

	// check if the message has been received
	assert(mySCB.messagesReceived == 1);
	// check if the message has been send properly
	assert(mycCB.state == MyClientMessageCallback::State::MessageSend);

	// send the response
	auto response = new ResponseMessage(1, & mySCB, "response", 8);	
	mySCB.lastMessage->isendResponse(response);

	// check if the message response has been properly received
	assert(mycCB.state == MyClientMessageCallback::State::MessageResponseReceived);
	assert(mycCB.response->payload == response->payload);

	// check if the proper reception is marked on the server-side
	assert(mySCB.state == MyServerCallback::State::MessageResponseSend);
	assert(mySCB.response->payload == response->payload);

	// close connections:
	delete(c1);
	delete(comm);

	return 0;
}

