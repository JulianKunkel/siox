#include <monitoring/statistics/multiplexer/StatisticsMultiplexer.hpp>
#include <monitoring/statistics/multiplexer/StatisticsMultiplexerPluginImplementation.hpp>
#include <knowledge/activity_plugin/ActivityPluginDereferencing.hpp>
#include <monitoring/ontology/OntologyDatatypes.hpp>
#include <monitoring/datatypes/Topology.hpp>
#include <knowledge/reasoner/AnomalyPlugin.hpp>

// #include <monitoring/statistics/StatisticsCollection.hpp>
// #include <monitoring/statistics/collector/StatisticsCollector.hpp>

#include <monitoring/statistics/multiplexer/plugins/statisticsHealthADPI/EnergyEfficiencyHealthADPIOptions.hpp>

#include <string>

#include <assert.h>


using namespace std;
using namespace monitoring;
using namespace knowledge;
using namespace core;

#define DEBUG

#ifdef DEBUG
#define OUTPUT(...) do { cout << "[Energy Efficiency Health ADPI] " << __VA_ARGS__ << "\n"; } while(0)
#else
#define OUTPUT(...)
#endif

#define ERROR(...) do { cerr << "[Energy Efficiency Health ADPI] " << __VA_ARGS__ << "!\n"; } while(0)


class EnergyEfficiencyHealthADPI : public StatisticsMultiplexerPlugin, public AnomalyPlugin {

	public:

		virtual void initPlugin() throw();

		virtual ComponentOptions * AvailableOptions();

		virtual void notifyAvailableStatisticsChange( const vector<shared_ptr<Statistic> > & statistics, bool addedStatistics, bool removedStatistics ) throw();
		virtual void newDataAvailable() throw();

		~EnergyEfficiencyHealthADPI();


	private:

		/*
		 * Fields to hold information on the statistics we are to watch, as per our options
		 */
		// Preliminary Component ID
		ComponentID cid = {{0,0,7},0};
		// Domain string used for all statistics in the ontology
		const string kStatisticsDomain = "statistics";
		// Stings to use in anomaly reporting
		const string kIssueHighConsumption = "High power consumption";
		const string kIssueLowConsumption = "Low power consumption";
		// How many values are to be observed before evaluating a statistic's values for anomalies?
		uint64_t nMinObservationCount = 100;
		// A flag indicating whether all statistics requested are being obtained right now.
		// Used for easier checking when the set of available ones changes.
		bool gotAllRequested = false;
		// Ontology and topology IDs of the statistics to watch
		// vector<OntologyAttributeID> requestedOntologyIDs;
		// vector<TopologyObjectId> requestedTopologyIDs;
		// Pointers to the actual statistics objects currently being watched;
		// each object comes with a sliding history of values at different granularities
		// vector<shared_ptr<Statistic>> statistics;

		/*
		 * Fields to hold the statistics values delivered to us
		 */

		// Total energy consumed
		double totalEnergyConsumed = 0.0;	// TODO: Make this available via a proper report!

		/*
		 * Fields to provide the estimated-deviation-from-expected-energy-consumption-anomaly feature
		 */
		// energy consumed
		const string kNameEnergyConsumedOnt = "power/rapl";
		const string kNameEnergyConsumedTop = "@localhost";
		const string kNameEnergyConsumed = kNameEnergyConsumedOnt + kNameEnergyConsumedTop;
		uint64_t nEnergyConsumedValues = 0;
		OntologyAttributeID 	idEnergyConsumedOnt;
		TopologyObjectId 		idEnergyConsumedTop;
		shared_ptr<Statistic> 	statisticEnergyConsumed;
		double energyConsumed;
		// cpu utilization
		const string kNameCpuUtilizationOnt = "utilization/cpu";
		const string kNameCpuUtilizationTop = "@localhost";
		const string kNameCpuUtilization = kNameCpuUtilizationOnt + kNameCpuUtilizationTop;
		uint64_t nCpuUtilizationValues = 0;
		OntologyAttributeID 	idCpuUtilizationOnt;
		TopologyObjectId 		idCpuUtilizationTop;
		shared_ptr<Statistic> 	statisticCpuUtilization;
		double cpuUtilization;
		// Values for parameters of estimated Gaussian
		uint64_t nEfficiencyCount = 0;
		double efficiencyMean = 0.0;
		double efficiencyM2 = 0.0;
		double efficiencyVar;
		// Stings to use in anomaly reporting
		const string kIssueHighEfficiency = "High power efficiency";
		const string kIssueLowEfficiency = "Low power efficiency";
};


