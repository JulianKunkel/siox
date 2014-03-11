#include <iostream>
#include <mutex>
#include "libpq-fe.h"

#include <monitoring/statistics/multiplexer/StatisticsMultiplexer.hpp>
#include <monitoring/statistics/multiplexer/StatisticsMultiplexerPluginImplementation.hpp>
#include <monitoring/statistics/StatisticTypesSerializableText.cpp>
#include <util/Util.hpp>

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
	PGconn *dbconn_;
	mutex mtx_;
};


StatisticsPostgreSQLWriter::~StatisticsPostgreSQLWriter()
{
	std::cout << "PostgreSQL Statistics Writter stopping..." << std::endl;
 	PQfinish(dbconn_);
}


void StatisticsPostgreSQLWriter::initPlugin() throw() 
{
	std::cout << "PostgreSQL Statistic Writter initializing..." << std::endl;
	
	StatisticsPostgreSQLWriterOptions &o = getOptions<StatisticsPostgreSQLWriterOptions>();
		
 	dbconn_ = PQconnectdb(o.dbinfo.c_str());
		
 	if (PQstatus(dbconn_) != CONNECTION_OK) {
 		std::cerr << "Connection to database failed: " << PQerrorMessage(dbconn_) << std::endl;
 	}
}


ComponentOptions* StatisticsPostgreSQLWriter::AvailableOptions() 
{
	return new StatisticsPostgreSQLWriterOptions();
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
	
	const int nparams = 4;
	const char *command = 
		"INSERT INTO statistics.stats (topology_id, ontology_id, value, timestamp) VALUES ($1::int4, $2::int4, $3, $4::int8)";

	uint64_t tim = util::htonll(timestamp);

	// write out all currently existing statistics
	for (auto itr = statistics->begin(); itr != statistics->end(); itr++) {

		StatisticsDescription &sd = **itr;
		uint32_t tid = htonl(sd.topologyId);
		uint32_t oid = htonl(sd.ontologyId);
		std::string val = (*itr)->curValue.toStr();

		const char *param_values[nparams] = {(char *) &tid, (char *) &oid, val.length() == 0 || val == "0" ? NULL : val.c_str(), (char *) &tim};
		int param_lengths[nparams] = {sizeof(tid), sizeof(oid), (int) val.length(), sizeof(tim)};
		int param_formats[nparams] = {1, 1, 0, 1};
		int result_format = 1;

		PGresult *res = PQexecParams(dbconn_, command, nparams, NULL, param_values, param_lengths, param_formats, result_format);

		if (PQresultStatus(res) != PGRES_COMMAND_OK) {
			std::cerr << "Error inserting statistic: " << PQresultErrorMessage(res) << std::endl;	
		}

		PQclear(res);
//		std::cout << "Description: " << sd << std::endl;
//		std::cout << "Value: " << (*itr)->curValue << std::endl;
	}
}

extern "C" {
	void* MONITORING_STATISTICS_MULTIPLEXER_PLUGIN_INSTANCIATOR_NAME() {
		return new StatisticsPostgreSQLWriter();
	}
}
