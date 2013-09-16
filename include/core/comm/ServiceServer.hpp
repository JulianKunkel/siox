#ifndef CORE_SERVICE_SERVER_H
#define CORE_SERVICE_SERVER_H

#include <memory> 

#include <core/comm/Message.hpp>

namespace core{

// A message from a client which is received by a server.
// The message callbacks are invoked on the server side
class ServerClientMessage : public BareMessage{
public:	
		// after a response is queued the message is freed !
		virtual void isendResponse(void * object) = 0;
		virtual void isendErrorResponse(CommunicationError error) = 0;

		ServerClientMessage(const char * payload, uint64_t size) : BareMessage(payload, size){}	
};


class ServiceServer;

class ServerCallback{
public:
	// This function also de-serializes the incoming data stream of the message.
	virtual void messageReceivedCB(std::shared_ptr<ServerClientMessage> msg, const char * message_data, uint64_t buffer_size) = 0;

	// After a response has been sent, the client message and response are destroyed.
	virtual void responseSendCB(BareMessage * response){};

	// After a response has been sent, the client message and response are destroyed.
	virtual void invalidMessageReceivedCB(CommunicationError error){};

	virtual uint64_t serializeResponseMessageLen(const ServerClientMessage * msg, const void * responseType) = 0;
	virtual void serializeResponseMessage(const ServerClientMessage * msg, const void * responseType, char * buffer, uint64_t & pos) = 0;

	virtual ~ServerCallback(){};
};



class CommunicationModule;

class ServiceServer{
	protected:
		ServerCallback * messageCallback;
	public:
		virtual void listen() throw(CommunicationModuleException) = 0;

		inline ServerCallback * getMessageCallback(){
			return messageCallback;
		}
		
		/** Advertises a new client among all connected clients. */
		// virtual void advertise(uint32_t mtype) = 0;

		/** Sends the message to all clients who subscribed the message's response type */
		virtual void ipublish( void * object ) = 0;

		virtual ~ServiceServer(){}

		virtual uint32_t headerSizeClientMessage() = 0;

		void setMessageCallback(ServerCallback * msg_rcvd_callback){
			messageCallback = msg_rcvd_callback;
		}
};





}

#endif

