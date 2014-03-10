#include <iostream>
#include <mutex>
#include "libpq-fe.h"

#include <monitoring/statistics/multiplexer/StatisticsMultiplexer.hpp>
#include <monitoring/statistics/multiplexer/StatisticsMultiplexerPluginImplementation.hpp>

#include <monitoring/statistics/StatisticTypesSerializableText.cpp>

#include "StatisticsPostgreSQLWriterOptions.hpp"

using namespace std;
using namespace monitoring;
using namespace core;

class StatisticsPostgreSQLWriter : public StatisticsMultiplexerPlugin {
public:
	virtual void initPlugin() throw();
	virtual ComponentOptions *AvailableOptions();
	virtual void notifyAvailableStatisticsChange(const vector<shared_ptr<Statistic> > &statistics, bool addedStatistics, bool removedStatistics) throw();
	virtual void newDataAvailable() throw();
	~StatisticsPostgreSQLWriter();
private:
	const vector<shared_ptr<Statistic> > *statistics;
// 	StatisticsPostgreSQLQuerier *querier_;
	PGconn *dbconn_;
	mutex mtx_;
};


StatisticsPostgreSQLWriter::~StatisticsPostgreSQLWriter()
{
	std::cout << "PostgreSQL Statistics Writter stopping..." << std::endl;
// 	PQfinish(dbconn_);
// 	delete(querier_);

}


void StatisticsPostgreSQLWriter::initPlugin() throw() 
{
	std::cout << "PostgreSQL Statistic Writter initializing..." << std::endl;
	
// 	StatisticsPostgreSQLWriterOptions &o = getOptions<StatisticsPostgreSQLWriterOptions>();
		
// 	dbconn_ = PQconnectdb(o.dbinfo.c_str());
		
// 	if (PQstatus(dbconn_) != CONNECTION_OK) {
// 		std::cerr << "Connection to database failed: " << PQerrorMessage(dbconn_) << std::endl;
// 	}

// 	querier_ = new StatisticsPostgreSQLQuerier(*dbconn_);
}


ComponentOptions* StatisticsPostgreSQLWriter::AvailableOptions() 
{
	return new StatisticsPostgreSQLOptions();
}


void StatisticsPostgreSQLWriter::notifyAvailableStatisticsChange(const vector<shared_ptr<Statistic> > &statistics, bool addedStatistics, bool removedStatistics) throw ()
{
	this->statistics = &statistics;
}

void StatisticsPostgreSQLWriter::newDataAvailable() throw()
{
	std::cout << "New data available." << std::endl;
	
	uint64_t timestamp = (*statistics->begin())->curTimestamp();
	std::cout << "Timestmap: " << timestamp << std::endl;
	uint64_t size = statistics->size();
	std::cout << "Size: " << size << std::endl;

	// write out all currently existing statistics
	for(auto itr = statistics->begin(); itr != statistics->end(); itr++) {
		StatisticsDescription &sd = **itr;
//		std::cout << "Description: " << sd << std::endl;
		std::cout << "Value: " << (*itr)->curValue << std::endl;
	}
}

extern "C" {
	void* MONITORING_STATISTICS_MULTIPLEXER_PLUGIN_INSTANCIATOR_NAME() {
		return new StatisticsPostgreSQLWriter();
	}
}
