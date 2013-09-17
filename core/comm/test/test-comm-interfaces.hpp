#include <assert.h>
#include <iostream>

#include <map>
#include <mutex>
#include <condition_variable>

#include <core/comm/CommunicationModule.hpp>
#include <core/module/ModuleLoader.hpp>

#include <core/container/container-binary-serializer.hpp>

using namespace core;
using namespace std;

class ProtectRaceConditions{
public:
	void waitUntilSthHappened(){
        unique_lock<mutex> lk(m);
        if (sth_happened){
        	sth_happened = false;
        	return;
        }
        cv.wait(lk);
        sth_happened = false;
	}

protected:

	bool sth_happened = false;

	mutex m;
	condition_variable cv;

	void sthHappens(){
		unique_lock<mutex> lk(m);
		sth_happened = true;
		cv.notify_one();
	}
};

class MyConnectionCallback: public ConnectionCallback, public ProtectRaceConditions{
public:
	int retries = 0;

	void connectionErrorCB(ServiceClient & connection, CommunicationError error){
		
		cout << "connectionErrorCB " << ((int) error) << " in  " << connection.getAddress()  << " retry: " << retries << endl;
		
		if(retries >= 2){
			cout << "Retry count reached, stopping.";
			sthHappens();
			return;
		}

		retries++;
		
		sthHappens();

		// normally we would add a sleeping time here...
		connection.ireconnect();
	}

	void connectionSuccessfullCB(ServiceClient & connection){
		cout << "connectionSuccessfullCB";
		retries = 0;

		sthHappens();
	}
};

class MyClientMessageCallback: public MessageCallback, public ProtectRaceConditions{
public:
	CommunicationError error; 
	shared_ptr<BareMessage> errMsg;

	shared_ptr<BareMessage> msg;
	string response;

	enum class State: uint8_t{
		Init = 0,
		MessageSend = 1,
		MessageResponseReceived = 2,
		Error = 3
	};

	State state = State::Init;

	void messageSendCB(BareMessage * msg){
		cout << "messageSendCB" << endl;

		state = State::MessageSend;
		this->msg = shared_ptr<BareMessage>(new BareMessage(*msg));

		sthHappens();
	}

	void messageResponseCB(BareMessage * msg, char * buffer, uint64_t buffer_size){
		cout << "messageResponseCB" << endl;

		state = State::MessageResponseReceived;

		this->msg = shared_ptr<BareMessage>(new BareMessage(*msg));
		uint64_t pos = 0;
		j_serialization::deserialize(this->response, buffer, pos, buffer_size);

		assert(buffer_size == pos );

		sthHappens();
	}

	void messageTransferErrorCB(BareMessage * msg, CommunicationError error){
		cout << "messageTransferErrorCB" << endl;

		this->error = error;
		this->errMsg = shared_ptr<BareMessage>(new BareMessage(*msg));

		state = State::Error;
		sthHappens();
	}

	uint64_t serializeMessageLen(const void * msgObject) {
		return j_serialization::serializeLen(*(string*) msgObject);
	}

	void serializeMessage(const void * msgObject, char * buffer, uint64_t & pos){
		j_serialization::serialize( *(string *) msgObject, buffer, pos ) ;
	}


};

class EmptyServerCallback: public ServerCallback, public ProtectRaceConditions{
public:
	bool invalidMessage = false;

	// The ownership of msg is given to this function.
	void messageReceivedCB(shared_ptr<ServerClientMessage> msg, const char * message_data, uint64_t buffer_size){
		msg->isendErrorResponse( CommunicationError::MESSAGE_TYPE_NOT_AVAILABLE );
		sthHappens();
	}

	uint64_t serializeResponseMessageLen(const ServerClientMessage * msg, const void * responseType){
		assert(false);
	}

	void serializeResponseMessage(const ServerClientMessage * msg, const void * responseType, char * buffer, uint64_t & pos){
		assert(false);
	}

	void invalidMessageReceivedCB(CommunicationError error){
		cout << "Invalid message, error: " << (uint32_t) error << endl;

		invalidMessage = true;
		sthHappens();
	}
};

class MyServerCallback: public ServerCallback, public ProtectRaceConditions{
public:
	bool invalidMessage = false;

	enum class State: uint8_t{
		Init,
		MessageReceived,
		MessageResponseSend,
		Error
	};

	State state = State::Init;

	int messagesReceived = 0;
	shared_ptr<ServerClientMessage> lastMessage;
	shared_ptr<BareMessage> response;

	string lastMessageText;


	 void messageReceivedCB(shared_ptr<ServerClientMessage> msg, const char * message_data, uint64_t buffer_size){
	 	this->messagesReceived++;
	 	this->lastMessage = msg;
		
		this->lastMessageText = string((char*) message_data, buffer_size);
	 	state = State::MessageReceived;

	 	sthHappens();
	 }


