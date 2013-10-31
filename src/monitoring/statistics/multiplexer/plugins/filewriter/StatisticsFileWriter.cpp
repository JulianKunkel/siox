/**
 * @file TestListener.cpp
 *
 * @author Nathanael Hübbe
 * @date 2013
 */

#include <iostream>

#include <monitoring/statistics/multiplexer/StatisticsMultiplexer.hpp>
#include <monitoring/statistics/multiplexer/StatisticsMultiplexerPluginImplementation.hpp>

#include "StatisticsFileWriterOptions.hpp"

using namespace std;
using namespace monitoring;
using namespace core;

class StatisticsFileWriter : public StatisticsMultiplexerPlugin {
	public:
		virtual void initPlugin() throw();

		virtual ComponentOptions * AvailableOptions();

		virtual void notifyAvailableStatisticsChange( const vector<shared_ptr<Statistic> > & statistics, bool addedStatistics, bool removedStatistics ) throw();
		virtual void newDataAvailable() throw();
	private:
		vector<shared_ptr<Statistic> > availableStatistics;
};

void StatisticsFileWriter::initPlugin() throw() {
	StatisticsFileWriterOptions &  o = getOptions<StatisticsFileWriterOptions>();

}

ComponentOptions* StatisticsFileWriter::AvailableOptions() {
	return new StatisticsFileWriterOptions();
}

void StatisticsFileWriter::notifyAvailableStatisticsChange( const vector<shared_ptr<Statistic> > & statistics, bool addedStatistics, bool removedStatistics ) throw(){
	availableStatistics = statistics;
}

void StatisticsFileWriter::newDataAvailable() throw(){

}

extern "C" {
	void* MONITORING_STATISTICS_MULTIPLEXER_PLUGIN_INSTANCIATOR_NAME() {
		return new StatisticsFileWriter();
	}
}
