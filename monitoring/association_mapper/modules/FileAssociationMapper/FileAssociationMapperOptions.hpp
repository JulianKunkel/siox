#include <core/component/component-macros.hpp>

using namespace std;

class FileAssociationMapperOptions: public core::ComponentOptions{
public:
	string filename;

	SERIALIZE_CONTAINER(MEMBER(filename))
};

