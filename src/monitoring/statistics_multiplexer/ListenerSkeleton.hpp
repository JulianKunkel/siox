#ifndef INCLUDE_GUARD_MONITORING_STATISTICS_LISTENER_SKELETON_H
#define INCLUDE_GUARD_MONITORING_STATISTICS_LISTENER_SKELETON_H

#include <monitoring/statistics_multiplexer/StatisticsMultiplexerPluginImplementation.hpp>

namespace monitoring {
	class ListenerSkeleton : public StatisticsMultiplexerPlugin {
		public:
			virtual bool registeredValidInput() = 0;	//for debugging purposes only, this entire class is for debugging purposes...
	};
}

#endif
