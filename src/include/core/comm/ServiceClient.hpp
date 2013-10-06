/**
 * ServiceClient provides an interface to communicate with a ServiceServer.
 *
 */

#ifndef CORE_SERVICE_CLIENT_H
#define CORE_SERVICE_CLIENT_H

#include <exception>
#include <string>

#include <core/comm/Message.hpp>

using namespace std;

namespace core{


class ServiceClient;
class CommunicationModule;

class ConnectionCallback{
public:
	virtual void connectionErrorCB(ServiceClient & client, CommunicationError error){}
	virtual void connectionSuccessfullCB(ServiceClient & client){}
};

class MessageCallback{
public:
	virtual void messageSendCB(BareMessage * msg) = 0;

	// this function de-serializes the buffer as well
	virtual void messageResponseCB(BareMessage * msg, char * buffer, uint64_t buffer_size){};

	virtual void messageTransferErrorCB(BareMessage * msg, CommunicationError error) = 0;

	virtual uint64_t serializeMessageLen(const void * msgObject)  = 0;
	virtual void serializeMessage(const void * msgObject, char * buffer, uint64_t & pos) = 0;

	virtual ~MessageCallback(){};
};

class ServiceClient{
protected:
	ConnectionCallback * connectionCallback;
	MessageCallback * messageCallback;
public:
		void setConnectionCallback(ConnectionCallback * ccb){
			this->connectionCallback = ccb;
		}

		void setMessageCallback(MessageCallback * mcb){
			messageCallback = mcb;
		}


		inline ConnectionCallback * getConnectionCallback(){
			return connectionCallback;
		}

		inline MessageCallback * getMessageCallback(){
			return messageCallback;
		}


		/**
	  	 * Try to reconnect asynchronously and resend all currently pending messages. 
	  	 */
		virtual void ireconnect() = 0;

		/**
		 * Return the address of the target communication endpoint.
		 */
		virtual const string & getAddress() const = 0 ;

		// This method sends a correctly serialized message.
		virtual void isend( BareMessage * msg ) = 0;


		// the number of bytes required for the header of any message!
		virtual uint32_t headerSize() = 0;
		virtual void serializeHeader(char * buffer, uint64_t & pos, uint64_t size) = 0;

		virtual ~ServiceClient(){}


		/**
		 * The object is serialized using the messageCallack.
		 * @note This function may return immediately.
		 */
		 virtual BareMessage * isend(const void * object, const void * user_ptr = nullptr ){
			uint64_t msg_size = headerSize() + messageCallback->serializeMessageLen(object);
			char * payload = (char*) malloc(msg_size);

			assert(payload);

			uint64_t pos = 0;

			serializeHeader(payload, pos, msg_size);
			assert(headerSize() == pos);

			messageCallback->serializeMessage(object, payload, pos);
			assert(pos == msg_size);

			BareMessage * msg = new BareMessage(payload, msg_size, user_ptr);

			isend(msg);
			return msg;
		}		
};

}

#endif
