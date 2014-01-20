#include <knowledge/activity_plugin/ActivityPluginDereferencing.hpp>
#include <monitoring/statistics/StatisticsIntegrator.hpp>

#include "QualitativeUtilizationOptions.hpp"

using namespace std;
using namespace monitoring;
using namespace core;

class QualitativeUtilization : public StatisticsIntegrator {
	public:
		ComponentOptions* AvailableOptions() override;
		virtual void nextTimestep() override;
		virtual vector<StatisticsProviderDatatypes> availableMetrics() override;
		virtual void initPlugin() throw() override;
		virtual void notifyAvailableStatisticsChange( const std::vector<std::shared_ptr<Statistic> > & statistics, bool addedStatistics, bool removedStatistics ) throw() override;
		virtual void newDataAvailable() throw() override;

	private:
		void fetchAttributeIds() throw();	//Caches the OntologyAttributeIDs and TopologyObjectIds that we need to filter the statistics. Operation may be incomplete, leaving a zero ID, valid IDs are not refetched.

		QualitativeUtilizationOptions* options = 0;

		bool AttributesComplete = false;
		OntologyAttributeID readDataAttribute = 0;
		OntologyAttributeID writtenDataAttribute = 0;
		OntologyAttributeID userTimeAttribute = 0;
		OntologyAttributeID niceTimeAttribute = 0;
		OntologyAttributeID systemTimeAttribute = 0;
		OntologyAttributeID idleTimeAttribute = 0;
		OntologyAttributeID iowaitTimeAttribute = 0;
		OntologyAttributeID interruptsTimeAttribute = 0;
		OntologyAttributeID softirqTimeAttribute = 0;
		OntologyAttributeID vmsTimeAttribute = 0;
		OntologyAttributeID vmsOsTimeAttribute = 0;
		TopologyTypeId blockDeviceType = 0;
		TopologyObjectId allCpusObject = 0;

		std::vector<std::shared_ptr<Statistic> > ioStatistics;
		uint64_t lastIoBlockCount = 0;	///@todo TODO: interprete the overflow_max_value and overflow_next_value fields of StatisticsProviderDatatypes
		std::vector<std::shared_ptr<Statistic> > busyCpuStatistics;
		uint64_t lastBusyCpuTime = 0;	///@todo TODO: interprete the overflow_max_value and overflow_next_value fields of StatisticsProviderDatatypes
		std::shared_ptr<Statistic> idleCpuStatistic;
		uint64_t lastIdleCpuTime = 0;	///@todo TODO: interprete the overflow_max_value and overflow_next_value fields of StatisticsProviderDatatypes

		StatisticsValue cpuUtilization = 0.0;
		StatisticsValue networkUtilization = 0.0;	//XXX: Should we split this into an upstream and a downstream utilization?
		StatisticsValue memoryUtilization = 0.0;
		StatisticsValue ioUtilization = 0.0;
};

ComponentOptions* QualitativeUtilization::AvailableOptions() {
	return new QualitativeUtilizationOptions();
}

//XXX: Currently, we aggregate the data in `newDataAvailable()`, because that guarantees that the inputs are all from the current iteration. However, it makes our output values change _while_ the listeners are informed of the new data. That means, that some listeners may still see the old values while others see the new ones. Integrating data in `nextTimestep()` would cause us to use inconsistent data to begin with. I cannot say whether it is a problem that some listeners get older data than others. I see only one fix to the inconsistency problem: calculate and cache the data in `newDataAvailable()`, and make it available when `nextTimestep()` is called. However, this would have the drawback that all our listeners would get data that is at least 0.1 seconds old.
void QualitativeUtilization::nextTimestep() {
}

vector<StatisticsProviderDatatypes> QualitativeUtilization::availableMetrics() {
	vector<StatisticsProviderDatatypes> result;

	result.push_back( {CPU, NODE, "utilization/cpu", "@localhost/utilization:cpu:percentage", cpuUtilization, GAUGE, "", "average cpu utilization on a node", 0, 0} );
	result.push_back( {MEMORY, NODE, "utilization/memory", "@localhost/utilization:memory:percentage", memoryUtilization, GAUGE, "", "memory utilization on a node", 0, 0} );
	result.push_back( {NETWORK, NODE, "utilization/network", "@localhost/utilization:network:percentage", networkUtilization, GAUGE, "", "average network utilization across all links of a node", 0, 0} );
	result.push_back( {INPUT_OUTPUT, NODE, "utilization/io", "@localhost/utilization:io:percentage", ioUtilization, GAUGE, "", "percentage of available I/O bandwidth used", 0, 0} );

	return result;
}

void QualitativeUtilization::initPlugin() throw() {
	options = &getOptions<QualitativeUtilizationOptions>();
}

