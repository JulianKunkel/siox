#ifndef CORE_COMM_MODULE_HPP
#define CORE_COMM_MODULE_HPP

#include <string>
#include <exception>

#include <core/component/Component.hpp>

#include <core/comm/ServiceClient.hpp>
#include <core/comm/ServiceServer.hpp>


using namespace std;

namespace core{
	class CommunicationModuleException : public std::exception {
	public:
		CommunicationModuleException( const string & err_msg ) : err_msg_( err_msg ) {}

		const char * what() const throw() {
		   return err_msg_.c_str();
		}

	private:
		const string err_msg_;
	};


	class CommunicationModule : public Component {
	public:
		//virtual void setWorkProcessor() = 0; 
		virtual ServiceServer * startServerService(const string & address) throw(CommunicationModuleException) = 0;
		virtual ServiceClient * startClientService(const string & server_address, ConnectionCallback & ccb) throw(CommunicationModuleException) = 0;
	};
}

#define CORE_COMM_INTERFACE "core_comm"

#endif