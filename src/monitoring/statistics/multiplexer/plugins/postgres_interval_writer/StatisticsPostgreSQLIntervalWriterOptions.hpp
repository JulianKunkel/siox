#ifndef INCLUDE_MONITORING_STATISTICS_MULTIPLEXER_POSTGRESQLWRITER_OPTIONS_H
#define INCLUDE_MONITORING_STATISTICS_MULTIPLEXER_POSTGRESQLWRITER_OPTIONS_H

#include <string>

#include <monitoring/statistics/multiplexer/StatisticsMultiplexerPluginOptions.hpp>

using namespace std;
using namespace monitoring;

//@serializable
class StatisticsPostgreSQLIntervalWriterOptions : public monitoring::StatisticsMultiplexerPluginOptions {
public:
	string dbinfo;
	int statisticsInterval;
	bool thisIsATest = false;
};

#endif