EnergyEfficiencyHealthADPI::~EnergyEfficiencyHealthADPI(){
}


void EnergyEfficiencyHealthADPI::initPlugin() throw() {
	// Retrieve options and any other module links necessary
	EnergyEfficiencyHealthADPIOptions &  options = getOptions<EnergyEfficiencyHealthADPIOptions>();
	assert( &options != nullptr );
	// OUTPUT( "Got options!" );
	ActivityPluginDereferencing * facade = GET_INSTANCE( ActivityPluginDereferencing, options.dereferencingFacade );
	assert( facade != nullptr );
	// OUTPUT( "Got a dereferencingFacade!" );
	Topology * topology = facade->topology();
	assert( topology != nullptr );
	// OUTPUT( "Got a topology!" );
	// Connect us to our reasoner
	facade->register_anomaly_plugin( this );

	// Retrieve other options
	nMinObservationCount = options.nMinObservationCount;


	/*
	 * Look up and remember information for all requested statistics
	 */
	OntologyAttribute 	ontologyAttribute;
	TopologyObject 		topologyObject;

	// Energy consumed
	//
	// Find and remember ontology id
	ontologyAttribute = facade->lookup_attribute_by_name( kStatisticsDomain, kNameEnergyConsumedOnt );
	// OUTPUT( "OntologyID: " << ontologyAttribute.aID );
	idEnergyConsumedOnt = ontologyAttribute.aID;
	// Find and remember topology id
	topologyObject = topology->lookupObjectByPath( kNameEnergyConsumedTop );
	// OUTPUT( "TopologyID: " << topologyObject.id() );
	idEnergyConsumedTop = topologyObject.id();
	// Prepare a shared_ptr to reference the actual statistic later on
	statisticEnergyConsumed = shared_ptr<Statistic>( nullptr );

	// CPU utilization
	//
	// Find and remember ontology id
	ontologyAttribute = facade->lookup_attribute_by_name( kStatisticsDomain, kNameCpuUtilizationOnt );
	// OUTPUT( "OntologyID: " << ontologyAttribute.aID );
	idCpuUtilizationOnt = ontologyAttribute.aID;
	// Find and remember topology id
	topologyObject = topology->lookupObjectByPath( kNameCpuUtilizationTop );
	// OUTPUT( "TopologyID: " << topologyObject.id() );
	idCpuUtilizationTop = topologyObject.id();
	// Prepare a shared_ptr to reference the actual statistic later on
	statisticCpuUtilization = shared_ptr<Statistic>( nullptr );
}


ComponentOptions* EnergyEfficiencyHealthADPI::AvailableOptions() {
	return new EnergyEfficiencyHealthADPIOptions();
}


/*
 * The set of statistics available to us has changed.
 */
void EnergyEfficiencyHealthADPI::notifyAvailableStatisticsChange( const vector<shared_ptr<Statistic> > & offeredStatistics, bool addedStatistics, bool removedStatistics ) throw(){

	// OUTPUT( "Fresh statistics catalogue with " << offeredStatistics.size() << " entries." );
	if( gotAllRequested && !removedStatistics ) // We're happy - no need for action.
		return;

	// Remove all statistics, and start anew with whatever we're offered,
	// picking only those statistics we are to watch as per our options.
	statisticEnergyConsumed = shared_ptr<Statistic>( nullptr );
	statisticCpuUtilization = shared_ptr<Statistic>( nullptr );
	for( auto s: offeredStatistics )
	{
		// OUTPUT( "Checking statistic [ontID=" << s->ontologyId << ",topID=" << s->topologyId << "]." );
		if( s->ontologyId == idEnergyConsumedOnt && s->topologyId == idEnergyConsumedTop )
				statisticEnergyConsumed = s;
		else if( s->ontologyId == idCpuUtilizationOnt && s->topologyId == idCpuUtilizationTop )
				statisticCpuUtilization = s;
	}
	// FIXME:
	// Make the test work again, if possible!
	//
	// Subscribe to a set of statistics
	// const std::vector<std::pair<std::string, std::string>>& ontologyAttributeTopologyPathPairs = {{
	// 		{"utilization/cpu", "@localhost"},
	// 		{"utilization/memory", "@localhost"}, // Alternative: {"utilization/memory/vm", "@localhost"},
	// 		{"utilization/io", "@localhost"},
	// 		{"utilization/network/send", "@localhost"},
	// 		{"utilization/network/receive", "@localhost"},
	// 		// {"time/cpu", "@localhost"}, // CONSUMED_CPU_SECONDS
	// 		// {"utilization/cpu", "@localhost"}, // power/rapl
	// 		// {"quantity/memory/volume", "@localhost"}, // CONSUMED_MEMORY_BYTES
	// 		// {"quantity/network/volume", "@localhost"}, // CONSUMED_NETWORK_BYTES
	// 		// {"quantity/io/volume", "@localhost"}, // CONSUMED_IO_BYTES
	// 	}};
	//
	// statistics.clear();
	// for( auto s : offeredStatistics )
	// {
	// 	statistics.push_back( s );
	// 	statisticsValues.push_back( StatisticsValue(0.0) );
	// }

	// OUTPUT( "Statistics catalogue now holds " << statistics.size() << " entries." );

	gotAllRequested = ( statisticEnergyConsumed && statisticCpuUtilization );
}


