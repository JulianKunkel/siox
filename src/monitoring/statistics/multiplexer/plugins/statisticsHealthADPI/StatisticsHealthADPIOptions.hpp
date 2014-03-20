#ifndef INCLUDE_MONITORING_STATISTICS_MULTIPLEXER_HEALTH_ADPI_OPTIONS_H
#define INCLUDE_MONITORING_STATISTICS_MULTIPLEXER_HEALTH_ADPI_OPTIONS_H

#include <string>

#include <core/component/component-options.hpp>
#include <core/component/ComponentReference.hpp>

#include <monitoring/statistics/multiplexer/StatisticsMultiplexerPluginOptions.hpp>

#include <monitoring/statistics/StatisticsCollection.hpp>
#include <monitoring/statistics/collector/StatisticsCollector.hpp>


//@serializable
class StatisticsHealthADPIOptions : public monitoring::StatisticsMultiplexerPluginOptions {
	public:

		core::ComponentReference statisticsCollector;

};

#endif