void QualitativeUtilization::notifyAvailableStatisticsChange( const std::vector<std::shared_ptr<Statistic> > & statistics, bool addedStatistics, bool removedStatistics ) throw() {
	if( !readDataAttribute ) fetchAttributeIds();
	Topology* topology = GET_INSTANCE( ActivityPluginDereferencing, options->dereferencingFacade )->topology();

	ioStatistics.clear();
	lastIoBlockCount = 0;
	busyCpuStatistics.clear();
	lastBusyCpuTime = 0;
	idleCpuStatistic = NULL;
	lastIdleCpuTime = 0;

	for( size_t i = statistics.size(); i--; ) {
		const std::shared_ptr<Statistic>& curStatistic = statistics[i];
		OntologyAttributeID curAttribute = curStatistic->ontologyId;
		TopologyObjectId curTopology = curStatistic->topologyId;
		if( !curAttribute ) continue;
		if( readDataAttribute == curAttribute || writtenDataAttribute == curAttribute ) {
			TopologyObject object = topology->lookupObjectById( curTopology );
			TopologyType type = topology->lookupTypeById( object.type() );
			if( type.id() == blockDeviceType ) {
				ioStatistics.push_back( curStatistic );
				cerr << "detected input statistic: ontologyId = " << curAttribute << ", topologyId = " << curTopology << "\n";
				lastIoBlockCount += curStatistic->curValue.uint64();
			}
		} else if( userTimeAttribute == curAttribute || niceTimeAttribute == curAttribute || systemTimeAttribute == curAttribute || iowaitTimeAttribute == curAttribute || interruptsTimeAttribute == curAttribute || softirqTimeAttribute == curAttribute || vmsTimeAttribute == curAttribute || vmsOsTimeAttribute == curAttribute ) {
			if( curTopology == allCpusObject ) {
				busyCpuStatistics.push_back( curStatistic );
				cerr << "detected busy time statistic: ontologyId = " << curAttribute << ", topologyId = " << curTopology << "\n";
				lastBusyCpuTime += curStatistic->curValue.uint64();
			}
		} else if( idleTimeAttribute == curAttribute ) {
			if( curTopology == allCpusObject ) {
				idleCpuStatistic = curStatistic;
				cerr << "detected idle time statistic: ontologyId = " << curAttribute << ", topologyId = " << curTopology << "\n";
				lastIdleCpuTime = curStatistic->curValue.uint64();
			}
		}
	}
}

void QualitativeUtilization::newDataAvailable() throw() {
	uint64_t curIoBlockCount = 0;
	for( size_t i = ioStatistics.size(); i--; ) curIoBlockCount += ioStatistics[i]->curValue.uint64();
	uint64_t curBusyCpuTime = 0;
	for( size_t i = busyCpuStatistics.size(); i--; ) curBusyCpuTime += busyCpuStatistics[i]->curValue.uint64();
	uint64_t curIdleCpuTime = ( idleCpuStatistic ) ? idleCpuStatistic->curValue.uint64() : 0;

	cerr << "detected " << curIoBlockCount - lastIoBlockCount << " io blocks from " << ioStatistics.size() << " statistics, curIoBlockCount == " << curIoBlockCount << ", idle time = " << curIdleCpuTime - lastIdleCpuTime << ", busy time = " << curBusyCpuTime - lastBusyCpuTime << "\n";

	ioUtilization = options->ioBlockSize * ( curIoBlockCount - lastIoBlockCount ) / options->availableIoBandwidth;
	cpuUtilization = ( curBusyCpuTime - lastBusyCpuTime ) / ( double )( curBusyCpuTime - lastBusyCpuTime + curIdleCpuTime - lastIdleCpuTime );

	lastIoBlockCount = curIoBlockCount;
	lastBusyCpuTime = curBusyCpuTime;
	lastIdleCpuTime = curIdleCpuTime;
}

void QualitativeUtilization::fetchAttributeIds() throw() {
	if( AttributesComplete ) return;
	ActivityPluginDereferencing* facade = GET_INSTANCE( ActivityPluginDereferencing, options->dereferencingFacade );
	Topology* topology = facade->topology();
	#define fetchOntologyAttribute( idVariable, attributeName ) do { \
		if( !idVariable ) { \
			IGNORE_EXCEPTIONS( idVariable = facade->lookup_attribute_by_name( kStatisticsDomain, attributeName ).aID; ); \
		} \
	} while(0)
	fetchOntologyAttribute( readDataAttribute, "quantity/block/dataRead" );
	fetchOntologyAttribute( writtenDataAttribute, "quantity/block/dataWritten" );
	fetchOntologyAttribute( userTimeAttribute, "time/user" );
	fetchOntologyAttribute( niceTimeAttribute, "time/nice" );
	fetchOntologyAttribute( systemTimeAttribute, "time/system" );
	fetchOntologyAttribute( idleTimeAttribute, "time/idle" );
	fetchOntologyAttribute( iowaitTimeAttribute, "time/iowait" );
	fetchOntologyAttribute( interruptsTimeAttribute, "time/interrupts" );
	fetchOntologyAttribute( softirqTimeAttribute, "time/softirq" );
	fetchOntologyAttribute( vmsTimeAttribute, "time/vms" );
	fetchOntologyAttribute( vmsOsTimeAttribute, "time/vmsOS" );

	if( !blockDeviceType ) blockDeviceType = topology->registerType( "block-device" ).id();
	if( !allCpusObject ) allCpusObject = topology->registerObjectByPath( "@localhost/cpu:all" ).id();

	AttributesComplete = readDataAttribute && writtenDataAttribute && userTimeAttribute && niceTimeAttribute && systemTimeAttribute && idleTimeAttribute && iowaitTimeAttribute && interruptsTimeAttribute && softirqTimeAttribute && vmsTimeAttribute && vmsOsTimeAttribute;
}

extern "C" {
	void * MONITORING_STATISTICS_PLUGIN_INSTANCIATOR_NAME()
	{
		return new QualitativeUtilization();
	}
}
