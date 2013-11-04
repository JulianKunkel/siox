#ifndef INCLUDE_MONITORING_STATISTICS_MULTIPLEXER_FILEWRITER_OPTIONS_H
#define INCLUDE_MONITORING_STATISTICS_MULTIPLEXER_FILEWRITER_OPTIONS_H

#include <string>

#include <monitoring/statistics/multiplexer/StatisticsMultiplexerPluginOptions.hpp>
#include <core/component/ComponentReference.hpp>

//@serializable
class StatisticsFileWriterOptions : public monitoring::StatisticsMultiplexerPluginOptions {
	public:
		std::string filename;
};

#endif
