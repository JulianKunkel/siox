/**
 * @file StatisticsTest.cpp
 *
 * @author Nathanael Hübbe
 * @date   2013
 */

#include <iostream>

#include <core/module/ModuleLoader.hpp>

#include <monitoring/statistics/collector/StatisticsProviderPlugin.hpp>
#include <monitoring/statistics/collector/StatisticsCollector.hpp>

#include <monitoring/statistics/Statistic.hpp>
#include <monitoring/statistics/multiplexer/StatisticsMultiplexer.hpp>
#include <monitoring/statistics/StatisticsIntegrator.hpp>
#include <monitoring/topology/Topology.hpp>
#include <monitoring/ontology/Ontology.hpp>
#include <knowledge/activity_plugin/ActivityPluginDereferencing.hpp>
#include <knowledge/activity_plugin/DereferencingFacadeOptions.hpp>

#include <monitoring/statistics/multiplexer/StatisticsMultiplexerPlugin.hpp>
#include <../src/monitoring/ontology/modules/file-ontology/FileOntologyOptions.hpp>
#include <../src/monitoring/statistics/collector/ThreadedStatisticsOptions.hpp>
#include <../src/monitoring/statistics/multiplexer/StatisticsMultiplexerSyncOptions.hpp>
#include "../QualitativeUtilizationOptions.hpp"


using namespace core;
using namespace monitoring;

