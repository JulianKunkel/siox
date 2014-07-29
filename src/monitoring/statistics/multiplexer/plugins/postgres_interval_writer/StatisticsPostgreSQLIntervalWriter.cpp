#include <iostream>
#include <mutex>
#include <libpq-fe.h>
#include <sstream>

#include <core/reporting/ComponentReportInterface.hpp>
#include <core/db/DatabaseSetup.hpp>

#include <monitoring/statistics/multiplexer/StatisticsMultiplexer.hpp>
#include <monitoring/statistics/multiplexer/StatisticsMultiplexerPluginImplementation.hpp>
#include <monitoring/statistics/StatisticTypesSerializableText.cpp>
#include <util/Util.hpp>
#include <util/time.h>

#include "StatisticsPostgreSQLIntervalWriterOptions.hpp"

using namespace std;
using namespace monitoring;
using namespace core;

/**
 * This class dumps statistics into Postgres but does not store every interval as one 
 * result but instead takes an configuratable interval e.g. 1s and stores these results.
 * This reduces the burden on the Postgres DB and the overhead.
 */
class StatisticsPostgreSQLIntervalWriter : public StatisticsMultiplexerPlugin, public ComponentReportInterface, public DatabaseSetup {
private:
	struct StatisticSummary{
		double min = 1e306;
		double max = 0;
		double sum = 0;
	};
public:
	virtual void initPlugin() throw() override;
	virtual ComponentOptions *AvailableOptions() override;
	virtual void notifyAvailableStatisticsChange(const vector<shared_ptr<Statistic> > &statistics, bool addedStatistics, bool removedStatistics) throw() override;
	virtual void newDataAvailable() throw() override;
	~StatisticsPostgreSQLIntervalWriter();

	virtual ComponentReport prepareReport() override;

	virtual void prepareDatabaseIfNecessary() override;
	virtual void cleanDatabase() override;

private:
	const vector<shared_ptr<Statistic> > *statistics;
	vector<StatisticSummary> aggregates;
	Timestamp intervalLength;
	Timestamp lastTriggeredTime = 0;
	int intervalUpdateCount = 0;

	PGconn *dbconn_;
	mutex mtx_;

	// statistics about operation
	int intervalsReported = 0;	
	int intervalsTriggered = 0;
	Timestamp totalExecutionTime = 0;
};

void StatisticsPostgreSQLIntervalWriter::prepareDatabaseIfNecessary(){
 	PGresult *res = PQexec(dbconn_, "CREATE SCHEMA aggregate;");
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		cerr << "ERROR could not setup the database: " << PQresultErrorMessage(res) << endl;
	}
	res = PQexec(dbconn_, 
		"CREATE TABLE aggregate.statistics (" 
	   " time_begin bigint," 
	   " time_end bigint," 
	   " topology_id bigint," 
	   " ontology_id bigint," 
	   " average float," 
	   " min float," 
	   " max float );"
 		);
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		cerr << "ERROR could not setup the database: " << PQresultErrorMessage(res) << endl;
	}


 	PQclear(res);
}

void StatisticsPostgreSQLIntervalWriter::cleanDatabase(){
	if (PQstatus(dbconn_) != CONNECTION_OK) {
		StatisticsPostgreSQLIntervalWriterOptions &o = getOptions<StatisticsPostgreSQLIntervalWriterOptions>();
		dbconn_ = PQconnectdb(o.dbinfo.c_str());
	}
 	PGresult *res = PQexec(dbconn_, "drop table aggregate.statistics;");
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		cerr << "ERROR could not drop the database: " << PQresultErrorMessage(res) << endl;
	}
 	PQclear(res);
}

ComponentReport StatisticsPostgreSQLIntervalWriter::prepareReport()
{
	ComponentReport rep;
	rep.addEntry("Intervals reported",  {ReportEntry::Type::SIOX_INTERNAL_DEBUG,  intervalsReported} );
	rep.addEntry("Intervals triggered", {ReportEntry::Type::SIOX_INTERNAL_DEBUG,  intervalsTriggered} );

	int statisticsReported = aggregates.size();
	rep.addEntry("Statistics reported", {ReportEntry::Type::SIOX_INTERNAL_DEBUG,  statisticsReported} );

	double overheadTime = siox_time_in_s(totalExecutionTime);
	rep.addEntry("Processing time", {ReportEntry::Type::SIOX_INTERNAL_PERFORMANCE,  overheadTime});
	return rep;
}



