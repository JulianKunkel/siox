#include <mutex>

#include <list>
#include <condition_variable>

#include <core/comm/ServiceServer.hpp>

class GIOServiceServer;
class ResponseThread;

class TCPClientMessage : public ServerClientMessage{
public:
	ResponseThread * rspThread;
	GIOServiceServer * server;
	uint32_t clientSidedID;

	TCPClientMessage(GIOServiceServer * server, uint32_t clientSidedID, ResponseThread* rspThread,  const char * payload, uint64_t size) : ServerClientMessage( payload, size), rspThread(rspThread), server(server), clientSidedID(clientSidedID) {}	

	static TCPClientMessage * readMessage(GInputStream * istream, GIOServiceServer * server, ResponseThread * rspThread){
	    gsize messageSizeRead = 0;
	    gboolean ret = FALSE;

	    const uint32_t header_length = clientMsgHeaderLen();
	    char * header = (char*) malloc(header_length);

		// Read header
        ret =  g_input_stream_read_all(istream, header, header_length, &messageSizeRead, NULL, NULL);   
        if(!ret || messageSizeRead == header_length) { 
            return nullptr;
        }
        // deserialize header
		uint32_t magicKey;
		uint32_t clientSidedID;
		uint64_t msgLength;
		uint64_t pos = 0;

        deserializeMsgHeader(header, pos, header_length, msgLength, magicKey, clientSidedID);

        char * message = (char*) realloc(header, msgLength);
        uint64_t toRead = msgLength - header_length;   

        ret =  g_input_stream_read_all(istream, message + header_length, toRead, & messageSizeRead, NULL, NULL);
        if(!ret || messageSizeRead != toRead ) { 
    		return nullptr;
        }

        return new TCPClientMessage(server, clientSidedID, rspThread, message, msgLength);
	}

	bool sendResponse(BareMessage * response, GOutputStream * ostream){
		gboolean ret = FALSE;
	    gsize bytes_written;

        ret =  g_output_stream_write_all(ostream, response->payload, response->size, & bytes_written, NULL, NULL);   
        if(!ret || bytes_written != response->size) { 
        	return false;
        }

        return true;
	}


	void serializeHeader(char * buffer, uint64_t & pos, uint64_t size){
		uint32_t magicKey = MAGIC_KEY;
		uint64_t msgLength = size;

		serializeMsgHeader(buffer, pos, msgLength, magicKey, clientSidedID);
	}	

	void isendResponse(void * object);

	void isendErrorResponse(CommunicationError error);
};

class ResponseThread{
public:
	GOutputStream * ostream;

	bool finished = false;

	// protect central data structures
	mutex m;
	condition_variable cv;
	
	list<pair<TCPClientMessage*, BareMessage*>> pendingMessages;

	ResponseThread(GOutputStream * ostream){
		this->ostream = ostream;
	}

	~ResponseThread(){
		g_object_unref(ostream);
	}

	void enqueueMessage(TCPClientMessage* msg, BareMessage * response){
		unique_lock<mutex> lk(m);
		pendingMessages.push_back({msg, response});
		if( pendingMessages.size() == 1){
			cv.notify_one();
		}
	}

	void responseThreadFunc(){
		while(true){
			pair<TCPClientMessage*, BareMessage*> msg;
			{
			unique_lock<mutex> lk(m);
			
			while(pendingMessages.size() == 0){
				if (finished ){
					return;
				}				
				cv.wait(lk);
			}
			if (finished ){
				return;
			}
			msg = pendingMessages.front();
			pendingMessages.pop_front();
			}
			msg.first->sendResponse(msg.second, ostream);
		}	
	}
};

class GIOServiceServer : public ServiceServer{
public:
	friend TCPClientMessage;

	GSocketListener * listener = nullptr;
	GCancellable * cancelable;
	string address;

	mutex pendingClientThreadMutex;
	condition_variable allThreadsFinished;
	uint64_t lastThreadID = 0;

	// map thread_id to the thread
	map<uint64_t, thread*> pendingClientThreads;

	void ipublish( void * object ){
		assert(false);
	}

	uint32_t headerSizeClientMessage(){
		return clientMsgHeaderLen();
	}

	void addAcceptorThread(){
		unique_lock<mutex> lk(pendingClientThreadMutex);

		uint64_t threadID = lastThreadID++;

		thread * threadPointer = new thread(& GIOServiceServer::acceptThreadFunc, this, threadID);
		pendingClientThreads[threadID] = threadPointer;		
	}

