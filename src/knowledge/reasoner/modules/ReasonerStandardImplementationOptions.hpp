#ifndef KNOWLEDGE_REASONER_COMMUNICATION_OPTIONS_HPP
#define KNOWLEDGE_REASONER_COMMUNICATION_OPTIONS_HPP


#include <core/component/component-options.hpp>
#include <core/component/ComponentReference.hpp>

#include <ostream>
#include <list>

//@serializable
struct ReasonerCommunicationOptions {
	std::string reasonerID;

	std::string upstreamReasoner;

	core::ComponentReference comm;
	std::string serviceAddress;
};

//@serializable
class ReasonerStandardImplementationOptions: public core::ComponentOptions {
	public:
		uint32_t update_intervall_ms;

		ReasonerCommunicationOptions communicationOptions;
		
		//@serializable
		enum class Role : uint8_t{
			NONE = 0,
			PROCESS = 1,
			NODE = 2,
			SYSTEM = 3,
		};

		Role role = Role::PROCESS;

		core::ComponentReference statisticsCollector; // if available
};

inline std::ostream & operator<<( std::ostream & os, ReasonerStandardImplementationOptions::Role role )
{
	switch(role){
		case ReasonerStandardImplementationOptions::Role::NONE:
			return os << "NONE";
		case ReasonerStandardImplementationOptions::Role::PROCESS:
			return os << "PROCESS";
		case ReasonerStandardImplementationOptions::Role::NODE:
			return os << "NODE";
		case ReasonerStandardImplementationOptions::Role::SYSTEM:
			return os << "SYSTEM";
		default:
			return os << "(UNKNOWN)";
	}
}

#endif