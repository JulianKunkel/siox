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
		OntologyAttributeID cpuTimeAttribute = 0;
		OntologyAttributeID cpuIdleTimeAttribute = 0;
		OntologyAttributeID totalMemoryAttribute = 0;
		OntologyAttributeID activeMemoryAttribute = 0;
		OntologyAttributeID inactiveMemoryAttribute = 0;
		OntologyAttributeID bandwidthBytesAttribute = 0;
		OntologyAttributeID bytesSentAttribute = 0;
		OntologyAttributeID bytesRecievedAttribute = 0;
		OntologyAttributeID likwidMemoryBandwidth = 0;

		TopologyTypeId ethType = 0;
		TopologyObjectId localhostObject = 0, allCpusObject = 0;		

		///@todo TODO: interprete the overflow_max_value and overflow_next_value fields of StatisticsProviderDatatypes
		std::shared_ptr<Statistic> ioStatisticsRead;
		std::shared_ptr<Statistic> ioStatisticsWrite;
		uint64_t lastIoVolume = 0;
		const StatisticsValue * likwidMemoryBandwithStatistic;

		std::shared_ptr<Statistic> cpuStatistic;
		std::shared_ptr<Statistic> cpuIdleStatistic;

		std::shared_ptr<Statistic> totalMemoryStatistic, activeMemoryStatistic, inactiveMemoryStatistic;
		std::vector<std::shared_ptr<Statistic> > bandwidthStatistics;
		std::vector<std::shared_ptr<Statistic> > bytesSentStatistics;
		uint64_t lastBytesSent;
		std::vector<std::shared_ptr<Statistic> > bytesRecievedStatistics;
		uint64_t lastBytesRecieved;

		uint64_t lastCPUtimeconsumed = 0;
		uint64_t lastCPUIdletime = 0;

		StatisticsValue cpuUtilization = 0.0f;
		StatisticsValue networkUtilizationTX = 0.0f;
		StatisticsValue networkUtilizationRX = 0.0f;
		StatisticsValue memoryUtilizationVM = 0.0f;
		StatisticsValue memoryUtilization = 0.0f;
		StatisticsValue ioUtilization = 0.0f;
		StatisticsValue ioVolume = (uint64_t) 0.0;
		StatisticsValue networkVolume = (uint64_t) 0.0;
};


ComponentOptions* QualitativeUtilization::AvailableOptions() {
	return new QualitativeUtilizationOptions();
}


//XXX: Currently, we aggregate the data in `newDataAvailable()`, because that guarantees that the inputs are all from the current iteration. However, it makes our output values change _while_ the listeners are informed of the new data. That means, that some listeners may still see the old values while others see the new ones. Integrating data in `nextTimestep()` would cause us to use inconsistent data to begin with. I cannot say whether it is a problem that some listeners get older data than others. I see only one fix to the inconsistency problem: calculate and cache the data in `newDataAvailable()`, and make it available when `nextTimestep()` is called. However, this would have the drawback that all our listeners would get data that is at least 0.1 seconds old.
void QualitativeUtilization::nextTimestep() throw() {
}


vector<StatisticsProviderDatatypes> QualitativeUtilization::availableMetrics() throw() {
	vector<StatisticsProviderDatatypes> r;

	r.push_back( {"utilization/cpu", "@localhost", cpuUtilization, GAUGE, "", "average cpu utilization on a node"} );
	r.push_back( {"utilization/memory/vm", "@localhost", memoryUtilizationVM, GAUGE, "", "memory utilization on a node computed using the virtual machine's stats"} );
	r.push_back( {"utilization/memory", "@localhost", memoryUtilization, GAUGE, "", "memory utilization on a node"} );	
	r.push_back( {"utilization/network/send", "@localhost", networkUtilizationTX, GAUGE, "", "average network utilization across all links of a node"} );
	r.push_back( {"utilization/network/receive", "@localhost", networkUtilizationRX, GAUGE, "", "average network utilization across all links of a node"} );	
	r.push_back( {"utilization/io", "@localhost", ioUtilization, GAUGE, "", "percentage of available I/O bandwidth used"} );

	r.push_back( {"quantity/network/volume", "@localhost", networkVolume, GAUGE, "", "Amount of data transferred over the network"} );
	r.push_back( {"quantity/io/volume", "@localhost", ioVolume, GAUGE, "", "Amount of data accessed on an I/O system"} );	

	return r;
}


void QualitativeUtilization::initPlugin() throw() {
	options = &getOptions<QualitativeUtilizationOptions>();
}


