#include <iostream>
#include <fstream>
#include <boost/archive/text_oarchive.hpp>

#include <monitoring/statistics/multiplexer/StatisticsMultiplexer.hpp>
#include <monitoring/statistics/multiplexer/StatisticsMultiplexerPluginImplementation.hpp>

#include <monitoring/statistics/StatisticTypesSerializableText.cpp>

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

		~StatisticsFileWriter();
	private:
		const vector<shared_ptr<Statistic> > * statistics;
		ofstream file;
		boost::archive::text_oarchive * oa;		
};

StatisticsFileWriter::~StatisticsFileWriter(){
	file.close();
}

void StatisticsFileWriter::initPlugin() throw() {
	StatisticsFileWriterOptions &  o = getOptions<StatisticsFileWriterOptions>();
	file.open( o.filename );
	oa = new boost::archive::text_oarchive( file, boost::archive::no_header | boost::archive::no_codecvt );
}

ComponentOptions* StatisticsFileWriter::AvailableOptions() {
	return new StatisticsFileWriterOptions();
}

void StatisticsFileWriter::notifyAvailableStatisticsChange( const vector<shared_ptr<Statistic> > & statistics, bool addedStatistics, bool removedStatistics ) throw(){
	this->statistics = & statistics;	
}

void StatisticsFileWriter::newDataAvailable() throw(){
	uint64_t timestamp = (*statistics->begin())->curTimestamp();
	*oa << timestamp ;
	uint64_t size = statistics->size();
	*oa << size;

	// write out all currently existing statistics
	for(auto itr = statistics->begin(); itr != statistics->end(); itr++){
		StatisticsDescription & sd = **itr;
		*oa << sd;
		*oa << (*itr)->curValue;		
	}
}

extern "C" {
	void* MONITORING_STATISTICS_MULTIPLEXER_PLUGIN_INSTANCIATOR_NAME() {
		return new StatisticsFileWriter();
	}
}
