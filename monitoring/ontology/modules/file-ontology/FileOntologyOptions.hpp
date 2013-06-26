#include <core/container/container-serializer.hpp>
#include <core/container/container-macros.hpp>

using namespace std;
using namespace monitoring;

class FileOntologyOptions: public core::ComponentOptions{
public:
	string filename;

	SERIALIZE(MEMBER(filename))
};


