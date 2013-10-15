#ifndef CORE_COMM_HIGHLEVEL_COMM_HPP
#define CORE_COMM_HIGHLEVEL_COMM_HPP

#include <core/comm/CommunicationModule.hpp>

using namespace std;

/* 
	This high-level client and server classes wrap an existing communication module to offer:
	1) Multiple addresses -- redundant endpoints, if one address is unavailable, another endpoint is connected. 
		Different endpoints are separated by a "," in the address string.
	2) Multiplexing of connections to one connection endpoint. Thus, multiple services can share one logical host address -- they are distinguished by a "#<SERVICENAME>" after the regular name.
	3) An asynchronous reply-less send is offered, which completes immediately after data is enqueued without invoking any callback. 
	4) Client-side flow-control: If the network is too slow to distribute pending messages, new messages are discarded and the ErrorCallback of the MessageType is invoked (by the calling thread).
		2.a) If no endpoint is available, all message transfer is paused, i.e. best-effort messages are discarded.
		2.b) Allows to control SIOX overhead: If the system network is too occupied, we stall transmission of messages.
	5) Multi-message type support: different message types can be send/received.	
 	*/

namespace core{

class HighLevelServerCallback{
public:
	// This function must also de-serialize the incoming message payload.
	// It allows to perform a non-blocking RPC by delegating the response (in form of the shared ptr) to another thread.
	virtual void messageReceivedCB(std::shared_ptr<ServerClientMessage> msg, const char * message_data, uint64_t buffer_size) = 0;

	virtual uint64_t serializeResponseMessageLen(const ServerClientMessage * msg, const void * responseType){};
	virtual void serializeResponseMessage(const ServerClientMessage * msg, const void * responseType, char * buffer, uint64_t & pos){};

	virtual ~HighLevelServerCallback(){};
};

class HighLevelServerCallbackUnidirectional{
public:
	// This function must de-serialize the incoming message payload.
	virtual void messageReceivedCB(const char * message_data, uint64_t data_size) = 0;

	virtual ~HighLevelServerCallbackUnidirectional(){}
};



class HighLevelCommunicationServiceServer{
public:
	void registerMessageCallback(uint32_t mtype, HighLevelServerCallback * msg_rcvd_callback);
	void registerMessageCallback(uint32_t mtype, HighLevelServerCallbackUnidirectional * msg_rcvd_callback);

	~HighLevelCommunicationServiceServer();
};


class HighLevelUnidirectionalMessageHandler{
public:
	// Data loss is handled in a similar fashion for all message types.
	virtual uint64_t serializeMessageLen(const void * msgObject)  = 0;
	virtual void serializeMessage(const void * msgObject, char * buffer, uint64_t & pos) = 0;

	virtual ~HighLevelUnidirectionalMessageCallback(){};
};

class HighLevelRPCMessageHandler{
public:
	virtual void * deserializeResponse(char * buffer, uint64_t buffer_size) = 0;

	virtual uint64_t serializeMessageLen(const void * msgObject)  = 0;
	virtual void serializeMessage(const void * msgObject, char * buffer, uint64_t & pos) = 0;

	virtual ~HighLevelRPCMessageCallback(){};
};

class HighLevelCommunicationServiceClient{
public:
	~HighLevelCommunicationServiceClient();


	/* Send data without waiting for a response, this messageType must not be a "messageWithResponse" */
	void isend( uint32_t mtype, void * object);

	/* Returns the created response object, this messageType must be a messageWithResponse.
		Ownership of the created object is given to the user.
		If an error occurs during transmission which cannot be fixed it throws an exception. 
	 */
	void * performRPC( uint32_t mtype, void * object ) throw (CommunicationModuleException);

	void registerRPCMessage(uint32_t mtype, HighLevelRPCMessageHandler * handler);
	void registerUnidirectionalMessage(uint32_t mtype, HighLevelUnidirectionalMessageHandler * handler);
};


class HighLevelCommunicationCommunication{
public:
	virtual HighLevelCommunicationTypeServiceServer * startServerService(const string & address) throw(CommunicationModuleException) = 0;

	virtual HighLevelCommunicationTypeServiceClient * startClientService(const string & serverAddressList) throw(CommunicationModuleException) = 0;
};

}

#endif