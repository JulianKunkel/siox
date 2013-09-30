#include <iostream>

#include <core/module/ModuleLoader.hpp>

#include <monitoring/statistics_collector/StatisticsProviderPlugin.hpp>
#include <monitoring/statistics_collector/StatisticsCollector.hpp>
#include <monitoring/statistics_collector/Statistic.hpp>
#include <monitoring/ontology/Ontology.hpp>
#include "../../../monitoring/ontology/modules/file-ontology/FileOntologyOptions.hpp"

#include "../ThreadedStatisticsOptions.hpp"


int main( int argc, char const * argv[] ) {
	monitoring::Ontology* ontology = core::module_create_instance<monitoring::Ontology>( "", "siox-monitoring-FileOntology", ONTOLOGY_INTERFACE );
	FileOntologyOptions* ontologyOptions = new FileOntologyOptions();
	ontologyOptions->filename = "optimizer-ontology-example.dat";
	ontology->init( ontologyOptions );

	monitoring::StatisticsCollector * collector = core::module_create_instance<monitoring::StatisticsCollector>( "", "siox-monitoring-ThreadedStatisticsCollector", STATISTICS_COLLECTOR_INTERFACE );
	monitoring::ThreadedStatisticsOptions* options = new monitoring::ThreadedStatisticsOptions();
	options->ontology.componentPointer = ontology;
	collector->init(options);

	monitoring::StatisticsProviderPlugin* plugin = module_create_instance<monitoring::StatisticsProviderPlugin>( "", "siox-monitoring-statisticsPlugin-providerskel", MONITORING_STATISTICS_PLUGIN_INTERFACE);
	plugin->init();

	collector->registerPlugin(plugin);
//	collector->availableMetrics();

	cerr << "sleeping\n";
	sleep(1);

	vector<shared_ptr<monitoring::Statistic> > statistics = collector->getStatistics();
	assert(statistics.size() == 3);
	array<monitoring::StatisticsValue, monitoring::Statistic::kHistorySize> values[3];
	statistics[0]->getHistoricValues(monitoring::HUNDRED_MILLISECONDS, &values[0], NULL);
	statistics[1]->getHistoricValues(monitoring::HUNDRED_MILLISECONDS, &values[1], NULL);
	statistics[2]->getHistoricValues(monitoring::HUNDRED_MILLISECONDS, &values[2], NULL);
	for(size_t i = 0; i < monitoring::Statistic::kHistorySize; i++) {
		double expectedValue = 0.8*(1<< values[2][i].int32())/2;
		assert(values[0][i].dbl() == expectedValue);
		assert(values[1][i].dbl() == expectedValue);
	}

	delete collector;
	delete plugin;
	delete ontology;

	cerr << "OK" << endl;
	return 0;
}