	void removeAcceptorThread(uint64_t threadID){
		unique_lock<mutex> lk(pendingClientThreadMutex);
		pendingClientThreads.erase(threadID);

		if( pendingClientThreads.size() == 0){
			allThreadsFinished.notify_one();
		}
	}

	void acceptThreadFunc(uint64_t threadID){
		cout << "acceptThreadFunc " << endl;
		GSocketConnection * connection;


		while( (connection =  g_socket_listener_accept(listener, nullptr, cancelable, nullptr)) == nullptr ){

			if ( g_cancellable_is_cancelled (cancelable)){
				removeAcceptorThread(threadID);
				return;
			}
		}

	    cout << "Incoming connection" << endl;

	    GOutputStream * ostream = g_io_stream_get_output_stream(G_IO_STREAM(connection));
	    GInputStream * istream = g_io_stream_get_input_stream(G_IO_STREAM(connection));

	    ResponseThread * responseThreadInstance = new ResponseThread(ostream);

		// spawn another thread to accept the next connection
		addAcceptorThread();

		// spawn a response thread
		thread responseThread( & ResponseThread::responseThreadFunc, responseThreadInstance);


	    while(1) {
	        TCPClientMessage * msg = TCPClientMessage::readMessage(istream, this, responseThreadInstance);
	        if ( msg == nullptr ){
	        		TCPClientMessage errMsg(this, 0, responseThreadInstance, nullptr, 0);
	        		errMsg.isendErrorResponse(CommunicationError::MESSAGE_INCOMPATIBLE);
	        	break;
	        }
	        messageCallback->messageReceivedCB(msg, msg->payload + clientMsgHeaderLen(), msg->size - clientMsgHeaderLen());
	    }

	    // terminate response thread
	    {
	    unique_lock<mutex> lk(responseThreadInstance->m);
	    responseThreadInstance->finished = true;
		}
	    responseThreadInstance->cv.notify_one();

	    responseThread.join();

	    g_object_unref(connection);

	    removeAcceptorThread(threadID);
	}

	void listen() throw(CommunicationModuleException){
		assert(listener == nullptr);

		listener = g_socket_listener_new();
		if(listener == nullptr ){
			throw CommunicationModuleException("Could not instantiate listener");
		}

		cout << "listen on " << address << endl;

		pair<string, uint16_t> addresses = splitAddress(address);

		GError * error = NULL;
		GInetAddress* localhost = g_inet_address_new_any(G_SOCKET_FAMILY_IPV4);
		if (! localhost) {
		    localhost = g_inet_address_new_any(G_SOCKET_FAMILY_IPV6);
		}

		GSocketAddress * sa = g_inet_socket_address_new(localhost, addresses.second);
		GSocketAddress * real_addr;
		if(! g_socket_listener_add_address(listener, sa, G_SOCKET_TYPE_STREAM, G_SOCKET_PROTOCOL_TCP, NULL, & real_addr, & error)){
			g_object_unref(sa);

			CommunicationModuleException e = CommunicationModuleException(error->message);
			g_error_free(error);
			throw e;
		}
		g_object_unref(real_addr);
		g_object_unref(sa);

		addAcceptorThread();
	}

	GIOServiceServer(const string & address) throw(CommunicationModuleException){
		cancelable =  g_cancellable_new();

		this->address = address;
	}

	~GIOServiceServer(){
		cout << "Shutting down server" << endl;
		g_cancellable_cancel(cancelable);

		unique_lock<mutex>  lock(pendingClientThreadMutex);
		while(pendingClientThreads.size() != 0){
			allThreadsFinished.wait(lock);
		}		

		g_object_unref(listener);
		g_object_unref(cancelable);
	}
};



void TCPClientMessage::isendResponse(void * object){
	uint64_t msg_size = clientMsgHeaderLen() + server->messageCallback->serializeResponseMessageLen(this, object);
	char * payload = (char*) malloc(msg_size);
	uint64_t pos = 0;

	serializeHeader(payload, pos, msg_size);

	server->messageCallback->serializeResponseMessage(this,  object, payload, pos);

	BareMessage * response = new BareMessage(payload, msg_size);
	rspThread->enqueueMessage(this, response);
}

void TCPClientMessage::isendErrorResponse(CommunicationError error){

}


