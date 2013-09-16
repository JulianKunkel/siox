#include "GIOClient.hpp"

#include "GIOinternal.hpp"


using namespace std;
using namespace core;

class ClientMessageSendQueue : public MessageSendQueue{
private:
	GIOClient * client;
protected:
	virtual void messageSend(BareMessage * msg){
		client->getMessageCallback()->messageSendCB(msg);
	}
public:
	ClientMessageSendQueue(GIOClient * client): client(client){}
};

/**
 This thread function connects to the server.
 Once created it spawns a sender thread.
 Then it serves as reading thread.
 */
void GIOClient::connectionThreadFunc(thread * lastThread){
	//cout << "Client connecting to " << address << endl;

	// we start with a locked connection_ready_mutex

	if (lastThread){
		g_cancellable_cancel(one_thread_error_cancelable);

		lastThread->join();

		delete(lastThread);
		g_object_unref(one_thread_error_cancelable);
	}

	one_thread_error_cancelable = g_cancellable_new();

	GSocketAddressEnumerator *enumerator;
	{
		pair<string, uint16_t> hostPort = splitAddress(address);
		GSocketConnectable * addr = g_network_address_new (hostPort.first.c_str(), hostPort.second);
  		enumerator = g_socket_connectable_enumerate (addr);
  		g_object_unref (addr);
  	}

	GError * error = NULL;
	GSocketConnection * conn = nullptr;

	GSocketAddress *sockaddr = nullptr;
  	while (! conn && (sockaddr = g_socket_address_enumerator_next (enumerator, NULL, NULL)))
    {
    	// by adding a cancelable here, a memleak happens.
      	conn = g_socket_client_connect(socket, G_SOCKET_CONNECTABLE(sockaddr), NULL, error ? NULL : & error);
      	g_object_unref (sockaddr);
    }
  	g_object_unref(enumerator);

	connection_ongoing = false;
	connection_ready_mutex.unlock();

	if (conn == nullptr){
		//cout << "Error " << error->message << endl;
		if( error ){
			g_error_free(error);
		}

		connectionCallback->connectionErrorCB(* this, CommunicationError::SERVER_NOT_ACCESSABLE);
		return;
	}

	if(error){
		g_error_free(error);
	}

	connection_established = true;

	if ( g_cancellable_is_cancelled (shutdown_cancelable)){
		g_object_unref(conn);
		return;
	}

	connectionCallback->connectionSuccessfullCB(*this);

	GInputStream * istream = g_io_stream_get_input_stream (G_IO_STREAM (conn));

	sendQueue->connect(g_io_stream_get_output_stream (G_IO_STREAM (conn)), one_thread_error_cancelable);
	CommunicationError comm_error;

	// start receiving responses
	while(! g_cancellable_is_cancelled (one_thread_error_cancelable)){
		uint64_t msgLength = 0;
		uint32_t clientSidedID = 0;

		char * payload = readSocketMessage(istream, msgLength, clientSidedID, comm_error, one_thread_error_cancelable);

		pendingResponses_mutex.lock();
		auto itr = pendingResponses.find(clientSidedID);
		if(itr == pendingResponses.end()){
			// we have received the response already
			cerr << "Received response without previous post -- discarding." << endl;
			if(payload != nullptr){
				free(payload);
			}
			pendingResponses_mutex.unlock();
			continue;
		}

		// remove message because we have now a response
		BareMessage * msg = itr->second;

		pendingResponses.erase(itr);
		pendingResponses_mutex.unlock();

		if( comm_error == CommunicationError::SUCCESS ){
			assert(payload);
			messageCallback->messageResponseCB(msg, payload + clientMsgHeaderLen(), msgLength - clientMsgHeaderLen());			
		}else{
			messageCallback->messageTransferErrorCB(msg, comm_error);

			if( comm_error == CommunicationError::MESSAGE_DAMAGED ){
				// maybe re-send automatically?
				delete(msg);
				break;
			}
		}

		// the message should never be accessed again !
		delete(msg);
		free(payload);
	}

	// the receiving connection has been closed, so we should join the writer thread.
	sendQueue->disconnect();

	g_io_stream_close(G_IO_STREAM (conn), shutdown_cancelable, NULL);

	g_object_unref(conn);

	if (! g_cancellable_is_cancelled (shutdown_cancelable)){
		// we expect the server terminated the connection for some reason.
		connectionCallback->connectionErrorCB(*this, comm_error);
	}

	cout << "connectionThreadFunc End" << endl;
}


GIOClient::GIOClient(const string & address){
	this->address = address;

	socket = g_socket_client_new();
	shutdown_cancelable =  g_cancellable_new();
	sendQueue = new ClientMessageSendQueue(this);
	lastMessageID++;
}


void GIOClient::ireconnect(){
	connection_ready_mutex.lock();

	// avoid multiple reconnects.
	if( connection_ongoing || connection_established || g_cancellable_is_cancelled (shutdown_cancelable) ){
		connection_ready_mutex.unlock();
		return;
	}
	connection_ongoing = true;
	
	connectionThread = new thread( & GIOClient::connectionThreadFunc, this, connectionThread);

	// the unlocking of the condition variable is made in the connectionThread!
}


void GIOClient::isend( BareMessage * msg ) {
	uint32_t clientSidedID;
	uint64_t pos = 0;

	j_serialization::deserialize(clientSidedID, msg->payload, pos, msg->size);

	assert(clientSidedID != 0);

	pendingResponses_mutex.lock();
	pendingResponses[clientSidedID] = msg;
	pendingResponses_mutex.unlock();
	
	sendQueue->enqueue(msg);
}

uint32_t GIOClient::headerSize(){
	return clientMsgHeaderLen();
}

void GIOClient::serializeHeader(char * buffer, uint64_t & pos, uint64_t size){
	uint32_t magicKey = MAGIC_KEY;
	uint32_t clientSidedID = lastMessageID.fetch_add(1);

	serializeMsgHeader(buffer, pos, size, magicKey, clientSidedID);
}

GIOClient::~GIOClient(){
	cout << "Shutting down client \"" << getAddress() << "\"" << endl;

	g_cancellable_cancel(shutdown_cancelable);	

	connection_ready_mutex.lock();
	if (connectionThread){
		g_cancellable_cancel(one_thread_error_cancelable);
		connection_ready_mutex.unlock();

		connectionThread->join();

		delete(connectionThread);
		g_object_unref(one_thread_error_cancelable);
	}

	g_object_unref(socket);
	g_object_unref(shutdown_cancelable);

	sendQueue->waitUntilAllMsgsAreSend();

	delete(sendQueue);

	cout << "Done client" << endl;
}