int main( int argc, char const * argv[] ) throw() {
	cerr << "Checkpoint 2\n";
	#define makeModule(Type, PointerName, ModuleName, InterfaceName) \
		Type* PointerName = module_create_instance<Type>( "", ModuleName, InterfaceName)
	makeModule( Topology, topology, "siox-monitoring-RamTopology", MONITORING_TOPOLOGY_INTERFACE );
	makeModule( ActivityPluginDereferencing, facade, "siox-knowledge-DereferencingFacade", ACTIVITY_DEREFERENCING_FACADE_INTERFACE );
	makeModule( Ontology, ontology, "siox-monitoring-FileOntology", ONTOLOGY_INTERFACE );
	makeModule( StatisticsProviderPlugin, cpuProvider, "siox-monitoring-statisticsPlugin-CPUstats", MONITORING_STATISTICS_PLUGIN_INTERFACE );
	makeModule( StatisticsProviderPlugin, networkProvider, "siox-monitoring-statisticsPlugin-network", MONITORING_STATISTICS_PLUGIN_INTERFACE );
	makeModule( StatisticsProviderPlugin, ioProvider, "siox-monitoring-statisticsPlugin-IOstats", MONITORING_STATISTICS_PLUGIN_INTERFACE );
	makeModule( StatisticsProviderPlugin, ramProvider, "siox-monitoring-statisticsPlugin-OSMemUsage", MONITORING_STATISTICS_PLUGIN_INTERFACE );
	makeModule( StatisticsCollector, collector, "siox-monitoring-ThreadedStatisticsCollector", STATISTICS_COLLECTOR_INTERFACE );
	makeModule( StatisticsMultiplexer, multiplexer, "siox-monitoring-StatisticsMultiplexerSync", STATISTICS_MULTIPLEXER_INTERFACE );
	makeModule( StatisticsIntegrator, integrator, "siox-monitoring-statisticsPlugin-qualitativeUtilization", MONITORING_STATISTICS_PLUGIN_INTERFACE );

	cerr << "Checkpoint 3\n";
	#define fetchOptions(Type, ComponentVariableName) \
		Type* ComponentVariableName##Options = &ComponentVariableName->getOptions<Type>();
	fetchOptions( ComponentOptions, topology );
	fetchOptions( DereferencingFacadeOptions, facade );
	fetchOptions( FileOntologyOptions, ontology );
	fetchOptions( StatisticsProviderPluginOptions, cpuProvider );
	fetchOptions( StatisticsProviderPluginOptions, networkProvider );
	fetchOptions( StatisticsProviderPluginOptions, ioProvider );
	fetchOptions( StatisticsProviderPluginOptions, ramProvider );
	fetchOptions( ThreadedStatisticsOptions, collector );
	fetchOptions( StatisticsMultiplexerSyncOptions, multiplexer );
	fetchOptions( QualitativeUtilizationOptions, integrator );

	cerr << "Checkpoint 4\n";
	(void)topologyOptions;	//avoid warning about unused variable
	facadeOptions->topology.componentPointer = topology;
	facadeOptions->ontology.componentPointer = ontology;
	ontologyOptions->filename = "optimizer-ontology-example.dat";
	cpuProviderOptions->statisticsCollector.componentPointer = collector;
	networkProviderOptions->statisticsCollector.componentPointer = collector;
	ioProviderOptions->statisticsCollector.componentPointer = collector;
	ramProviderOptions->statisticsCollector.componentPointer = collector;
	collectorOptions->dereferencingFacade.componentPointer = facade;
	collectorOptions->ontology.componentPointer = ontology;
	collectorOptions->smux.componentPointer = multiplexer;
	(void)multiplexerOptions;	//avoid warning about unused variable
	integratorOptions->statisticsCollector.componentPointer = collector;
	integratorOptions->multiplexer.componentPointer = multiplexer;
	integratorOptions->dereferencingFacade.componentPointer = facade;
	integratorOptions->availableIoBandwidth = 100ull << 20;
	integratorOptions->ioBlockSize = 512;

	cerr << "Checkpoint 5\n";
	topology->init();
	ontology->init();
	facade->init();
	multiplexer->init();
	collector->init();
	cpuProvider->init();
	networkProvider->init();
	ioProvider->init();
	ramProvider->init();
	integrator->init();

	cerr << "Checkpoint 6\n";
//	{
//		vector<shared_ptr<Statistic> > statistics = collector->availableMetrics();
//		assert( statistics.size() == 3 );
//		statistics[0]->requestReduction( SUM );
//		statistics[1]->requestReduction( SUM );
//		statistics[2]->requestReduction( SUM );
//	}

	cerr << "sleeping\n";
	sleep( 1 );
	cerr << "waking up\n";

//	{
//		vector<shared_ptr<Statistic> > statistics = collector->availableMetrics();
//		assert( statistics.size() == 3 );
//		array<StatisticsValue, Statistic::kHistorySize> values[3];
//		statistics[0]->getHistoricValues( SUM, HUNDRED_MILLISECONDS, &values[0], NULL );
//		statistics[1]->getHistoricValues( SUM, HUNDRED_MILLISECONDS, &values[1], NULL );
//		statistics[2]->getHistoricValues( SUM, HUNDRED_MILLISECONDS, &values[2], NULL );
//		double expectedSum = 0;
//		for(size_t i = 0; i < Statistic::kHistorySize; i++) {
//			double expectedValue = 0.8*(1 << values[2][i].int32())/2;
//			expectedSum += expectedValue;
//			assert( values[0][i] == expectedValue );
//			assert( values[1][i] == expectedValue );
//		}
//
////		StatisticsDescription description( ontology->lookup_attribute_by_name( "Statistics", "test/weather" ).aID, {{"node", LOCAL_HOSTNAME}, {"tschaka", "test2"}} );
//		StatisticsDescription description( ontology->lookup_attribute_by_name( "Statistics", "test/weather" ).aID, topology->lookupObjectByPath( "@localhost/weather-station:0" ).id() );
//		array<StatisticsValue, Statistic::kHistorySize> nameLookupValues = collector->getStatistics( SUM, HUNDRED_MILLISECONDS, description );
//		assert( values[1] == nameLookupValues );
//		StatisticsValue aggregatedValue = collector->getReducedStatistics( SUM, SECOND, *statistics[0] );
//		assert( aggregatedValue == expectedSum );
//
//		StatisticsValue rollingValue = collector->getRollingStatistics( SUM, SECOND, *statistics[0] );
//		expectedSum = 0;
//		for(size_t i = 20; i --> 10; ) expectedSum += 0.8*(1 << i);
//		assert( rollingValue == expectedSum );
//
//		assert( listener->registeredValidInput() );
//	}

	cpuProvider->finalize();
	networkProvider->finalize();
	ioProvider->finalize();
	ramProvider->finalize();
	integrator->finalize();
	collector->finalize();
	multiplexer->finalize();
	ontology->finalize();
	topology->finalize();

	delete cpuProvider;
	delete networkProvider;
	delete ioProvider;
	delete ramProvider;
	delete integrator;
	delete collector;
	delete multiplexer;
	delete ontology;
	delete topology;

	cerr << "OK" << endl;
	return 0;
}

