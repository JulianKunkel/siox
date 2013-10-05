#include "GIOServer.hpp"

#include "GIOinternal.hpp"


class GIOServiceServer;

class ServerMessageSendQueue : public MessageSendQueue{
private:
	GIOServiceServer * server;
protected:
	virtual void messageSend(BareMessage * response){
		server->getMessageCallback()->responseSendCB(response);
		delete(response);
	}
public:
	ServerMessageSendQueue(GIOServiceServer * server): server(server){}
};


class TCPClientMessage : public ServerClientMessage{
public:
	MessageSendQueue * rspThread;
	GIOServiceServer * server;
	uint64_t clientSidedID;

	TCPClientMessage(GIOServiceServer * server, uint64_t clientSidedID, MessageSendQueue* rspThread,  const char * payload, uint64_t size) : ServerClientMessage( payload, size), rspThread(rspThread), server(server), clientSidedID(clientSidedID) {}	

	void isendResponse(const void * object);

	void isendErrorResponse(CommunicationError error);
};

void TCPClientMessage::isendResponse(const void * object){
	uint64_t msg_size = clientMsgHeaderLen() + server->messageCallback->serializeResponseMessageLen(this, object);
	char * payload = (char*) malloc(msg_size);
	uint64_t pos = 0;
	serializeMsgHeader(payload, pos, msg_size, MAGIC_KEY, clientSidedID);

	server->messageCallback->serializeResponseMessage(this,  object, payload, pos);

	rspThread->enqueue(new BareMessage(payload, msg_size));
}

void TCPClientMessage::isendErrorResponse(CommunicationError error){
	uint64_t msg_size = clientMsgHeaderLen();
	char * payload = (char*) malloc(msg_size);
	uint64_t pos = 0;

	// send the error instead of the message size!
	serializeMsgHeader(payload, pos, (uint64_t) error, MAGIC_ERROR_KEY, clientSidedID);

	rspThread->enqueue(new BareMessage(payload, msg_size));
}


/// GIO SERVICE SERVER

void GIOServiceServer::ipublish( void * object ){
	assert(false);
}

uint32_t GIOServiceServer::headerSizeClientMessage(){
	return clientMsgHeaderLen();
}

void GIOServiceServer::addAcceptorThread(){
	unique_lock<mutex> lk(pendingClientThreadMutex);

	uint64_t threadID = lastThreadID++;

    GCancellable * one_thread_cancelable = g_cancellable_new();
	thread * threadPointer = new thread(& GIOServiceServer::acceptThreadFunc, this, threadID, one_thread_cancelable);
	pendingClientThreads[threadID] = {threadPointer, one_thread_cancelable};
}

void GIOServiceServer::removeAcceptorThread(uint64_t threadID){
	unique_lock<mutex> lk(pendingClientThreadMutex);
	auto itr = pendingClientThreads.find(threadID);

	cleanTerminatedThread();

	if (itr != pendingClientThreads.end()){
		auto var = itr->second;

	    g_object_unref(var.second);
		finishedThread = var.first;

		pendingClientThreads.erase(itr);
	}

	if( pendingClientThreads.size() == 0){
		allThreadsFinished.notify_one();
	}
}

void GIOServiceServer::cleanTerminatedThread(){
	if(finishedThread == nullptr){
		return;
	}
	finishedThread->join();
	delete(finishedThread);
	finishedThread = nullptr;
}

void GIOServiceServer::acceptThreadFunc(uint64_t threadID, GCancellable * one_thread_error_cancelable){
	GSocketConnection * connection;

	while( (connection =  g_socket_listener_accept(listener, nullptr, cancelable, nullptr)) == nullptr ){

		if ( g_cancellable_is_cancelled (cancelable)){
			if(connection){
				g_object_unref(connection);
			}
			removeAcceptorThread(threadID);
			return;
		}
	}

    // cout << "Incoming connection" << endl;

    GInputStream * istream = g_io_stream_get_input_stream(G_IO_STREAM(connection));

    // this spawns a response thread
    ServerMessageSendQueue messageSendQueueInstance(this);

    messageSendQueueInstance.connect(g_io_stream_get_output_stream(G_IO_STREAM(connection)), one_thread_error_cancelable);

	// spawn another thread to accept the next connection
	addAcceptorThread();

    while(1) {
		uint64_t msgLength;
		uint32_t clientSidedID;
		CommunicationError error;

		char * payload = readSocketMessage(istream, msgLength, clientSidedID, error, one_thread_error_cancelable);

        if ( payload == nullptr ){
        	if (msgLength > 0){ 
        		cout << "Received broken message" << endl;

        		messageCallback->invalidMessageReceivedCB(error);
        		TCPClientMessage errMsg(this, clientSidedID, & messageSendQueueInstance, nullptr, 0);
        		errMsg.isendErrorResponse(error);
        	}

        	break;
        }

        cout << "Received message" << endl;

        auto msg = shared_ptr<TCPClientMessage>(new TCPClientMessage(this, clientSidedID, & messageSendQueueInstance, payload, msgLength));

        messageCallback->messageReceivedCB(msg, payload + clientMsgHeaderLen(), msgLength - clientMsgHeaderLen());
    }

    messageSendQueueInstance.terminate();

    g_object_unref(connection);

    removeAcceptorThread(threadID);
}

void GIOServiceServer::listen() throw(CommunicationModuleException){
	assert(listener == nullptr);

	listener = g_socket_listener_new();
	if(listener == nullptr ){		
		throw CommunicationModuleException("Could not instantiate listener");
	}

	//cout << "listen on " << address << endl;


	SocketType type;	
	GSocketAddress * gsocketAddr = getSocket(address, type);

	GError * error = NULL;

	int ret = g_socket_listener_add_address(listener, gsocketAddr, type == SocketType::IPC ?  (GSocketType) SocketType::TCP : (GSocketType) type, G_SOCKET_PROTOCOL_DEFAULT, NULL, NULL, & error);

	g_object_unref(gsocketAddr);

	if (! ret){
		CommunicationModuleException e = CommunicationModuleException(error->message);
		g_error_free(error);
		throw e;
	}

	if(error != NULL){
		g_error_free(error);
	}

	addAcceptorThread();
}

GIOServiceServer::GIOServiceServer(const string & address) throw(CommunicationModuleException){
	cancelable =  g_cancellable_new();

	this->address = address;
}

GIOServiceServer::~GIOServiceServer(){
	cout << "Shutting down server" << endl;
	g_cancellable_cancel(cancelable);

	unique_lock<mutex>  lock(pendingClientThreadMutex);

	// stop all receiver threads
	for(auto itr = pendingClientThreads.begin(); itr != pendingClientThreads.end(); itr++){
		 g_cancellable_cancel(itr->second.second);
	}

	while(pendingClientThreads.size() != 0){
		allThreadsFinished.wait(lock);
	}

	cleanTerminatedThread();

	if (listener){
		g_object_unref(listener);
	}
	g_object_unref(cancelable);

	// remove unix socket.
	SocketType type;
	string hostname;
	uint16_t port;
	string ipcPath;
	splitAddressParts(address, type, hostname, port, ipcPath);

	if(ipcPath != ""){
		unlink (ipcPath.c_str());
	}

	cout << "Done server" << endl;
}
