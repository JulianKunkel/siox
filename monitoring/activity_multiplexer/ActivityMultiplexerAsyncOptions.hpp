#include <core/component/component-macros.hpp>


using namespace core;

namespace monitoring {

	class ActivityMultiplexerAsyncOptions: public ComponentOptions {
		public:
			float maxTimePerPlugin = 0.01;
			int maxPendingActivities = 100;

			SERIALIZE_CONTAINER( MEMBER( maxTimePerPlugin ) MEMBER( maxPendingActivities ) )
	};

}
