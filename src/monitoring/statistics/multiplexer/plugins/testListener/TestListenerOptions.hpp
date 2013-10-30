/**
 * @file TestListenerOptions.hpp
 *
 * @author Nathanael Hübbe
 * @date 2013
 */

#ifndef INCLUDE_GUARD_MONITORING_STATISTICS_MULTIPLEXER_TEST_LISTENER_OPTIONS_H
#define INCLUDE_GUARD_MONITORING_STATISTICS_MULTIPLEXER_TEST_LISTENER_OPTIONS_H

#include <monitoring/statistics/multiplexer/StatisticsMultiplexerPluginOptions.hpp>
#include <core/component/ComponentReference.hpp>

namespace monitoring {
	//@serializable
	class TestListenerOptions : public StatisticsMultiplexerPluginOptions {
		public:
			//TODO: StatisticsMultiplexerPluginOptions already has stuff like this. Remove duplication.
			core::ComponentReference ontology;
			core::ComponentReference smux;
	};
}

#endif
