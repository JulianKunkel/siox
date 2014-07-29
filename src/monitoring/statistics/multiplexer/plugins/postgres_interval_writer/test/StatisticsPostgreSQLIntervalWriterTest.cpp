#include <libpq-fe.h>


#include <iostream>

#include <core/module/ModuleLoader.hpp>
#include <core/reporting/ComponentReportInterface.hpp>
#include <core/db/DatabaseSetup.hpp>

#include <monitoring/statistics/collector/StatisticsProviderPlugin.hpp>
#include <monitoring/statistics/collector/StatisticsCollector.hpp>
#include <monitoring/statistics/collector/ThreadedStatisticsOptions.hpp>

#include <monitoring/statistics/multiplexer/StatisticsMultiplexer.hpp>
#include <monitoring/statistics/multiplexer/StatisticsMultiplexerPlugin.hpp>
#include <monitoring/topology/Topology.hpp>
#include <monitoring/ontology/Ontology.hpp>
#include <monitoring/ontology/modules/TopologyOntology/TopologyOntologyOptions.hpp>
#include <monitoring/statistics/multiplexer/plugins/postgres_interval_writer/StatisticsPostgreSQLIntervalWriterOptions.hpp>

// The string to connect to the DB
// We assume the local username has access to the DB
// 
// Supported connection string
// host = '127.0.0.1' port = '5432' user = 'postgres' password = 'siox.db.X916' dbname = 'siox'
#define DBINFO " dbname = 'sioxtest' "

using namespace std;

using namespace monitoring;
using namespace core;


/*
 This test checks the whole chain, from creation of events through draining the events into a smux plugin.
 */
int main( int argc, char const * argv[] )
{
	// data source
	StatisticsProviderPlugin * provider = module_create_instance<StatisticsProviderPlugin>( "", "siox-monitoring-statisticsPlugin-OSMemUsage" , MONITORING_STATISTICS_PLUGIN_INTERFACE );
	StatisticsCollector * collector = module_create_instance<StatisticsCollector>( "", "siox-monitoring-ThreadedStatisticsCollector", STATISTICS_COLLECTOR_INTERFACE );

	Ontology * ontology = core::module_create_instance<Ontology>( "", "siox-monitoring-TopologyOntology", ONTOLOGY_INTERFACE );
	Topology* topology = module_create_instance<Topology>( "", "siox-monitoring-RamTopology", MONITORING_TOPOLOGY_INTERFACE );


	// drain
	StatisticsMultiplexer * multiplexer = module_create_instance<StatisticsMultiplexer>( "", "siox-monitoring-StatisticsMultiplexerSync", STATISTICS_MULTIPLEXER_INTERFACE );
	StatisticsMultiplexerPlugin * mplexer_plugin = module_create_instance<StatisticsMultiplexerPlugin>( "", "siox-monitoring-statisticsMultiplexerPlugin-PostgreSQLIntervalWriter", STATISTICS_MULTIPLEXER_PLUGIN_INTERFACE );


	// initialize options
	{
		TopologyOntologyOptions * op = new TopologyOntologyOptions();
		op->topology.componentPointer = topology ;
		ontology->init( op );
	}

	{
		ThreadedStatisticsOptions & op = dynamic_cast<ThreadedStatisticsOptions &>(collector->getOptions());
		op.topology = topology;
		op.ontology = ontology;
		op.smux = multiplexer;
	}

	{
		StatisticsProviderPluginOptions & op = dynamic_cast<StatisticsProviderPluginOptions &>(provider->getOptions());
		op.statisticsCollector = collector;
	}

	{
		StatisticsPostgreSQLIntervalWriterOptions & op = dynamic_cast<StatisticsPostgreSQLIntervalWriterOptions &>(mplexer_plugin->getOptions());
		op.multiplexer = multiplexer;
		op.dbinfo = DBINFO;
		op.statisticsInterval = 10;
	}

	// clean the test DB
	DatabaseSetup * db_setup = dynamic_cast<DatabaseSetup*>(mplexer_plugin);
	db_setup->cleanDatabase();
 	db_setup->prepareDatabaseIfNecessary();

	// now initialize all plugins

	topology->init();
	collector->init();
	provider->init();
	multiplexer->init();
	mplexer_plugin->init();

	// start all threaded functions

	collector->start();
	provider->start();
	multiplexer->start();
	//mplexer_plugin->start();

	// main test
	// wait 3 seconds
	sleep(3);

	// check correctness

	ComponentReportInterface * cri = dynamic_cast<ComponentReportInterface*>(mplexer_plugin);
	ComponentReport cr = cri->prepareReport();

	for (auto key = cr.data.begin() ; key != cr.data.end(); key++){
		cout << key->first->name << " " << key->second.value << endl;
	}

	// end test

	// TODO correct reverse order of dependencies
	collector->stop();
	provider->stop();
	multiplexer->stop();
	mplexer_plugin->stop();
	topology->stop();
	ontology->stop();

	collector->finalize();
	provider->finalize();
	multiplexer->finalize();
	mplexer_plugin->finalize();
	topology->finalize();
	ontology->finalize();
	
	
	delete( topology );
	delete( ontology );
	delete( provider );
	delete( collector );
	delete( multiplexer );
	delete( mplexer_plugin );


	// determine number of created stats:

	auto dbconn = PQconnectdb(DBINFO);
	auto res = PQexec(dbconn, "select count(*) from aggregate.statistics;");

	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		cerr << "Error determining count from statistic: " << PQresultErrorMessage(res) << endl;
	}else{
		cout << "TEST INTERFACE: Statistics entries stored in the database: " << PQgetvalue(res, 0, 0) << endl;
	}
	PQclear(res); 		

	cout << "OK" << endl;
	return 0;
}


