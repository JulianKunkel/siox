#include <core/component/component-macros.hpp>


using namespace core;

namespace monitoring{


class ActivityMultiplexerPluginOptions: public ComponentOptions{
public:
	ComponentReference multiplexer;
	ComponentReference dereferenceFacade;

	SERIALIZE_CONTAINER(MEMBER(multiplexer) MEMBER(dereferenceFacade))
};

}


#define SERIALIZE_OPTIONS(VAR_) SERIALIZE(VAR_ PARENT_CLASS(ActivityMultiplexerPluginOptions))

