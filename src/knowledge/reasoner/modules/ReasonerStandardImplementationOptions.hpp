#include <core/component/component-options.hpp>
#include <core/component/ComponentReference.hpp>

#include <list>

//@serializable
class ReasonerStandardImplementationOptions: public core::ComponentOptions {
	public:
		uint32_t update_intervall_ms;
		std::list<std::string> remoteReasoners;

		core::ComponentReference comm;
		std::string serviceAddress;
};

