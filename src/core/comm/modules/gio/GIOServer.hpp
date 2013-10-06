#ifndef GIO_SERVER_INTERNALS_HPP
#define GIO_SERVER_INTERNALS_HPP


#include <glib.h>
#include <gio/gio.h>

#include <mutex>
#include <atomic>
#include <iostream>
#include <map>
#include <list>
#include <utility>
#include <exception> 
#include <thread>
#include <condition_variable>

#include <core/comm/ServiceServer.hpp>

#include <util/JobProcessorQueue.hpp>


using namespace std;
using namespace core;

class TCPClientMessage;

class GIOServiceServer : public ServiceServer{
protected:
	friend TCPClientMessage;

	GSocketListener * listener = nullptr;
	GCancellable * cancelable;
	string address;

	mutex pendingClientThreadMutex;
	condition_variable allThreadsFinished;
	uint64_t lastThreadID = 0;

	// map thread_id to the thread
	map<uint64_t, pair<thread*, GCancellable*>> pendingClientThreads;

	thread* finishedThread = nullptr;
	void cleanTerminatedThread();

public:
	void ipublish( void * object );
	
	uint32_t headerSizeClientMessage();

	void addAcceptorThread();

	void removeAcceptorThread(uint64_t threadID);

	void acceptThreadFunc(uint64_t threadID, GCancellable * cancelable);

	void listen() throw(CommunicationModuleException);

	GIOServiceServer(const string & address, ProcessorQueue * sendQueue) throw(CommunicationModuleException);

	~GIOServiceServer();
};


#endif