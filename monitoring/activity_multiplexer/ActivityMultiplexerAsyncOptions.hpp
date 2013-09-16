#include <core/component/component-options.hpp>

using namespace core;

namespace monitoring {

	//@serializable
	class ActivityMultiplexerAsyncOptions: public ComponentOptions {
		public:
			float maxTimePerPlugin = 0.01;
			int maxPendingActivities = 100;
	};

}