void QualitativeUtilization::notifyAvailableStatisticsChange( const std::vector<std::shared_ptr<Statistic> > & statistics, bool addedStatistics, bool removedStatistics ) throw() {
	if( !readDataAttribute ) fetchAttributeIds();
	Topology* topology = GET_INSTANCE( ActivityPluginDereferencing, options->dereferencingFacade )->topology();

	ioStatisticsRead = nullptr;
	ioStatisticsWrite = nullptr;
	lastIoVolume = 0;
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
		if( readDataAttribute == curAttribute && curTopology == allCpusObject ) {
			ioStatisticsRead = curStatistic ;
			lastIoVolume += curStatistic->curValue.uint64();
		} else if( writtenDataAttribute == curAttribute && curTopology == allCpusObject ) {
			ioStatisticsWrite = curStatistic ;
			lastIoVolume += curStatistic->curValue.uint64();
		} else if( cpuIdleTimeAttribute == curAttribute && curTopology == allCpusObject ) {
			lastCPUIdletime = curStatistic->curValue.uint64();
			cpuIdleStatistic = curStatistic;
		} else if( cpuTimeAttribute == curAttribute && curTopology == allCpusObject ) {
			cpuStatistic = curStatistic;
			lastCPUtimeconsumed = curStatistic->curValue.uint64();
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
	uint64_t curIOvolume = 0, curIdleCpuTime = 0, curBandwidth = 0, curBytesSent = 0, curBytesRecieved = 0;
	curIOvolume = ioStatisticsRead->curValue.uint64() + ioStatisticsWrite->curValue.uint64();

	for( size_t i = bandwidthStatistics.size(); i--; ) curBandwidth += bandwidthStatistics[i]->curValue.uint64();
	for( size_t i = bytesSentStatistics.size(); i--; ) curBytesSent += bytesSentStatistics[i]->curValue.uint64();
	for( size_t i = bytesRecievedStatistics.size(); i--; ) curBytesRecieved += bytesRecievedStatistics[i]->curValue.uint64();

	// TODO use blockdev --getbsz /dev/sda1 to determine block size
	ioUtilization = (float) (uint64_t)( curIOvolume - lastIoVolume ) / options->availableIoBandwidth;

	curIdleCpuTime = cpuIdleStatistic->curValue.uint64();
	uint64_t curCPUConsumed = cpuStatistic->curValue.uint64();

	float deltaCPU = ( curCPUConsumed - lastCPUtimeconsumed + curIdleCpuTime - lastCPUIdletime);	
	cpuUtilization = (deltaCPU == 0.0f) ? 0.0f : ( curCPUConsumed - lastCPUtimeconsumed ) / deltaCPU;
	lastCPUtimeconsumed = curCPUConsumed;
	lastCPUIdletime = curIdleCpuTime;

	memoryUtilizationVM = 0.0f;

	if( totalMemoryStatistic && activeMemoryStatistic && inactiveMemoryStatistic) {
		//There is unfortunately not a right way to calculate memory consumption. The approach used here tries to assess the danger of thrashing: If we have a high amount of inactive pages, there is no danger of thrashing. Likewise, if we have a low number of active one, danger is low, even if there are few inactive pages. To reflect that both measures are important, we just use the average between active and not inactive pages as the memory that is currently used. However, it might turn out that the plain average is not good enough, that a weighted average is required, or some other, more complex formula is needed. So, feel free to correct me.
		uint64_t total = totalMemoryStatistic->curValue.uint64();
		uint64_t lowUsage = activeMemoryStatistic->curValue.uint64();
		uint64_t highUsage = total - inactiveMemoryStatistic->curValue.uint64();
		memoryUtilizationVM = (float) ( lowUsage + highUsage ) / ( 2.0 * total );
	}
	networkUtilizationTX = (float) ( curBytesSent - lastBytesSent) /  options->availableNetworkBandwidth;
	networkUtilizationRX = (float) ( curBytesRecieved - lastBytesRecieved ) /  options->availableNetworkBandwidth;

	networkVolume = (uint64_t) ( curBytesSent - lastBytesSent) + ( curBytesRecieved - lastBytesRecieved ) ;
	ioVolume = ((uint64_t) 512) * ( curIOvolume - lastIoVolume );

	lastIoVolume = curIOvolume;
	lastBytesSent = curBytesSent;
	lastBytesRecieved = curBytesRecieved;

	if ( likwidMemoryBandwithStatistic != nullptr ){
		memoryUtilization =  (float) likwidMemoryBandwithStatistic->flt() / options->availableMemoryBandwidth;
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
	fetchOntologyAttribute( cpuTimeAttribute, "time/cpu" );
	fetchOntologyAttribute( cpuIdleTimeAttribute, "time/idle" );
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
