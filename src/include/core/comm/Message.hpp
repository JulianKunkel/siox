#ifndef CORE_CONNECTION_MESSAGE_H
#define CORE_CONNECTION_MESSAGE_H

#include <memory> 

#include <core/container/container-binary-serializer.hpp>

namespace core{


enum class CommunicationError : uint8_t{
	SUCCESS = 0,
	CONNECTION_LOST = 1,
	SERVER_NOT_ACCESSABLE = 2,
	MESSAGE_TYPE_NOT_AVAILABLE = 3,
	MESSAGE_INCOMPATIBLE = 4,
	MESSAGE_DAMAGED = 5,
	INVALID_ADDRESS = 6,
	UNKNOWN = 255
};

class CommunicationModuleException : public std::exception {
private:
	const string err_msg_;

public:
	CommunicationModuleException( const string & err_msg ) : err_msg_( err_msg ), error(CommunicationError::UNKNOWN) {}
	CommunicationModuleException( const string & err_msg, CommunicationError error) : err_msg_( err_msg ), error(error) {}
	const char * what() const throw() {
	   return err_msg_.c_str();
	}
	const CommunicationError error;
};

class BareMessage{
	public:
		// message size
		uint64_t size;

		// message payload includes only the raw user data.
		const char * payload;

		// information associated with this message
		const void * user_ptr;

		// the ownership of the payload is given to the connection message.
		BareMessage(const char * payload, uint64_t size, const void * user_ptr = nullptr) : size(size) , payload(payload), user_ptr(user_ptr){}

		BareMessage(BareMessage && msg): size(msg.size) , payload(msg.payload), user_ptr(msg.user_ptr){ 
			msg.payload = nullptr;
		}

		BareMessage(const BareMessage & msg) : size(msg.size), payload((char*) malloc(msg.size)){ 
			memcpy((void*) payload, msg.payload, msg.size);
		}

		// remove assignment operator
    	BareMessage & operator=(const BareMessage&) = delete;    	

		virtual ~BareMessage(){
			if(payload != nullptr){
				free((void*) payload);
			}
		}
};


}

#endif
