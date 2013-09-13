#include <mutex>
#include <atomic>

#include <core/comm/ServiceClient.hpp>
#include <core/comm/Message.hpp>


using namespace std;

class GIOClient;

class GIOClient : public ServiceClient{
public:	
	string address;
	GSocketClient * socket;
	GSocketConnection * conn;
	
	atomic_int_fast32_t lastMessageID;	

	mutex connection_ready_mutex;
	
	bool connection_ongoing = false;
	bool connection_established = false;

	thread * connectionThread = nullptr;
	GCancellable * cancelable;

	GIOClient(const string & address){
		this->address = address;

		socket = g_socket_client_new();
		cancelable =  g_cancellable_new();
	}

	void senderThreadFunc(GOutputStream * ostream){

	}

	void recvThreadFunc(GInputStream * istream){

	}


	/**
	 This thread function connects to the server.
	 Once created it spawns a sender thread.
	 Then it serves as reading thread.
	 */
	void connectionThreadFunc(thread * lastThread){
		cout << "Client connecting to " << address << endl;

		if (lastThread){
			lastThread->join();

			delete(lastThread);
		}

		connection_ready_mutex.lock();

		//pair<string, uint16_t> addresses = splitAddress(address);

		GError * error = NULL;
		conn = g_socket_client_connect_to_host(socket, address.c_str(), 1, cancelable, & error);

		connection_ongoing = false;		
		connection_ready_mutex.unlock();

		if (conn == nullptr){
			cout << "Error " << error->message << endl;
			g_error_free(error);

			connectionCallback->connectionErrorCB(* this, CommunicationError::SERVER_NOT_ACCESSABLE);
			return;
		}

		connection_established = true;

		connectionCallback->connectionSuccessfullCB(*this);

		if ( g_cancellable_is_cancelled (cancelable)){
			g_object_unref(conn);
			return;
		}

		GInputStream * istream = g_io_stream_get_input_stream (G_IO_STREAM (conn));
  		GOutputStream * ostream = g_io_stream_get_output_stream (G_IO_STREAM (conn));
		thread writerThread( & GIOClient::senderThreadFunc, this, ostream);

		// start the receiving function
		recvThreadFunc(istream);		

		// the receiving connection has been closed, so we should join the writer thread.
		writerThread.join();

  		g_object_unref(conn);
  	}



	void ireconnect(){
		connection_ready_mutex.lock();

		cout << "ireconnect " << connection_ongoing << " " << connection_established << endl;		
		// avoid multiple reconnects.
		if( connection_ongoing || connection_established ){
			connection_ready_mutex.unlock();

			return;
		}
		connection_ongoing = true;

		connectionThread = new thread( & GIOClient::connectionThreadFunc, this, connectionThread);

		connection_ready_mutex.unlock();
	}

	const string & getAddress() const{
		return this->address;
	}

	BareMessage * isend( void * object ){
		uint64_t msg_size = headerSize() + messageCallback->serializeMessageLen(object);
		char * payload = (char*) malloc(msg_size);
		uint64_t pos = 0;

		serializeHeader(payload, pos, msg_size);
		assert(headerSize() == pos);

		messageCallback->serializeMessage(object, payload, pos);
		assert(pos == msg_size);

		BareMessage * msg = new BareMessage(payload, msg_size);

		isend(msg);
		return msg;
	}

	void isend( BareMessage * msg ) {
		delete(msg);
	}

	uint32_t headerSize(){
		return clientMsgHeaderLen();
	}

	void serializeHeader(char * buffer, uint64_t & pos, uint64_t size){
		uint32_t magicKey = MAGIC_KEY;
		uint32_t clientSidedID = lastMessageID.fetch_add(1);

		serializeMsgHeader(buffer, pos, size, magicKey, clientSidedID);
	}

	~GIOClient(){
		cout << "Shutting down client \"" << getAddress() << "\"" << endl;

		connection_ready_mutex.lock();

		g_cancellable_cancel(cancelable);
		
		if (connectionThread){
			connection_ready_mutex.unlock();

			connectionThread->join();
			delete(connectionThread);
		}

		g_object_unref(socket);
		g_object_unref(cancelable);
		// notify client etc.
	}
};
