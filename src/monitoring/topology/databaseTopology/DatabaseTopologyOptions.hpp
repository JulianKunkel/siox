#ifndef INCLUDE_GUARD_DATABASE_TOPOLOGY_OPTIONS_HPP
#define INCLUDE_GUARD_DATABASE_TOPOLOGY_OPTIONS_HPP

#include <string>
#include <core/component/component-options.hpp>

using namespace std;

namespace monitoring {
	//@serializable
	class DatabaseTopologyOptions : public core::ComponentOptions {
		public:
		string hostaddress;
		uint32_t port;
		string username;
		string password;
		string dbname;
	};
}

#endif
