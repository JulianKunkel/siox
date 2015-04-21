#include <string>

#include <core/component/component-options.hpp>

//@serializable
class FileAssociationMapperOptions: public core::ComponentOptions {
	public:
		std::string filename;
};

