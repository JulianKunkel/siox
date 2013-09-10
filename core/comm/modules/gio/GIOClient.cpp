#include <core/comm/ServiceClient.hpp>
#include <core/comm/Message.hpp>

class GIOClient : public ServiceClient{
public:	
	string address;

	GIOClient(const string & address){
		this->address = address;
	}

	void ireconnect(){
		//if (server == nullptr){
			// we have an error here!
		//	connectionCallback->connectionErrorCB(*this, ConnectionError::SERVER_NOT_ACCESSABLE);
		//	return;
		//}

		connectionCallback->connectionSuccessfullCB(*this);
	}

	const string & getAddress() const{
		return this->address;
	}

	void isend( std::shared_ptr<CreatedMessage> msg ){
		GIOServiceServer * server = servers[address];
		if (server == nullptr){
			// we have an error here!
			msg->mcb.messageTransferErrorCB(this, msg, ConnectionError::SERVER_NOT_ACCESSABLE);
			return;
		}

		// notify server about reception of message
		if (server->callbacks.find(msg->type) != server->callbacks.end()){
			ServerCallback * cb = server->callbacks[msg->type];			
			cb->messageReceivedCB(std::shared_ptr<InMemoryServerConnection>(new InMemoryServerConnection(this, msg)), msg);
		}else{
			msg->mcb.messageTransferErrorCB(this, msg, ConnectionError::MESSAGE_TYPE_NOT_AVAILABLE);
			return;
		}

		// message completion
		msg->mcb.messageSendCB(msg);
	}

	~GIOClient(){
		cout << "Shutting down client \"" << getAddress() << "\"" << endl;
	}	
};

