/**
 * @file monitoring/statistics_multiplexer/plugin/testListener/TestListener.hpp
 *
 * @author Nathanael Hübbe
 * @date 2013
 */

#ifndef INCLUDE_GUARD_MONITORING_STATISTICS_TEST_LISTENER_H
#define INCLUDE_GUARD_MONITORING_STATISTICS_TEST_LISTENER_H

#include <monitoring/statistics_multiplexer/StatisticsMultiplexerPluginImplementation.hpp>

namespace monitoring {
	class TestListener : public StatisticsMultiplexerPlugin {
		public:
			virtual bool registeredValidInput() = 0;	//for debugging purposes only, this entire class is for debugging purposes...
	};
}

#endif
