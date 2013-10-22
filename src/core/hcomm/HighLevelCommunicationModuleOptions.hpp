#include <core/component/component-options.hpp>
#include <core/component/ComponentReference.hpp>

using namespace core;

//@serializable
class MultiplexerOptions: public ComponentOptions {
	public:
		ComponentReference parentComm;
};
