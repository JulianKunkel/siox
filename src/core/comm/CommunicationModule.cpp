#include <core/comm/CommunicationModule.hpp>

#include <util/DefaultProcessorQueues.hpp>


namespace core{
ServiceServer * CommunicationModule::startServerService(const string & address, ServerCallback * msg_rcvd_callback) throw(CommunicationModuleException){
	assert(msg_rcvd_callback);
	ServiceServer * server = startServerService(address);
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
	if (sendQueue == nullptr){
		sendQueue = new FIFOProcessorQueue();
	}

	ServiceClient * client = startClientService(server_address, sendQueue);

	client->setConnectionCallback(ccb);
	client->setMessageCallback(messageCallback);

	client->ireconnect();
	return client;
}
}