#ifndef CORE_CONNECTION_MESSAGE_H
#define CORE_CONNECTION_MESSAGE_H

#include <memory> 

#include <core/comm/Connection.hpp>

namespace core{

class Message;
class CreatedMessage;

class MessageCallback{
public:
	virtual void messageSendCB(std::shared_ptr<CreatedMessage> msg) = 0;
	virtual void messageResponseCB(std::shared_ptr<CreatedMessage> msg, std::shared_ptr<Message> response){};
	virtual void messageTransferErrorCB(std::shared_ptr<CreatedMessage> msg, ConnectionError error) = 0;

	virtual ~MessageCallback(){};
};


class Message{
	public:
		// message type / or response
		uint32_t type;

		// message size
		uint64_t size;

		// payload of the message
		const char * payload;

		// the ownership of the payload is given to the connection message.
		Message(uint32_t type, const char * payload, uint64_t size) : type(type), size(size) , payload(payload){}

		~Message(){
			if(payload != nullptr){
				free(nullptr);
			}
		}
};

class CreatedMessage : public Message{
public:	
		// the local message callback to invoke if the message transfer fails or completes.
		MessageCallback & mcb;

		CreatedMessage(MessageCallback & mcb,  uint32_t type, const char * payload, uint64_t size) : Message(type, payload, size), mcb(mcb) {}		
};


}

#endif
