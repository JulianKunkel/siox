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

	(void)topologyOptions;	//avoid warning about unused variable
	facadeOptions->topology.componentPointer = topology;
	facadeOptions->ontology.componentPointer = ontology;
	ontologyOptions->filename = "optimizer-ontology-example.dat";
	cpuProviderOptions->statisticsCollector.componentPointer = collector;
	networkProviderOptions->statisticsCollector.componentPointer = collector;
	ioProviderOptions->statisticsCollector.componentPointer = collector;
	ramProviderOptions->statisticsCollector.componentPointer = collector;
	collectorOptions->topology = topology;
	collectorOptions->ontology.componentPointer = ontology;
	collectorOptions->smux.componentPointer = multiplexer;
	(void)multiplexerOptions;	//avoid warning about unused variable
	integratorOptions->statisticsCollector.componentPointer = collector;
	integratorOptions->multiplexer.componentPointer = multiplexer;
	integratorOptions->dereferencingFacade.componentPointer = facade;
	integratorOptions->availableIoBandwidth = 100ull << 20;

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

	cerr << "sleeping\n";
	sleep( 1 );
	cerr << "waking up\n";

	//Theoretically, we should be checking the output here for sensibility. But that is hard to do because there is no way to decide if a value is correct, apart from recalculating it. So we only do a functional test here.

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

