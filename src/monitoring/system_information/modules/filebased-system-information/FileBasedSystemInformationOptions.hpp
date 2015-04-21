#include <string>

#include <core/component/component-options.hpp>

//@serializable
class FileBasedSystemInformationOptions: public core::ComponentOptions {
	public:
		std::string filename;
};

