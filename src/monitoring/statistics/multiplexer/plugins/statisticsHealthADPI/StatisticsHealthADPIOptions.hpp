#ifndef INCLUDE_MONITORING_STATISTICS_MULTIPLEXER_HEALTH_ADPI_OPTIONS_H
#define INCLUDE_MONITORING_STATISTICS_MULTIPLEXER_HEALTH_ADPI_OPTIONS_H

#include <string>
#include <vector>
#include <utility>

#include <core/component/component-options.hpp>
// #include <core/component/ComponentReference.hpp>

#include <monitoring/statistics/multiplexer/StatisticsMultiplexerPluginOptions.hpp>

using namespace std;

//@serializable
class StatisticsHealthADPIOptions : public monitoring::StatisticsMultiplexerPluginOptions {

	public:

		vector< pair<string,string> >	requestedStatistics;

};

#endif
