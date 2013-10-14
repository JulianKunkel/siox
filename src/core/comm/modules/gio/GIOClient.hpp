#ifndef GIO_CLIENT_INTERNALS_HPP
#define GIO_CLIENT_INTERNALS_HPP


#include <assert.h>
#include <glib.h>
#include <gio/gio.h>

#include <mutex>
#include <atomic>
#include <iostream>
#include <unordered_map>
#include <utility>
#include <exception> 
#include <thread>
#include <condition_variable>

#include <core/comm/ServiceClient.hpp>
#include <core/comm/Message.hpp>

using namespace std;
using namespace core;

class MessageSendQueue;

/*
The client creates two threads.
First one to connect to the server, once connected this one serves as receiver thread.
This thread also spawns another "sender" thread.
 */
class GIOClient : public ServiceClient{
protected:
	string address;

	GSocketClient * socket  = nullptr;
	
	atomic_int_fast32_t lastMessageID;	

	mutex connection_ready_mutex;
	
	bool connection_ongoing = false;
	bool connection_established = false;

	thread * connectionThread = nullptr;
	GCancellable * shutdown_cancelable;
	GCancellable * one_thread_error_cancelable;
	MessageSendQueue * sendQueue;

	mutex pendingResponses_mutex;
	unordered_map<uint32_t, BareMessage*> pendingResponses;
public:	

	/**
	 This thread function connects to the server.
	 Once created it spawns a sender thread.
	 Then it serves as reading thread.
	 */
	void connectionThreadFunc(thread * lastThread);
	

	GIOClient(const string & address);

	const string & getAddress() const{
		return this->address;
	}

	void ireconnect();

	void isend( BareMessage * msg );

	uint32_t headerSize();

	void serializeHeader(char * buffer, uint64_t & pos, uint64_t size);

	~GIOClient();
};


#endif