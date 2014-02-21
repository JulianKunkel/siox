#include <knowledge/activity_plugin/ActivityPluginDereferencing.hpp>
#include <monitoring/statistics/StatisticsIntegrator.hpp>

#include "QualitativeUtilizationOptions.hpp"

using namespace std;
using namespace monitoring;
using namespace core;


class QualitativeUtilization : public StatisticsIntegrator {
	public:
		ComponentOptions* AvailableOptions() override;
		virtual void nextTimestep() throw() override;
		virtual vector<StatisticsProviderDatatypes> availableMetrics() throw() override;
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
		OntologyAttributeID totalMemoryAttribute = 0;
		OntologyAttributeID activeMemoryAttribute = 0;
		OntologyAttributeID inactiveMemoryAttribute = 0;
		OntologyAttributeID bandwidthBytesAttribute = 0;
		OntologyAttributeID bytesSentAttribute = 0;
		OntologyAttributeID bytesRecievedAttribute = 0;
		OntologyAttributeID likwidMemoryBandwidth = 0;

		TopologyTypeId blockDeviceType = 0;
		TopologyTypeId ethType = 0;
		TopologyObjectId localhostObject = 0, allCpusObject = 0;

		///@todo TODO: interprete the overflow_max_value and overflow_next_value fields of StatisticsProviderDatatypes
		std::vector<std::shared_ptr<Statistic> > ioStatistics;
		uint64_t lastIoBlockCount = 0;
		std::vector<std::shared_ptr<Statistic> > busyCpuStatistics;
		uint64_t lastBusyCpuTime = 0;
		std::shared_ptr<Statistic> idleCpuStatistic;
		const StatisticsValue * likwidMemoryBandwithStatistic;

		uint64_t lastIdleCpuTime = 0;
		std::shared_ptr<Statistic> totalMemoryStatistic, activeMemoryStatistic, inactiveMemoryStatistic;
		std::vector<std::shared_ptr<Statistic> > bandwidthStatistics;
		std::vector<std::shared_ptr<Statistic> > bytesSentStatistics;
		uint64_t lastBytesSent;
		std::vector<std::shared_ptr<Statistic> > bytesRecievedStatistics;
		uint64_t lastBytesRecieved;

		StatisticsValue cpuUtilization = 0.0;
		StatisticsValue networkUtilization = 0.0;	//XXX: Should we split this into an upstream and a downstream utilization?
		StatisticsValue memoryUtilizationVM = 0.0;
		StatisticsValue memoryUtilization = 0.0;
		StatisticsValue ioUtilization = 0.0;
};


ComponentOptions* QualitativeUtilization::AvailableOptions() {
	return new QualitativeUtilizationOptions();
}


//XXX: Currently, we aggregate the data in `newDataAvailable()`, because that guarantees that the inputs are all from the current iteration. However, it makes our output values change _while_ the listeners are informed of the new data. That means, that some listeners may still see the old values while others see the new ones. Integrating data in `nextTimestep()` would cause us to use inconsistent data to begin with. I cannot say whether it is a problem that some listeners get older data than others. I see only one fix to the inconsistency problem: calculate and cache the data in `newDataAvailable()`, and make it available when `nextTimestep()` is called. However, this would have the drawback that all our listeners would get data that is at least 0.1 seconds old.
void QualitativeUtilization::nextTimestep() throw() {
}