StatisticsPostgreSQLIntervalWriter::~StatisticsPostgreSQLIntervalWriter()
{
	std::cout << "PostgreSQL StatisticIntervalWriter stopping..." << std::endl;
 	if (PQstatus(dbconn_) == CONNECTION_OK) {
	 	PQfinish(dbconn_);
	 }
}

void StatisticsPostgreSQLIntervalWriter::initPlugin() throw()
{
	std::cout << "PostgreSQL StatisticIntervalWriter initializing..." << std::endl;

	StatisticsPostgreSQLIntervalWriterOptions &o = getOptions<StatisticsPostgreSQLIntervalWriterOptions>();

 	dbconn_ = PQconnectdb(o.dbinfo.c_str());

 	intervalLength = convert_seconds_to_timestamp( o.intervalLengthInS );

 	assert(intervalLength >= 1000*1000ll*1000);

 	if (PQstatus(dbconn_) != CONNECTION_OK) {
 		cerr << "Connection to database failed: " << PQerrorMessage(dbconn_) << endl;
 	}
}


ComponentOptions* StatisticsPostgreSQLIntervalWriter::AvailableOptions()
{
	return new StatisticsPostgreSQLIntervalWriterOptions();
}


void StatisticsPostgreSQLIntervalWriter::notifyAvailableStatisticsChange(const vector<shared_ptr<Statistic> > &statistics, bool addedStatistics, bool removedStatistics) throw ()
{
	this->statistics = &statistics;

	// update aggregate
	this->aggregates.resize(statistics.size());
}

void StatisticsPostgreSQLIntervalWriter::newDataAvailable() throw()
{
	intervalsTriggered++;

 	if (PQstatus(dbconn_) != CONNECTION_OK) {
 		return;
 	}

	Timestamp curTime = (*statistics->begin())->curTimestamp();

 	Timestamp start = siox_gettime();

	// ignore the first timestamp
	if ( lastTriggeredTime == 0 ){
		lastTriggeredTime = curTime;
		return;
	}

	// update aggregates
	for (unsigned i=0; i < statistics->size() ; i++ ){
		double val = (*statistics)[i]->curValue.toFloat();
		aggregates[i].min = aggregates[i].min < val ? aggregates[i].min : val;
		aggregates[i].max = aggregates[i].max > val ? aggregates[i].max : val;
		aggregates[i].sum += val;
	}

	intervalUpdateCount++;

	if ( curTime - lastTriggeredTime < intervalLength ){
		return;
	}
	
	intervalsReported++;


	const int nparams = 7;

	stringstream s;

	s << "INSERT INTO aggregate.statistics (time_begin, time_end, topology_id, ontology_id, average, min, max) VALUES ";

	// write out all currently existing statistics
	for (unsigned i=0; i < statistics->size() ; i++ ){

		if (i != 0){
			s << ",";
		}

		Statistic &sd = *(*statistics)[i];
		s << "(" << lastTriggeredTime << "," << curTime << "," << sd.topologyId << "," << sd.ontologyId << "," << (aggregates[i].sum / intervalUpdateCount) << "," << aggregates[i].min  << "," << aggregates[i].max << ")";
		// TODO check accuracy

		aggregates[i].min = 1e307;
		aggregates[i].max = 0;
		aggregates[i].sum = 0;
	}

	PGresult * res = PQexec(dbconn_, s.str().c_str() );

	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		cerr << "Error inserting statistic. Postgres error message: " << PQresultErrorMessage(res) << endl;
	}

	PQclear(res);

	intervalUpdateCount = 0;
	lastTriggeredTime = curTime;

	totalExecutionTime += siox_gettime() - start;	
}

extern "C" {
	void* MONITORING_STATISTICS_MULTIPLEXER_PLUGIN_INSTANCIATOR_NAME() {
		return new StatisticsPostgreSQLIntervalWriter();
	}
}
