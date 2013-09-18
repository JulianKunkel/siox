#ifndef GIOINTERNALS_HPP
#define GIOINTERNALS_HPP

#include <string>
#include <list>

#include <assert.h>
#include <glib.h>
#include <gio/gio.h>
#include <gio/gunixsocketaddress.h>

#include <sstream>

#include <boost/regex.hpp>

// this two magic keys indicate the message type
#define MAGIC_KEY 14099
#define MAGIC_ERROR_KEY 16962

using namespace std;

#define SIOX_IPC_PATH "/tmp/"

inline string convertHex( const char * payload, uint64_t size )
{
	char table[] = "0123456789ABCDEF";
	stringstream s;

	for(uint64_t pos = 0; pos < size; pos++ ) {
		const char val = payload[pos];

		int first = val & 15;
		int second = (val & 240)>>4;

		s << table[second];
		s << table[first]  << " ";
	}

	return s.str();
}

enum class SocketType : int{
	TCP = G_SOCKET_TYPE_STREAM,
	SCTP = G_SOCKET_TYPE_SEQPACKET,
	IPC = G_SOCKET_TYPE_DATAGRAM

	// G_SOCKET_TYPE_DATAGRAM   G_SOCKET_PROTOCOL_UDP => not usable with socket_client
	// G_SOCKET_TYPE_SEQPACKET  G_SOCKET_PROTOCOL_SCTP
	// G_SOCKET_TYPE_STREAM     G_SOCKET_PROTOCOL_TCP	
};

inline void splitAddressParts(const string & address, SocketType & type, string & hostname, uint16_t & port, string & ipcPath) throw(CommunicationModuleException){
	port = 4711;

	boost::cmatch cm;
	if ( boost::regex_match(address.c_str(), cm, boost::regex("(?:(ipc|tcp|sctp)://)?(?:([^:]*):?)?([0-9]+)?") ) ) {
		if(cm[1] == "ipc"){
			type = SocketType::IPC;
		}else if (cm[1] == "sctp"){
			type = SocketType::SCTP;
			assert(true); // not 100% supported, yet.
		}else{
			type = SocketType::TCP;
		}
		hostname = cm[2];

		if(cm[3] != ""){
			port = atoi(cm[3].str().c_str());
		}


		if(type == SocketType::IPC){
			string address;
			address = SIOX_IPC_PATH;

			if (hostname == ""){
				throw CommunicationModuleException("Invalid IPC address " + address);
			}

			address += hostname;
			address += ".socket";

			ipcPath = address;
		}else{
			ipcPath = "";
		}

	}else{
		throw CommunicationModuleException("Invalid address " + address);
	}
}

// This function splits the string [type://]<hostname>:<port> into the tupel and returns a socket address.
// Additionally it supports the protocol
inline GSocketAddress * getSocket(const string & address, SocketType & type) {
	string hostname;
	uint16_t port;
	string ipcPath;
	GSocketAddress * gsocketAddr;

	splitAddressParts(address, type, hostname, port, ipcPath);

	if(type == SocketType::IPC){
		gsocketAddr = G_SOCKET_ADDRESS(g_unix_socket_address_new(ipcPath.c_str()));
	}else{
		// for the server
		if (hostname == "all" || hostname == ""){
			GInetAddress* localhost = g_inet_address_new_any(G_SOCKET_FAMILY_IPV4);
			if (! localhost) {
			    localhost = g_inet_address_new_any(G_SOCKET_FAMILY_IPV6);
			}
			gsocketAddr = g_inet_socket_address_new(localhost, port);
			g_object_unref(localhost);
		}else{				
			GSocketAddressEnumerator * enumerator;
			GSocketConnectable * addr = g_network_address_new (hostname.c_str(), port);
		  	enumerator = g_socket_connectable_enumerate (addr);
		  	g_object_unref (addr);

		  	GError * error = NULL;
		  	gsocketAddr = g_socket_address_enumerator_next(enumerator, NULL, & error);
	    	// by adding a cancelable here, a memleak happens.
		  	g_object_unref(enumerator);

		  	if(gsocketAddr == nullptr){
		  		string what = error->message;
		  		g_clear_error(& error);

		  		throw CommunicationModuleException("Address cannot be resolved " + address + " error: " + what);
		  	}

		  	g_clear_error(& error);
		}
	}

	return gsocketAddr;
}

inline uint32_t clientMsgHeaderLen(){
	const uint32_t magicKey = 0;
	const uint32_t clientSidedID = 0;
	const uint64_t msgLength = 0;
	return j_serialization::serializeLen(magicKey) + j_serialization::serializeLen(clientSidedID) + j_serialization::serializeLen(msgLength);
}

inline void serializeMsgHeader(char * buffer, uint64_t & pos, uint64_t size, uint32_t magicKey, uint32_t clientSidedID)
{
	assert(pos == 0);

	j_serialization::serialize(clientSidedID, buffer, pos);
	j_serialization::serialize(magicKey, buffer, pos);
	j_serialization::serialize(size, buffer, pos);
}

inline void deserializeMsgHeader(char * buffer, uint64_t & pos, uint64_t msgLength, uint64_t & size, uint32_t & magicKey, uint32_t & clientSidedID)
{
	assert(pos == 0);
	j_serialization::deserialize(clientSidedID, buffer, pos, msgLength);
	j_serialization::deserialize(magicKey, buffer, pos, msgLength);
	j_serialization::deserialize(size, buffer, pos, msgLength);	
}