void EnergyEfficiencyHealthADPI::newDataAvailable() throw(){
	// TODO:
	// Make sense of this?!
	//
	// uint64_t timestamp = (*statistics->begin())->curTimestamp();
	// *oa << timestamp ;
	// uint64_t size = statistics->size();
	// *oa << size;
	//
	// // write out all currently existing statistics
	// for(auto itr = statistics->begin(); itr != statistics->end(); itr++){
	// 	StatisticsDescription & sd = **itr;
	// 	*oa << sd;
	// 	*oa << (*itr)->curValue;
	// }

	// OUTPUT( "Received new data!" );

	energyConsumed = statisticEnergyConsumed->curValue.toDouble();
	nEnergyConsumedValues ++;
	totalEnergyConsumed += value; // Quick hack to supply a meter for energy consumed here.
	// OUTPUT( "EnergyConsumed value # " << nEnergyConsumedValues << ": " << energyConsumed );
	OUTPUT( "Total energy consumed up to now: " << totalEnergyConsumed );

	cpuUtilization = statisticCpuUtilization->curValue.toDouble();
	nCpuUtilizationValues ++;
	// OUTPUT( "CpuUtilization value # "<< nCpuUtilizationValues << ": " << cpuUtilization );


	// Once we have all values, compute energy efficiency and react to it
	if ( gotAllRequested ) // Do we have the same number of values for both?
	{	// Compute and test efficiency metric
		double efficiency = cpuUtilization / energyConsumed;
		nEfficiencyCount++;
		OUTPUT( "Efficiency # " << nEfficiencyCount << ": " << cpuUtilization << " / " << energyConsumed << " = " << efficiency );

		// If past the settling phase, test for anomalies
		if ( nEfficiencyCount > nMinObservationCount )
		{
			efficiencyVar = efficiencyM2 / (nCpuUtilizationValues - 1);
			// OUTPUT( "Estimated Law: N(" << efficiencyMean << "," << efficiencyVar << ")" );
			double stddev = sqrt(efficiencyVar);
			if ( efficiency < efficiencyMean - stddev )
			{
				OUTPUT( "Flagging Anomaly for efficiency: " << efficiency << "<" << efficiencyMean - stddev << " => " << toString(HealthState::ABNORMAL_BAD) );
				addObservation( cid, HealthState::ABNORMAL_BAD,  kIssueLowEfficiency, 0 );
			}
			if ( efficiency > efficiencyMean + stddev )
			{
				OUTPUT( "Flagging Anomaly for efficiency: " << efficiency << ">" << efficiencyMean + stddev << " => " << toString(HealthState::ABNORMAL_GOOD) );
				addObservation( cid, HealthState::ABNORMAL_GOOD,  kIssueHighEfficiency, 0 );
			}
		}

		// Estimate mean and variance, according to Knuth:
		double delta = efficiency - efficiencyMean;
		// OUTPUT( "delta = " << delta );
		efficiencyMean += delta / nCpuUtilizationValues;
		// OUTPUT( "efficiencyMean = " << efficiencyMean );
		efficiencyM2 += delta * (efficiency - efficiencyMean);
		// OUTPUT( "efficiencyM2 = " << efficiencyM2 );
	}
}


#undef OUTPUT
#undef ERROR


extern "C" {
	void* MONITORING_STATISTICS_MULTIPLEXER_PLUGIN_INSTANCIATOR_NAME() {
		return new EnergyEfficiencyHealthADPI();
	}
}
