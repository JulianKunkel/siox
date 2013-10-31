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

		virtual const vector<pair<OntologyAttributeID, vector< pair< string, string> > > > requiredMetrics() throw();
		virtual void notify(const vector<shared_ptr<Statistic> >& statistics) throw();
};

void StatisticsFileWriter::initPlugin() throw() {
	StatisticsFileWriterOptions &  o = getOptions<StatisticsFileWriterOptions>();

}

ComponentOptions* StatisticsFileWriter::AvailableOptions() {
	return new StatisticsFileWriterOptions();
}

const vector<pair<OntologyAttributeID, vector< pair< string, string> > > > StatisticsFileWriter::requiredMetrics() throw() {
	return vector<pair<OntologyAttributeID, vector< pair< string, string> > > >();
}

void StatisticsFileWriter::notify(const vector<shared_ptr<Statistic> >& statistics) throw() {

}

extern "C" {
	void* MONITORING_STATISTICS_MULTIPLEXER_PLUGIN_INSTANCIATOR_NAME() {
		return new StatisticsFileWriter();
	}
}
