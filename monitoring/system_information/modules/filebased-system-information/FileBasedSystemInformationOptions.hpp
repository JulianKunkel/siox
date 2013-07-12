#include <core/component/component-macros.hpp>

using namespace std;
using namespace monitoring;

class FileBasedSystemInformationOptions: public core::ComponentOptions{
public:
	string filename;

	SERIALIZE_CONTAINER(MEMBER(filename))
};

