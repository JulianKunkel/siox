#include <mutex>
#include <list>
#include <condition_variable>

#include <core/comm/ServiceServer.hpp>

class GIOServiceServer;
class ResponseThread;

struct TCPMessageHeader{
	uint32_t magicKey = 427;
	uint32_t clientSidedID; 
	uint64_t messageLength;
	uint32_t returnValue; // only valid for responses
};


class TCPClientMessage : public ServerClientMessage{
public:
	ResponseThread * rspThread;
	uint32_t clientSidedID;

	TCPClientMessage(uint32_t clientSidedID, ResponseThread* rspThread,  const char * payload, uint64_t size) : ServerClientMessage( payload, size), rspThread(rspThread), clientSidedID(clientSidedID) {}	

	static TCPClientMessage * readMessage(GInputStream * istream, ResponseThread * rspThread){
	    gsize messageSizeRead = 0;
	    gboolean ret = FALSE;

	    TCPMessageHeader hdr;

		// Read length
        ret =  g_input_stream_read_all(istream, & hdr, 16, &messageSizeRead, NULL, NULL);   
        if(!ret || messageSizeRead == 16 || hdr.magicKey != 427) { 
            return nullptr;
        }

        char * message = (char*) malloc(hdr.messageLength);               
        // Write message
        ret =  g_input_stream_read_all(istream, message, hdr.messageLength, &messageSizeRead, NULL, NULL);   
        if(!ret || messageSizeRead != hdr.messageLength ) { 
    		return nullptr;
        }

        return new TCPClientMessage(hdr.clientSidedID, rspThread, message, hdr.messageLength);
	}

	bool sendResponse(ResponseMessage * response, GOutputStream * ostream){
		gboolean ret = FALSE;
	    gsize bytes_written;
	    TCPMessageHeader hdr;
	    hdr.clientSidedID = clientSidedID;
	    hdr.returnValue = response->response;
	    hdr.messageLength = response->size;

        // Response size
        ret =  g_output_stream_write_all(ostream, & hdr, 20, & bytes_written, NULL, NULL);   
        if(!ret) { 
        	return false;
        }

        ret =  g_output_stream_write_all(ostream, response->payload, response->size, & bytes_written, NULL, NULL);   
        if(!ret) { 
        	return false;
        }

        return true;
	}

	void isendResponse(ResponseMessage * response);
};

class ResponseThread{
public:
	GOutputStream * ostream;

	bool finished = false;

	// protect central datastructures
	mutex m;
	condition_variable cv;
	
	list<pair<TCPClientMessage*, ResponseMessage*>> pendingMessages;

	ResponseThread(GOutputStream * ostream){
		this->ostream = ostream;
	}

	~ResponseThread(){
		g_object_unref(ostream);
	}

	void enqueueMessage(TCPClientMessage* msg, ResponseMessage* response){
		unique_lock<mutex> lk(m);
		pendingMessages.push_back({msg, response});
		if( pendingMessages.size() == 1){
			cv.notify_one();
		}
	}

	void responseThreadFunc(){
		while(true){
			pair<TCPClientMessage*, ResponseMessage*> msg;
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

void TCPClientMessage::isendResponse(ResponseMessage * response){
    rspThread->enqueueMessage(this, response);
}

class GIOServiceServer : public ServiceServer{
public:
	GSocketListener * listener;
	GCancellable * cancelable;

	void ipublish( ResponseMessage * msg ){
		assert(false);
		delete(msg);
	}


	void acceptThreadFunc(){

		GError * error = nullptr;
		GSocketConnection * connection =  g_socket_listener_accept(listener, nullptr, cancelable, &error);
		if ( g_cancellable_is_cancelled (cancelable))
			return;

	    cout << "Incomming connection" << endl;

	    GOutputStream * ostream = g_io_stream_get_output_stream(G_IO_STREAM(connection));
	    GInputStream * istream = g_io_stream_get_input_stream(G_IO_STREAM(connection));

	    ResponseThread * responseThreadInstance = new ResponseThread(ostream);

		// spawn a response thread
		thread responseThread( & ResponseThread::responseThreadFunc, responseThreadInstance);

		// spawn another thread to accept the next connection
		{
		thread clientThread( & GIOServiceServer::acceptThreadFunc, this);
		clientThread.detach();
		}

	    while(1) {
	        TCPClientMessage * msg = TCPClientMessage::readMessage(istream, responseThreadInstance);
	        if ( msg == nullptr ){
	        		TCPClientMessage errMsg(0, responseThreadInstance, nullptr, 0);
	        		ResponseMessage response(0, nullptr, "Error, invalid packet format!", 22);
	        		errMsg.sendResponse(& response, ostream);
	        	break;
	        }
	        srcb->messageReceivedCB(this, msg);
	    }

	    // terminate response thread
	    {
	    unique_lock<mutex> lk(responseThreadInstance->m);
	    responseThreadInstance->finished = true;
		}
	    responseThreadInstance->cv.notify_one();

	    responseThread.join();

	    g_object_unref(connection);
	}

	GIOServiceServer(const string & address, uint16_t port) throw(CommunicationModuleException){
		listener = g_socket_listener_new();
		cancelable =  g_cancellable_new();
		if(listener == nullptr ){
			throw CommunicationModuleException("Could not instantiate listener");
		}

	    assert(sizeof(TCPMessageHeader) == 24);

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

			CommunicationModuleException e = CommunicationModuleException(error->message);
			g_error_free(error);
			throw e;
		}

		thread clientThread( & GIOServiceServer::acceptThreadFunc, this);
    	clientThread.detach();		
	}

	~GIOServiceServer(){
		cout << "Shutting down server" << endl;
		g_cancellable_cancel(cancelable);

		g_object_unref(listener);
		g_object_unref(cancelable);
	}
};