inline char * readSocketMessage(GInputStream * istream, uint64_t & msgLength, uint32_t & clientSidedID, CommunicationError & errorValue, GCancellable * cancelable){

	uint32_t magicKey = 0;

    gsize messageSizeRead = 0;
    gboolean ret = FALSE;
    errorValue = CommunicationError::SUCCESS;

    const uint32_t header_length = clientMsgHeaderLen();
    char * header = (char*) malloc(header_length);
    
    //GError * error = NULL;

	// Read header
    ret =  g_input_stream_read_all(istream, header, header_length, &messageSizeRead, cancelable, nullptr);   

    if(!ret || messageSizeRead != header_length) { 
    	free(header);
    	
    	errorValue = CommunicationError::MESSAGE_DAMAGED;
    	msgLength = messageSizeRead;

		//if( error ){
		//	g_error_free(error);
		//}

        return nullptr;
    }

    // deserialize header
	uint64_t pos = 0;

    deserializeMsgHeader(header, pos, header_length, msgLength, magicKey, clientSidedID);

	//cout << "RHeader: " << convertHex(header, header_length) << endl;

    if (magicKey != MAGIC_KEY){
    	free(header);

    	if (magicKey == MAGIC_ERROR_KEY){
    		errorValue = (CommunicationError) msgLength;
    		return nullptr;
    	}

    	errorValue = CommunicationError::MESSAGE_DAMAGED;
    	return nullptr;
    }

    char * message = (char*) realloc(header, msgLength);
    uint64_t toRead = msgLength - header_length;   

    ret =  g_input_stream_read_all(istream, message + header_length, toRead, & messageSizeRead, cancelable, NULL);
    if(! ret || messageSizeRead != toRead ) { 
    	free(message);
    	errorValue = CommunicationError::MESSAGE_DAMAGED;
		return nullptr;
    }

	errorValue = CommunicationError::SUCCESS;
    return message;
}

inline bool sendSocketMessage(BareMessage * msg, GOutputStream * ostream){
	gboolean ret = FALSE;
    gsize bytes_written;

    ret =  g_output_stream_write_all(ostream, msg->payload, msg->size, & bytes_written, NULL, NULL);   
    if(!ret || bytes_written != msg->size) { 
    	return false;
    }

    //cout << "SHEADER: " << convertHex(msg->payload, clientMsgHeaderLen() ) << endl;

    return true;
}


class MessageSendQueue{
protected:
	bool connected = false;

	// protect central data structures
	mutex m;
	condition_variable cv;
	
	list<BareMessage*> pendingMessages;
	GCancellable * cancelable;

	// the response thread owned
	thread * myThread; 

	GOutputStream * ostream = nullptr;

	virtual void messageSend(BareMessage * msg){		
	}


	void msgThreadFunc(){
		while(true){
			BareMessage* msg;
			{
			unique_lock<mutex> lk(m);
			
			while(pendingMessages.size() == 0){
				if (! connected ){
					return;
				}				
				cv.wait(lk);
			}
			if (! connected){
				return;
			}
			msg = pendingMessages.front();
			pendingMessages.pop_front();
			}

			//cout << "Sending !" << endl;
			// if we cannot send data to the remote we close the connection.
			if ( ! sendSocketMessage(msg, ostream) ) {
				unique_lock<mutex> lk(m);
				pendingMessages.push_back(msg);

				connected = false;

		    	g_cancellable_cancel(cancelable);

				return;
			}

			//cout << "Done" << endl;
			messageSend(msg);
		}	
	}

public:
	virtual ~MessageSendQueue(){}

	void clearPendingMessages(){
		 for(auto itr = pendingMessages.begin() ; itr != pendingMessages.end() ; itr++){
    		delete(*itr);
    	}
	}

	void connect(GOutputStream * ostream, GCancellable * cancelable){
		m.lock();
		assert(! connected);

		this->cancelable = cancelable;
		connected = true;
		this->ostream = ostream;
		m.unlock();

		myThread = new thread( & MessageSendQueue::msgThreadFunc, this);
	}


	void disconnect(){
   		// terminate response thread
    	m.lock();
    	if(! connected){
    		m.unlock();
    		return;
    	}
    	connected = false;
    	m.unlock();

	    cv.notify_one();
    	myThread->join();
    	delete(myThread);
	}

	void enqueue(BareMessage * msg){
		unique_lock<mutex> lk(m);
		pendingMessages.push_back(msg);
		if( pendingMessages.size() == 1){
			cv.notify_one();
		}
	}

	void terminate(){
		disconnect();

    	// Destroy all pending messages without notification.
		clearPendingMessages();
	}

	void waitUntilAllMsgsAreSend(){

		disconnect();

		// now we try to send all pending messages by ourselve.
		unique_lock<mutex> lk(m);

		while(pendingMessages.size() > 0){
			if (! connected ){
				break;
			}

			auto msg = pendingMessages.front();
			pendingMessages.pop_front();

			if ( sendSocketMessage(msg, ostream) ){
				messageSend(msg);
			}
		}
	}

	list<BareMessage*> getUnsendMessages(){
		assert(ostream == nullptr);
		return pendingMessages;
	}
};



#endif