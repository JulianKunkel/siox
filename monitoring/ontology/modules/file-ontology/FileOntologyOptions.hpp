#include <core/component/component-macros.hpp>

using namespace std;
using namespace monitoring;

class FileOntologyOptions: public core::ComponentOptions{
public:
	string filename;

	SERIALIZE_CONTAINER(MEMBER(filename))
};

