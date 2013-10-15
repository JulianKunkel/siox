#include <core/component/component-options.hpp>

using namespace core;

namespace monitoring {

	//@serializable
	class ActivityMultiplexerAsyncOptions: public ComponentOptions {
		public:
			/** Not implemented, but it planned as a way to skip plugins that produce a lot of overhead */
			float maxTimePerPlugin = 0.01;
			/** Capacity of the async queue, if full, activities will be dropped */
			int maxPendingActivities = 100;
	};

}
