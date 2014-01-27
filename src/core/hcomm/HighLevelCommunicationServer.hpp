#ifndef CORE_HCOMM_HL_COMM_M_HPP
#define CORE_HCOMM_HL_COMM_M_HPP

#include <core/hcomm/HighLevelCommunicationModule.hpp>
#include <core/hcomm/HighLevelCommunicationModuleOptions.hpp>

class HighLevelServiceClient;
class HighLevelServiceServer;

class HLCommunicationModuleImpl : public HLCommunicationModule {
public:
	virtual HLCommServiceServer * startServerService(const string & address) throw(CommunicationModuleException);

	virtual HLCommServiceClient * startClientService(const string & server_address) throw(CommunicationModuleException);

	virtual void init(){

	}

	virtual ComponentOptions* AvailableOptions(){
		return new HighLevelCommunicationModuleOptions();
	}

	void hasStopped(HighLevelServiceClient * client){
		unique_lock<mutex> lk(parentListMutex);
		// TODO remove from the unordered map and list
		// Delete the client if all virtual connections are completed
	}

	void hasStopped(HighLevelServiceServer * client){
		unique_lock<mutex> lk(parentListMutex);
		// TODO remove from the unordered map and list
		// Delete the server if all virtual connections are completed
	}

private:
	// this mutex protects createdServers and createdClientConnections
	mutex parentListMutex;

	// map the server address to only one server instance
	unordered_map<string, pair<ServiceServer*, list<HighLevelServiceServer*> > > createdServers;

	// map the client address to only one client instance
	unordered_map<string, pair<ServiceClient*, list<HighLevelServiceClient*> > > createdClientConnections;
};

#endif