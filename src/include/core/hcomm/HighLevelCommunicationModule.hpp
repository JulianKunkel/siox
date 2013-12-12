#ifndef CORE_HCOMM_HIGHLEVELCOMMUNICATION_HPP
#define CORE_HCOMM_HIGHLEVELCOMMUNICATION_HPP

/* 
	This client and server classes wraps an existing communication module to offer:
	1) Multiple addresses -- redundant endpoints, if one address is unavailable, another endpoint is connected. 
		Different endpoints are separated by a "," in the address string.
	2) Multiplexing of connections to one connection endpoint. Thus, multiple services can share one logical host address -- they are distinguished by a "#<SERVICENAME>" after the regular name.
	3) An asynchronous reply-less send is offered, which completes immediately after data is enqueued without invoking any callback. 
	4) Flow-control: If the network is too slow to distribute pending messages, new messages are discarded and the ErrorCallback of the MessageType is invoked (by the calling thread).
		2.a) If no endpoint is available, all message transfer is paused, i.e. best-effort messages are discarded.
		2.b) Allows to control SIOX overhead: If the system network is too occupied, we stall transmission of messages.
	5) Multi-message type support: different message types can be send/received.	
 	*/


#include <map>

#include <core/hcomm/HighLevelCommunicationModule.hpp>

/*
 * This class multiplexes the underlying network service for different message types.
 * A message is classified by an integer type ID.
 */

namespace core{

class HLServerCallback{
public:
	// This function must also de-serialize the incoming message payload.
	// It allows to perform a non-blocking RPC by delegating the response (in form of the shared ptr) to another thread.
	virtual void messageReceivedCB(std::shared_ptr<ServerClientMessage> msg, const char * message_data, uint64_t buffer_size) = 0;

	virtual uint64_t serializeResponseMessageLen(const ServerClientMessage * msg, const void * responseType){ 
		return 0; 
	};
	virtual void serializeResponseMessage(const ServerClientMessage * msg, const void * responseType, char * buffer, uint64_t & pos){};

	virtual ~HLServerCallback(){};
};

class HLServerCallbackUnidirectional{
public:
	// This function must de-serialize the incoming message payload.
	virtual void messageReceivedCB(const char * message_data, uint64_t data_size) = 0;

	virtual ~HLServerCallbackUnidirectional(){}
};



class HLCommServiceServer{
public:
	void registerMessageCallback(uint32_t mtype, HLServerCallback * msg_rcvd_callback);
	void registerMessageCallback(uint32_t mtype, HLServerCallbackUnidirectional * msg_rcvd_callback);

	~HLCommServiceServer();
};


class HLUnidirectionalMessageHandler{
public:
	// Data loss is handled in a similar fashion for all message types.
	virtual uint64_t serializeMessageLen(const void * msgObject)  = 0;
	virtual void serializeMessage(const void * msgObject, char * buffer, uint64_t & pos) = 0;

	virtual ~HLUnidirectionalMessageHandler(){};
};

class HLRPCMessageHandler{
public:
	virtual void * deserializeResponse(char * buffer, uint64_t buffer_size) = 0;

	virtual uint64_t serializeMessageLen(const void * msgObject)  = 0;
	virtual void serializeMessage(const void * msgObject, char * buffer, uint64_t & pos) = 0;

	virtual ~HLRPCMessageHandler(){};
};

class HLCommServiceClient{
public:
	~HLCommServiceClient();


	/* Send data without waiting for a response, this messageType must not be a "messageWithResponse" */
	void isend( uint32_t mtype, void * object);

	/* Returns the created response object, this messageType must be a messageWithResponse.
		Ownership of the created response object is given to the user.
		If an error occurs during transmission which cannot be fixed it throws an exception. 
	 */
	//void * performRPC( uint32_t mtype, void * object ) throw (CommunicationModuleException);

	//void registerRPCMessage(uint32_t mtype, HLRPCMessageHandler * handler);
	void registerUnidirectionalMessage(uint32_t mtype, HLUnidirectionalMessageHandler * handler);
};


class HLCommunicationModule : public core::Component{
public:
	virtual HLCommServiceServer * startServerService(const string & address) throw(CommunicationModuleException) = 0;

	virtual HLCommServiceClient * startClientService(const string & serverAddressList) throw(CommunicationModuleException) = 0;
};

}

#define CORE_HCOMM_INTERFACE "core_hcomm"
#define CORE_HCOMM_INSTANCIATOR_NAME MODULE_INSTANCIATOR_NAME(core_hcomm)


#endif
