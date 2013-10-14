#include <core/component/component-options.hpp>


using namespace core;

namespace monitoring {

	//@container
	class MultiplexerAsyncOptions: public ComponentOptions {
		public:
			float max_time_per_plugin = 0.01;
	};

}
