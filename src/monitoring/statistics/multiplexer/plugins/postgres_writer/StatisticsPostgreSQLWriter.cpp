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
	virtual void initPlugin() throw() override;
	virtual ComponentOptions *AvailableOptions() override;
	virtual void notifyAvailableStatisticsChange(const vector<shared_ptr<Statistic> > &statistics, bool addedStatistics, bool removedStatistics) throw() override;
	virtual void newDataAvailable() throw() override;
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
	// uint64_t size = statistics->size();

	const int nparams = 4;

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
