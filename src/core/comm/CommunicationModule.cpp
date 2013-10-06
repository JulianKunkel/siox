#include <core/comm/CommunicationModule.hpp>

namespace core{
ServiceServer * CommunicationModule::startServerService(const string & address, ServerCallback * msg_rcvd_callback, ProcessorQueue * sendQueue) throw(CommunicationModuleException){
	assert(msg_rcvd_callback);
	ServiceServer * server = startServerService(address, sendQueue);
	server->setMessageCallback(msg_rcvd_callback);
	try{
		server->listen();
	}catch(CommunicationModuleException & e){
		delete(server);
		throw;
	}
	
	return server;
}

ServiceClient * CommunicationModule::startClientService(const string & server_address, ConnectionCallback * ccb, MessageCallback * messageCallback, ProcessorQueue * sendQueue) throw(CommunicationModuleException){
	ServiceClient * client = startClientService(server_address, sendQueue);

	client->setConnectionCallback(ccb);
	client->setMessageCallback(messageCallback);

	client->ireconnect();
	return client;
}
}