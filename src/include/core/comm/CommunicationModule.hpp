#ifndef CORE_COMM_MODULE_HPP
#define CORE_COMM_MODULE_HPP

#include <string>
#include <exception>

#include <core/component/Component.hpp>

#include <core/comm/ServiceClient.hpp>
#include <core/comm/ServiceServer.hpp>

#include <util/JobProcessorQueue.hpp>

using namespace util;
using namespace std;

namespace core{
	class CommunicationModule : public Component {
	protected:		
		virtual ServiceServer * startServerService(const string & address, ProcessorQueue * sendQueue) throw(CommunicationModuleException) = 0;
		virtual ServiceClient * startClientService(const string & server_address, ProcessorQueue * sendQueue) throw(CommunicationModuleException) = 0;		
	public:
		
		ServiceServer * startServerService(const string & address, ServerCallback * msg_rcvd_callback, ProcessorQueue * sendQueue = nullptr) throw(CommunicationModuleException);

		ServiceClient * startClientService(const string & server_address, ConnectionCallback * ccb, MessageCallback * messageCallback, ProcessorQueue * sendQueue = nullptr) throw(CommunicationModuleException);
	};
}

#define CORE_COMM_INTERFACE "core_comm"

#endif