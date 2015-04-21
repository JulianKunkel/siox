#include <string>

#include <core/component/component-options.hpp>

//@serializable
class FileOntologyOptions: public core::ComponentOptions {
	public:
		std::string filename;
};

