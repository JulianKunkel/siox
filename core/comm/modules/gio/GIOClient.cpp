#include <mutex>

#include <core/comm/ServiceClient.hpp>
#include <core/comm/Message.hpp>

using namespace std;

class GIOClient;

class GIOClient : public ServiceClient{
public:	
	string address;
	GSocketClient * socket;
	GSocketConnection * conn;

	mutex connection_ready_mutex;
	
	bool connection_ongoing = false;
	bool connection_established = false;

	GIOClient(const string & address){
		this->address = address;

		socket = g_socket_client_new();
	}

	void connectionThread(){
		cout << "Client connecting to " << address << endl;

		connection_ready_mutex.lock();

		pair<string, uint16_t> addresses = splitAddress(address);

		conn = g_socket_client_connect_to_uri      (socket, addresses.first.c_str(), addresses.second,  NULL, NULL);

		connection_ongoing = false;		

		if (conn == nullptr){		
			connection_ready_mutex.unlock();
			connectionCallback->connectionErrorCB(* this, ConnectionError::SERVER_NOT_ACCESSABLE);
			return;
		}

		connection_established = true;

		connection_ready_mutex.unlock();

		//GObjectInputStream * istream = g_io_stream_get_input_stream (G_IO_STREAM (connection));
  		//GOutputStream * ostream = g_io_stream_get_output_stream (G_IO_STREAM (connection));

  		//g_object_unref(conn);
  		conn = nullptr;
  	}



	void ireconnect(){
		lock_guard<mutex> lock(connection_ready_mutex);
		// avoid multiple reconnects.
		cout << "ireconnect " << connection_ongoing << " " << connection_established << endl;
		if( connection_ongoing || connection_established ){
			return;
		}
		connection_ongoing = true;

		thread connectionThread( & GIOClient::connectionThread, this);
		connectionThread.detach();
	}

	const string & getAddress() const{
		return this->address;
	}

	void isend( ClientMessage * msg ){
		
	}

	~GIOClient(){
		cout << "Shutting down client \"" << getAddress() << "\"" << endl;

		g_object_unref(socket);
		// notify client etc.
	}
};
