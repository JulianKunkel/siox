#include <iostream>
#include <mutex>
#include <libpq-fe.h>

#include <core/reporting/ComponentReportInterface.hpp>

#include <monitoring/statistics/multiplexer/StatisticsMultiplexer.hpp>
#include <monitoring/statistics/multiplexer/StatisticsMultiplexerPluginImplementation.hpp>
#include <monitoring/statistics/StatisticTypesSerializableText.cpp>
#include <util/Util.hpp>


#include "StatisticsPostgreSQLIntervalWriterOptions.hpp"

using namespace std;
using namespace monitoring;
using namespace core;

/**
 * This class dumps statistics into Postgres but does not store every interval as one 
 * result but instead takes an configuratable interval e.g. 1s and stores these results.
 * This reduces the burden on the Postgres DB and the overhead.
 */
class StatisticsPostgreSQLIntervalWriter : public StatisticsMultiplexerPlugin, public ComponentReportInterface {
public:
	virtual void initPlugin() throw() override;
	virtual ComponentOptions *AvailableOptions() override;
	virtual void notifyAvailableStatisticsChange(const vector<shared_ptr<Statistic> > &statistics, bool addedStatistics, bool removedStatistics) throw() override;
	virtual void newDataAvailable() throw() override;
	~StatisticsPostgreSQLIntervalWriter();

	virtual ComponentReport prepareReport() override;
	virtual void stop() override;

private:
	const vector<shared_ptr<Statistic> > *statistics;
	int statisticsIterationsBeforeSync;
	int currentIterations = 0;
	PGconn *dbconn_;
	mutex mtx_;

	// statistics about operation
	int intervalsReported = 0;	
	int intervalsTriggered = 0;
	int statisticsReported = 0;
};

ComponentReport StatisticsPostgreSQLIntervalWriter::prepareReport()
{
	ComponentReport rep;
	rep.addEntry("Intervals reported",  {ReportEntry::Type::SIOX_INTERNAL_DEBUG,  intervalsReported} );
	rep.addEntry("Intervals triggered", {ReportEntry::Type::SIOX_INTERNAL_DEBUG,  intervalsTriggered} );
	rep.addEntry("Statistics reported", {ReportEntry::Type::SIOX_INTERNAL_DEBUG,  statisticsReported} );
	return rep;
}



StatisticsPostgreSQLIntervalWriter::~StatisticsPostgreSQLIntervalWriter()
{
	std::cout << "PostgreSQL StatisticIntervalWriter stopping..." << std::endl;
 	if (PQstatus(dbconn_) == CONNECTION_OK) {
	 	PQfinish(dbconn_);
	 }
}

void StatisticsPostgreSQLIntervalWriter::stop() {
	StatisticsPostgreSQLIntervalWriterOptions &o = getOptions<StatisticsPostgreSQLIntervalWriterOptions>();	

 	if (o.thisIsATest){
 		// clean table
 		PGresult *res = PQexec(dbconn_, "select count(*) from aggregate.statistics;");

 		if (PQresultStatus(res) != PGRES_TUPLES_OK) {
			cerr << "Error determining count from statistic: " << PQresultErrorMessage(res) << endl;
		}else{
			cout << "TEST INTERFACE: Statistics entries stored in the database: " << PQgetvalue(res, 0, 0) << endl;
		}
 		PQclear(res); 		
 	}
}

void StatisticsPostgreSQLIntervalWriter::initPlugin() throw()
{
	std::cout << "PostgreSQL StatisticIntervalWriter initializing..." << std::endl;

	StatisticsPostgreSQLIntervalWriterOptions &o = getOptions<StatisticsPostgreSQLIntervalWriterOptions>();

 	dbconn_ = PQconnectdb(o.dbinfo.c_str());

 	statisticsIterationsBeforeSync = o.statisticsInterval;

 	assert(statisticsIterationsBeforeSync > 1);

 	if (PQstatus(dbconn_) != CONNECTION_OK) {
 		std::cerr << "Connection to database failed: " << PQerrorMessage(dbconn_) << std::endl;
 	}

 	if (o.thisIsATest){
 		// clean table
 		PGresult *res = PQexec(dbconn_, "delete from aggregate.statistics ;");
 		PQclear(res);
 	}
}


ComponentOptions* StatisticsPostgreSQLIntervalWriter::AvailableOptions()
{
	return new StatisticsPostgreSQLIntervalWriterOptions();
}


void StatisticsPostgreSQLIntervalWriter::notifyAvailableStatisticsChange(const vector<shared_ptr<Statistic> > &statistics, bool addedStatistics, bool removedStatistics) throw ()
{
	this->statistics = &statistics;
}

void StatisticsPostgreSQLIntervalWriter::newDataAvailable() throw()
{
	intervalsTriggered++;

	currentIterations++;
	if (currentIterations < statisticsIterationsBeforeSync){
		return;
	}
	currentIterations = 0;

	intervalsReported++;

	uint64_t timestamp = (*statistics->begin())->curTimestamp();

	const int nparams = 7;
	const char *command =
		"INSERT INTO aggregate.statistics (time_begin, time_end, topology_id, ontology_id, average, min, max) VALUES ($1::int8, $2::int8, $3::int4, $4::int4, $5, $6, $7)";

	uint64_t tim = util::htonll(timestamp);

	// write out all currently existing statistics
	for (auto itr = statistics->begin(); itr != statistics->end(); itr++) {

		StatisticsDescription &sd = **itr;
		uint32_t tid = htonl(sd.topologyId);
		uint32_t oid = htonl(sd.ontologyId);

		string average = (*itr)->curValue.toStr();

		const char *param_values[nparams] = {(char *) &tim, (char *) &tim, (char *) &tid, (char *) &oid, average.c_str(), average.c_str(), average.c_str() };
		int param_lengths[nparams] = {sizeof(tim), sizeof(tim), sizeof(tid), sizeof(oid), (int) average.length(), (int) average.length(), (int) average.length()};

		int param_formats[nparams] = {1, 1, 1, 1, 0, 0, 0};
		int result_format = 1;

		PGresult *res = PQexecParams(dbconn_, command, nparams, NULL, param_values, param_lengths, param_formats, result_format);

		if (PQresultStatus(res) != PGRES_COMMAND_OK) {
			std::cerr << "Error inserting statistic: " << PQresultErrorMessage(res) << std::endl;
		}

		PQclear(res);
	}
}

extern "C" {
	void* MONITORING_STATISTICS_MULTIPLEXER_PLUGIN_INSTANCIATOR_NAME() {
		return new StatisticsPostgreSQLIntervalWriter();
	}
}
