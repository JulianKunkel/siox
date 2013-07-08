#include <core/component/component-macros.hpp>

using namespace std;
using namespace monitoring;

class FileAssociationMapperOptions: public core::ComponentOptions{
public:
	string filename;

	SERIALIZE_CONTAINER(MEMBER(filename))
};