vector<StatisticsProviderDatatypes> QualitativeUtilization::availableMetrics() throw() {
	vector<StatisticsProviderDatatypes> r;

	r.push_back( {CPU, NODE, "utilization/cpu", "@localhost", cpuUtilization, GAUGE, "", "average cpu utilization on a node", 0, 0} );
	r.push_back( {MEMORY, NODE, "utilization/memory/vm", "@localhost", memoryUtilizationVM, GAUGE, "", "memory utilization on a node computed using the virtual machine's stats", 0, 0} );
	r.push_back( {MEMORY, NODE, "utilization/memory", "@localhost", memoryUtilization, GAUGE, "", "memory utilization on a node", 0, 0} );	
	r.push_back( {NETWORK, NODE, "utilization/network", "@localhost", networkUtilization, GAUGE, "", "average network utilization across all links of a node", 0, 0} );
	r.push_back( {INPUT_OUTPUT, NODE, "utilization/io", "@localhost", ioUtilization, GAUGE, "", "percentage of available I/O bandwidth used", 0, 0} );

	return r;
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
	bandwidthStatistics.clear();
	bytesSentStatistics.clear();
	lastBytesSent = 0;
	bytesRecievedStatistics.clear();
	lastBytesRecieved = 0;

	for( size_t i = statistics.size(); i--; ) {
		const std::shared_ptr<Statistic>& curStatistic = statistics[i];
		OntologyAttributeID curAttribute = curStatistic->ontologyId;
		TopologyObjectId curTopology = curStatistic->topologyId;
		TopologyTypeId curTopologyType = topology->lookupTypeById( topology->lookupObjectById( curTopology ).type() ).id();
		if( !curAttribute ) continue;
		if( readDataAttribute == curAttribute || writtenDataAttribute == curAttribute ) {
			if( curTopologyType == blockDeviceType ) {
				ioStatistics.push_back( curStatistic );
				lastIoBlockCount += curStatistic->curValue.uint64();
			}
		} else if( userTimeAttribute == curAttribute || niceTimeAttribute == curAttribute || systemTimeAttribute == curAttribute || iowaitTimeAttribute == curAttribute || interruptsTimeAttribute == curAttribute || softirqTimeAttribute == curAttribute || vmsTimeAttribute == curAttribute || vmsOsTimeAttribute == curAttribute ) {
			if( curTopology == allCpusObject ) {
				busyCpuStatistics.push_back( curStatistic );
				lastBusyCpuTime += curStatistic->curValue.uint64();
			}
		} else if( idleTimeAttribute == curAttribute ) {
			if( curTopology == allCpusObject ) {
				idleCpuStatistic = curStatistic;
				lastIdleCpuTime = curStatistic->curValue.uint64();
			}
		} else if( totalMemoryAttribute == curAttribute && curTopology == localhostObject ) {
			totalMemoryStatistic = curStatistic;
		} else if( activeMemoryAttribute == curAttribute && curTopology == localhostObject ) {
			activeMemoryStatistic = curStatistic;
		} else if( inactiveMemoryAttribute == curAttribute && curTopology == localhostObject ) {
			inactiveMemoryStatistic = curStatistic;
		} else if( bandwidthBytesAttribute == curAttribute && curTopologyType == ethType ) {
			bandwidthStatistics.push_back( curStatistic );
		} else if( bytesSentAttribute == curAttribute && curTopologyType == ethType ) {
			bytesSentStatistics.push_back( curStatistic );
			lastBytesSent += curStatistic->curValue.uint64();
		} else if( bytesRecievedAttribute == curAttribute && curTopologyType == ethType ) {
			bytesRecievedStatistics.push_back( curStatistic );
			lastBytesRecieved += curStatistic->curValue.uint64();
		} else if( likwidMemoryBandwidth == curAttribute && curTopology == localhostObject ) {
			if ( removedStatistics )
				likwidMemoryBandwithStatistic = nullptr;
			else
				likwidMemoryBandwithStatistic = & curStatistic->curValue;
		}

	}
}


