#include <core/comm/ServiceServer.hpp>

class GIOServiceServer;

extern "C"{
static void accept_thread_func(GIOServiceServer * server);
static void response_thread_func(GIOServiceServer * server);
}

class TCPMessage : public Message{
public:
	GSocketConnection * connection;
	uint32_t clientSidedID;

	TCPMessage(uint32_t clientSidedID, GSocketConnection* connection,  uint32_t type, const char * payload, uint64_t size) : Message(type, payload, size), connection(connection), clientSidedID(clientSidedID) {}	

	static TCPMessage * readMessage(GSocketConnection* connection){
	    GInputStream * istream = g_io_stream_get_input_stream(G_IO_STREAM(connection));

	    gsize messageSizeRead = 0;
	    uint32_t messageType;
	    uint64_t messageLength;
	    uint32_t clientSidedID;
	    void* messageBuffer;
	    gboolean ret = FALSE;

		// Read type
        ret =  g_input_stream_read_all(istream, & messageLength, sizeof(uint64_t), &messageSizeRead, NULL, NULL);   
        if(!ret || messageSizeRead == 8) { 
        	// TODO send an error message back
            return nullptr;
        }
        
        // Read length
        ret =  g_input_stream_read_all(istream, & messageType, sizeof(uint32_t), &messageSizeRead, NULL, NULL);   
        if(!ret || messageSizeRead != 4) { 
        	// TODO send an error message back
            return nullptr;
        }

        // Read clientSidedID
        ret =  g_input_stream_read_all(istream, & clientSidedID, sizeof(uint32_t), &messageSizeRead, NULL, NULL);   
        if(!ret || messageSizeRead != 4) { 
        	// TODO send an error message back
            return nullptr;
        }

        char * message = (char*) malloc(messageLength);

                
        // Read message
        ret =  g_input_stream_read_all(istream, message, messageLength, &messageSizeRead, NULL, NULL);   
        if(!ret || messageSizeRead != messageLength ) { 
        	// TODO send an error message back
    		return nullptr;
        }
        messageBuffer = (void*) message;

        return new TCPMessage(clientSidedID, connection, messageType, message, messageLength);
	}

	void send(GSocketConnection* connection){

	    gboolean ret = FALSE;
	    gsize bytes_written;

	    GOutputStream * ostream = g_io_stream_get_output_stream(G_IO_STREAM(connection));

        // Read type
        ret =  g_output_stream_write_all(ostream, & size, sizeof(uint64_t), & bytes_written,  NULL, NULL);   
        if(!ret) { 
        	// TODO send an error message back
            return;
        }
        
        // Read length
        ret =  g_output_stream_write_all(ostream, & type, sizeof(uint32_t), & bytes_written, NULL, NULL);   
        if(!ret) { 
        	// TODO send an error message back
            return;
        }

        // Read clientSidedID
        ret =  g_output_stream_write_all(ostream, & clientSidedID, sizeof(uint32_t), & bytes_written, NULL, NULL);   
        if(!ret) { 
        	// TODO send an error message back
            return;
        }

                
        // Read message
        ret =  g_output_stream_write_all(ostream, payload, size, & bytes_written, NULL, NULL);   
        if(!ret) { 
        	// TODO send an error message back
    		return;
        }
	}
};


class GIOServiceServer : public ServiceServer{
public:
	map<uint32_t, ServerCallback*> callbacks;

	GSocketListener * listener;
	GCancellable * cancelable;

	void advertise(uint32_t mtype ){
		assert(false);
	}

	void ipublish( std::shared_ptr<CreatedMessage> msg ){
		assert(false);
	}

	void register_message_callback(uint32_t mtype, ServerCallback * msg_rcvd_callback){
		callbacks[mtype] = msg_rcvd_callback;
	}

	void unregister_message_callback(uint32_t mtype){
		callbacks.erase(mtype);
	}

	void isend( std::shared_ptr<Message> msg, std::shared_ptr<CreatedMessage> response ){
		TCPMessage* tmsg = ((TCPMessage*) &*msg);
		GSocketConnection* connection = tmsg->connection;

	}

	void responseThreadFunc(){
		// TODO add queues etc.
	}

	void acceptThreadFunc(){
		GError * error = nullptr;
		GSocketConnection * connection =  g_socket_listener_accept(server->listener, nullptr, server->cancelable, &error);
		if ( g_cancellable_is_cancelled (server->cancelable))
			return;

		// spawn a response thread
		thread responseThread(response_thread_func, server);
		responseThread.detach();

		// spawn another thread to accept the next connection
		{
		thread clientThread(accept_thread_func, server);
		clientThread.detach();
		}

	    cout << "Incomming connection" << endl;

	    while(1) {

	        shared_ptr<Message> msg(TCPMessage::readMessage(connection));
			if ( callbacks.find(msg->type) != callbacks.end() ){
				callbacks[msg->type]->messageReceivedCB(this,msg);
			}else{
				// TODO return an error response to the client
			}
	    }
	}

	GIOServiceServer(const string & address, uint16_t port) throw(CommunicationModuleException){
		listener = g_socket_listener_new();
		cancelable =  g_cancellable_new();
		if(listener == nullptr ){
			throw CommunicationModuleException("Could not instantiate listener");
		}

		GError * error = NULL;
		GInetAddress* localhost = g_inet_address_new_any(G_SOCKET_FAMILY_IPV4);
		if (! localhost) {
		    localhost = g_inet_address_new_any(G_SOCKET_FAMILY_IPV6);
		}
		GSocketAddress * sa = g_inet_socket_address_new(localhost, port);
		GSocketAddress * real_addr;
		if(! g_socket_listener_add_address(listener, sa, G_SOCKET_TYPE_STREAM, G_SOCKET_PROTOCOL_TCP, NULL, & real_addr, & error)){
			
			g_object_unref(listener);
			g_object_unref(cancelable);

			throw CommunicationModuleException(error->message);
		}

		thread clientThread(accept_thread_func, this);
    	clientThread.detach();		
	}

	~GIOServiceServer(){
		cout << "Shutting down server" << endl;
		g_cancellable_cancel(cancelable);

		g_object_unref(listener);
		g_object_unref(cancelable);
	}
};

static void response_thread_func(GIOServiceServer * server){
	server->responseThreadFunc();
}

static void accept_thread_func(GIOServiceServer * server){
	server->acceptThreadFunc();
}

