#include <core/component/component-options.hpp>
#include <core/component/ComponentReference.hpp>

#include <list>


//@serializable
class ReasonerStandardImplementationOptions: public core::ComponentOptions {
	public:
		uint32_t update_intervall_ms;
		std::list<std::string> downstreamReasoners;
		std::string upstreamReasoner;

		core::ComponentReference comm;
		std::string serviceAddress;

		enum class Role : uint8_t{
			PROCESS = 0,
			NODE = 1,
			SYSTEM = 2,
		};

		Role role = Role::PROCESS ;		
};