void QualitativeUtilization::newDataAvailable() throw() {
	uint64_t curIoBlockCount = 0, curBusyCpuTime = 0, curIdleCpuTime = 0, curBandwidth = 0, curBytesSent = 0, curBytesRecieved = 0;
	for( size_t i = ioStatistics.size(); i--; ) curIoBlockCount += ioStatistics[i]->curValue.uint64();
	for( size_t i = busyCpuStatistics.size(); i--; ) curBusyCpuTime += busyCpuStatistics[i]->curValue.uint64();
	if( idleCpuStatistic ) curIdleCpuTime = idleCpuStatistic->curValue.uint64();
	for( size_t i = bandwidthStatistics.size(); i--; ) curBandwidth += bandwidthStatistics[i]->curValue.uint64();
	for( size_t i = bytesSentStatistics.size(); i--; ) curBytesSent += bytesSentStatistics[i]->curValue.uint64();
	for( size_t i = bytesRecievedStatistics.size(); i--; ) curBytesRecieved += bytesRecievedStatistics[i]->curValue.uint64();

	ioUtilization = options->ioBlockSize * ( curIoBlockCount - lastIoBlockCount ) / options->availableIoBandwidth;
	cpuUtilization = ( curBusyCpuTime - lastBusyCpuTime ) / ( double )( curBusyCpuTime - lastBusyCpuTime + curIdleCpuTime - lastIdleCpuTime );
	memoryUtilizationVM = 0.0;
	if( totalMemoryStatistic && activeMemoryStatistic && inactiveMemoryStatistic) {
		//There is unfortunately not a right way to calculate memory consumption. The approach used here tries to assess the danger of thrashing: If we have a high amount of inactive pages, there is no danger of thrashing. Likewise, if we have a low number of active one, danger is low, even if there are few inactive pages. To reflect that both measures are important, we just use the average between active and not inactive pages as the memory that is currently used. However, it might turn out that the plain average is not good enough, that a weighted average is required, or some other, more complex formula is needed. So, feel free to correct me.
		uint64_t total = totalMemoryStatistic->curValue.uint64();
		uint64_t lowUsage = activeMemoryStatistic->curValue.uint64();
		uint64_t highUsage = total - inactiveMemoryStatistic->curValue.uint64();
		memoryUtilizationVM = ( lowUsage + highUsage ) / ( 2.0 * total );
	}
	networkUtilization = ( curBytesSent - lastBytesSent + curBytesRecieved - lastBytesRecieved ) / ( 2.0 * curBandwidth / 10 );	//*2 for duplex, /10 for 100ms poll interval

	lastIoBlockCount = curIoBlockCount;
	lastBusyCpuTime = curBusyCpuTime;
	lastIdleCpuTime = curIdleCpuTime;
	lastBytesSent = curBytesSent;
	lastBytesRecieved = curBytesRecieved;

	if ( likwidMemoryBandwithStatistic != nullptr ){
		memoryUtilization = likwidMemoryBandwithStatistic->flt() / options->availableMemoryBandwidth;
	}
}


void QualitativeUtilization::fetchAttributeIds() throw() {
	if( AttributesComplete ) return;
	ActivityPluginDereferencing* facade = GET_INSTANCE( ActivityPluginDereferencing, options->dereferencingFacade );
	Topology* topology = facade->topology();
	AttributesComplete = true;

	#define fetchOntologyAttribute( idVariable, attributeName ) do { \
		if( !idVariable ) { \
			IGNORE_EXCEPTIONS( idVariable = facade->lookup_attribute_by_name( kStatisticsDomain, attributeName ).aID; ); \
			if( !idVariable ) AttributesComplete = false; \
		} \
	} while( 0 )
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
	fetchOntologyAttribute( totalMemoryAttribute, "quantity/memory/MemTotal" );
	fetchOntologyAttribute( activeMemoryAttribute, "quantity/memory/Active" );
	fetchOntologyAttribute( inactiveMemoryAttribute, "quantity/memory/Inactive" );
	fetchOntologyAttribute( bandwidthBytesAttribute, "quantity/bandwidthBytes" );
	fetchOntologyAttribute( bytesSentAttribute, "quantity/bytesSent" );
	fetchOntologyAttribute( bytesRecievedAttribute, "quantity/bytesRecieved" );
	fetchOntologyAttribute( likwidMemoryBandwidth, "throughput/memory/bandwidth" );

	#define fetchTopologyId( idVariable, topologyFunction, argument ) do { \
		if( !idVariable ) { \
			if( auto temp = topology->topologyFunction( argument ) ) { \
				idVariable = temp.id(); \
			} else { \
				AttributesComplete = false; \
			} \
		} \
	} while( 0 )
	fetchTopologyId( blockDeviceType, registerType, "block-device" );
	fetchTopologyId( ethType, registerType, "eth" );
	fetchTopologyId( localhostObject, registerObjectByPath, "@localhost" );
	fetchTopologyId( allCpusObject, registerObjectByPath, "@localhost" );
}


extern "C" {
	void * MONITORING_STATISTICS_PLUGIN_INSTANCIATOR_NAME()
	{
		return new QualitativeUtilization();
	}
}
