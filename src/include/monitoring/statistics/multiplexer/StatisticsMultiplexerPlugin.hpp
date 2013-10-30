#ifndef STATISTICSMULTIPLEXER_PLUGIN_H
#define STATISTICSMULTIPLEXER_PLUGIN_H

#include <assert.h>

#include <core/component/Component.hpp>
#include <monitoring/statistics/multiplexer/StatisticsMultiplexerListener.hpp>

namespace monitoring {

	class StatisticsMultiplexerPlugin: public core::Component, public StatisticsMultiplexerListener {
	};

}

#define STATISTICS_MULTIPLEXER_PLUGIN_INTERFACE "monitoring_statisticsmultiplexer_plugin"

#endif

