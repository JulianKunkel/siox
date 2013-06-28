#include <core/component/component-macros.hpp>


#include <monitoring/activity_multiplexer/ActivityMultiplexerPlugin.hpp>

using namespace std;
using namespace monitoring;

class FileWriterPluginOptions: public ActivityMultiplexerPluginOptions{
public:
	string filename;

	SERIALIZE_CONTAINER(MEMBER(filename))
};


