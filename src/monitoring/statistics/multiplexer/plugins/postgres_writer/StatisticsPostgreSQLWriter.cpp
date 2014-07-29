#include <iostream>
#include <mutex>
#include "libpq-fe.h"

#include <core/persist/SetupPersistentStructures.hpp>

#include <monitoring/statistics/multiplexer/StatisticsMultiplexer.hpp>
#include <monitoring/statistics/multiplexer/StatisticsMultiplexerPluginImplementation.hpp>
#include <monitoring/statistics/StatisticTypesSerializableText.cpp>
#include <util/Util.hpp>

#include "StatisticsPostgreSQLWriterOptions.hpp"

using namespace std;
using namespace monitoring;
using namespace core;

class StatisticsPostgreSQLWriter : public StatisticsMultiplexerPlugin, public SetupPersistentStructures {
public:
	virtual void initPlugin() throw() override;
	virtual ComponentOptions *AvailableOptions() override;
	virtual void notifyAvailableStatisticsChange(const vector<shared_ptr<Statistic> > &statistics, bool addedStatistics, bool removedStatistics) throw() override;
	virtual void newDataAvailable() throw() override;
	
	virtual int preparePersistentStructuresIfNecessary() override;
	virtual int cleanPersistentStructures() override;

	~StatisticsPostgreSQLWriter();
private:
	const vector<shared_ptr<Statistic> > *statistics;
	PGconn *dbconn_;
	mutex mtx_;
};


int StatisticsPostgreSQLWriter::preparePersistentStructuresIfNecessary(){
	if (PQstatus(dbconn_) != CONNECTION_OK) {
		StatisticsPostgreSQLWriterOptions &o = getOptions<StatisticsPostgreSQLWriterOptions>();
		dbconn_ = PQconnectdb(o.dbinfo.c_str());
	}

 	PGresult *res = PQexec(dbconn_, "CREATE SCHEMA statistics;");
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		cerr << "ERROR could not setup the database: " << PQresultErrorMessage(res) << endl;
	}
	int ret = true;
	res = PQexec(dbconn_, 
		"CREATE TABLE statistics.stats (" 
    	"stat_id bigint NOT NULL,"
    	"topology_id bigint,"
    	"ontology_id bigint,"
    	"value float,"
    	"timestamp bigint)" ); 
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		cerr << "ERROR could not setup the database: " << PQresultErrorMessage(res) << endl;
		ret = false;
	}
 	PQclear(res);

 	return ret;
}

int StatisticsPostgreSQLWriter::cleanPersistentStructures(){
	if (PQstatus(dbconn_) != CONNECTION_OK) {
		StatisticsPostgreSQLWriterOptions &o = getOptions<StatisticsPostgreSQLWriterOptions>();
		dbconn_ = PQconnectdb(o.dbinfo.c_str());
	}
	int ret = true;
 	PGresult *res = PQexec(dbconn_, "drop table statistics.stats;");
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		cerr << "ERROR could not drop the database: " << PQresultErrorMessage(res) << endl;
		ret = false;
	}
 	PQclear(res);

 	return ret;
}


StatisticsPostgreSQLWriter::~StatisticsPostgreSQLWriter()
{
	std::cout << "PostgreSQL Statistics Writer stopping..." << std::endl;
 	PQfinish(dbconn_);
}


void StatisticsPostgreSQLWriter::initPlugin() throw()
{
	std::cout << "PostgreSQL Statistic Writer initializing..." << std::endl;

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
	stringstream s;
	s << "INSERT INTO statistics.stats (topology_id, ontology_id, value, timestamp) VALUES ";

	// write out all currently existing statistics
	for (unsigned i=0; i < statistics->size() ; i++ ){

		if (i != 0){
			s << ",";
		}
		Statistic &sd = *(*statistics)[i];

		s << "(" << sd.topologyId << "," << sd.ontologyId << "," << sd.curValue.toStr() << "," << sd.curTimestamp() << ")";
	}

	PGresult * res = PQexec(dbconn_, s.str().c_str() );

	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		cerr << "Error inserting statistic. Postgres error message: " << PQresultErrorMessage(res) << endl;
	}

	PQclear(res);

}

extern "C" {
	void* MONITORING_STATISTICS_MULTIPLEXER_PLUGIN_INSTANCIATOR_NAME() {
		return new StatisticsPostgreSQLWriter();
	}
}
