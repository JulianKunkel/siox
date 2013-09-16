#ifndef SERVER_FACTORY_H
#define SERVER_FACTORY_H

#include <exception>
#include <string>

#include <core/comm/CommunicationModuleImplementation.hpp>

#include "CommASIOOptions.hpp"

using namespace std;

class CommASIO : public core::CommunicationModule {
public:
	virtual core::ServiceServer * startServerService(const string & address) throw(core::CommunicationModuleException);

	virtual core::ServiceClient * startClientService(const string & server_address, core::ConnectionCallback & ccb) throw(core::CommunicationModuleException);

	virtual void init(){

	}

	virtual core::ComponentOptions* AvailableOptions(){
		return new CommASIOOptions();
	}
};

#endif
