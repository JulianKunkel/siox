#include <core/component/component-macros.hpp>


using namespace core;

namespace monitoring{


class MultiplexerAsyncOptions: public ComponentOptions{
public:
	float max_time_per_plugin = 0.01;

	SERIALIZE_CONTAINER( MEMBER(max_time_per_plugin) )
};

}