	void responseSendCB(ServerClientMessage * msg, BareMessage * r){
		state = State::MessageResponseSend;
		response = shared_ptr<BareMessage>(new BareMessage(*r));

		sthHappens();
	}

	void invalidMessageReceivedCB(CommunicationError error){		
		cout << "Invalid message, error: " << (uint32_t) error << endl;
		invalidMessage = true;
		sthHappens();
	}


	uint64_t serializeResponseMessageLen(const ServerClientMessage * msg, const void * responseType){
		return j_serialization::serializeLen(*(string*) responseType);
	}

	void serializeResponseMessage(const ServerClientMessage * msg,  const void * responseType, char * buffer, uint64_t & pos){
		j_serialization::serialize( ((string *) responseType)->c_str(), buffer, pos ) ;
	}

};


void doubleListenerError(CommunicationModule * comm, string address1, string address2, string address3){
	EmptyServerCallback myEmptyServerCB;

	ServiceServer * s1 = comm->startServerService(address1, & myEmptyServerCB);

	try{
		ServiceServer * s3 = comm->startServerService(address1,  & myEmptyServerCB );
		assert(s3 == nullptr);
	}catch(CommunicationModuleException & e){
		// we expect that this server address is already occupied.
	}
	delete(s1);
}

void missingListener(CommunicationModule * comm, string address1, string address2, string address3){
	cout << endl << "Try to connect to a port which does not exist, so we should retry connecting" << endl;
	MyConnectionCallback myCCB;
	MyClientMessageCallback myMessageCB;

	ServiceClient * c1 = comm->startClientService(address3, & myCCB, & myMessageCB);
	
	assert(c1 != nullptr);

	while(myCCB.retries < 2){
		myCCB.waitUntilSthHappened();
	}
	delete(c1);
}

void rejectingListener(CommunicationModule * comm, string address1, string address2, string address3){
	EmptyServerCallback myEmptyServerCB;
	MyConnectionCallback myCCB;
	MyClientMessageCallback myMessageCB;

	cout << endl <<  "Connect to a server which rejects all messages " << endl;
	ServiceServer * s2 = comm->startServerService(address2, & myEmptyServerCB );

	ServiceClient * c2 = comm->startClientService(address2, & myCCB, & myMessageCB);

	myCCB.waitUntilSthHappened();

	// produce an error because the target message type is not registered.
	string data = "test";

	c2->isend(& data);

	myMessageCB.waitUntilSthHappened();
	if (myMessageCB.state == MyClientMessageCallback::State::MessageSend){
		// wait for receiving the answer
		myMessageCB.waitUntilSthHappened();
	}
	
	assert(myMessageCB.state == MyClientMessageCallback::State::Error);
	assert(myMessageCB.error == CommunicationError::MESSAGE_TYPE_NOT_AVAILABLE);

	// block forever:
	//myMessageCB.waitUntilSthHappened();

	delete(s2);
	delete(c2);
}

void messageExchange(CommunicationModule * comm, string address1, string address2, string address3){
	MyConnectionCallback myCCB;
	MyClientMessageCallback myMessageCB;

	cout << endl <<  "Simple message exchange" << endl;

	MyServerCallback mySCB;	
	ServiceServer * s3 = comm->startServerService(address2, & mySCB );

	ServiceClient * c2 = comm->startClientService(address2, & myCCB, & myMessageCB);

	myCCB.waitUntilSthHappened();
	
	string data = "test";
	// register a message type on the server and re-send the previous message:
	c2->isend(& data);

	mySCB.waitUntilSthHappened();
	myMessageCB.waitUntilSthHappened();

	// check if the message has been received
	assert(mySCB.messagesReceived == 1);
	// check if the message has been send properly
	assert(myMessageCB.state == MyClientMessageCallback::State::MessageSend);

	// send the response
	string response("response");
	mySCB.lastMessage->isendResponse(& response);

	myMessageCB.waitUntilSthHappened();

	// check if the message response has been properly received
	assert(myMessageCB.state == MyClientMessageCallback::State::MessageResponseReceived);
	assert(myMessageCB.response == response);

	// close connections:
	delete(c2);
	delete(s3);	
}


void runTestSuite(string module, string address1, string address2, string address3){
	CommunicationModule * comm = core::module_create_instance<CommunicationModule>( "", module, CORE_COMM_INTERFACE );

	assert(comm != nullptr);
	comm->init();

	doubleListenerError(comm, address1, address2, address3);
	missingListener(comm, address1, address2, address3);
	rejectingListener(comm, address1, address2, address3);
	messageExchange(comm, address1, address2, address3);

	delete(comm);
}
