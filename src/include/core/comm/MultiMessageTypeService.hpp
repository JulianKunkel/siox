#ifndef CORE_COMM_MULTIMESSAGE_HPP
#define CORE_COMM_MULTIMESSAGE_HPP

#include <map>

#include <core/comm/ServiceServer.hpp>
#include <core/comm/ServiceClient.hpp>

/*
 * This class multiplexes the underlying network service for different message types.
 * A message is classified by an integer type ID.
 */

namespace core{
class MultiMessageServerCallback;
class MultiMessageCommunication;

class MultiMessageTypeServiceServer{
public:
	void registerMessageCallback(uint32_t mtype, ServerCallback * msg_rcvd_callback);

	~MultiMessageTypeServiceServer();
private:
	friend MultiMessageServerCallback;
	friend MultiMessageCommunication;

	MultiMessageTypeServiceServer(CommunicationModule * module, const string & address)  throw(CommunicationModuleException) ;

	std::map<uint32_t, ServerCallback*> callbacks;
	
	ServiceServer * backend;
	MultiMessageServerCallback * cb;

	uint32_t getClientMessageType(const BareMessage * msg);
};

class MultiMessageClientCallback;

class MultiMessageTypeServiceClient{
public:
	~MultiMessageTypeServiceClient();

	BareMessage * isend( uint32_t mtype, void * object );

	void registerMessageCallback(uint32_t mtype, MessageCallback * msgCallback);

	void ireconnect(){
		backend->ireconnect();
	}

	const string & getAddress() const{
		return backend->getAddress();
	}

private:
	friend MultiMessageClientCallback;
	friend MultiMessageCommunication;

	MultiMessageTypeServiceClient(CommunicationModule * module, const string & server_address, ConnectionCallback * ccb)  throw(CommunicationModuleException) ;

	std::map<uint32_t, MessageCallback*> callbacks;

	uint32_t getType(const BareMessage * msg);

	ServiceClient * backend;
	MultiMessageClientCallback * cb;
};


class MultiMessageCommunication{
public:
	MultiMessageCommunication(CommunicationModule * backend) : backend(backend){}

	MultiMessageTypeServiceServer * startServerService(const string & address) throw(CommunicationModuleException){
		return new MultiMessageTypeServiceServer(backend, address);
	}

	MultiMessageTypeServiceClient * startClientService(const string & server_address, ConnectionCallback * ccb) throw(CommunicationModuleException){
		return new MultiMessageTypeServiceClient(backend, server_address, ccb);
	}

private:
	CommunicationModule * backend;
};

}

#endif
