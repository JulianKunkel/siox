#include <monitoring/statistics/multiplexer/StatisticsMultiplexer.hpp>
#include <monitoring/statistics/multiplexer/StatisticsMultiplexerPluginImplementation.hpp>
#include <knowledge/activity_plugin/ActivityPluginDereferencing.hpp>
#include <monitoring/ontology/OntologyDatatypes.hpp>
#include <monitoring/datatypes/Topology.hpp>
#include <knowledge/reasoner/AnomalyPlugin.hpp>

// #include <monitoring/statistics/StatisticsCollection.hpp>
// #include <monitoring/statistics/collector/StatisticsCollector.hpp>

#include <monitoring/statistics/multiplexer/plugins/statisticsHealthADPI/StatisticsHealthADPIOptions.hpp>

#include <string>

#include <assert.h>


using namespace std;
using namespace monitoring;
using namespace knowledge;
using namespace core;

#define DEBUG

#ifdef DEBUG
#define OUTPUT(...) do { cout << "[Statistics Health ADPI] " << __VA_ARGS__ << "\n"; } while(0)
#else
#define OUTPUT(...)
#endif

#define ERROR(...) do { cerr << "[Statistics Health ADPI] " << __VA_ARGS__ << "!\n"; } while(0)


class StatisticsHealthADPI : public StatisticsMultiplexerPlugin, public AnomalyPlugin {

	public:

		virtual void initPlugin() throw();

		virtual ComponentOptions * AvailableOptions();

		virtual void notifyAvailableStatisticsChange( const vector<shared_ptr<Statistic> > & statistics, bool addedStatistics, bool removedStatistics ) throw();
		virtual void newDataAvailable() throw();

		~StatisticsHealthADPI();


	private:

		/*
		 * Fields to hold information on the statistics we are to watch, as per our options
		 */
		// Preliminary Component ID
		ComponentID cid = {{0,0,0},0};
		// How close (relative to total value spread observed up to now) to highest and lowest value
		// to be regarded anomalous?
		const double kWarnQuantile = 0.01;
		// How many values are to be observed before evaluating a statistic's values for anomalies?
		const uint64_t kMinObservationCount = 10;
		// Domain string used for all statistics in the ontology
		const string kStatisticsDomain = "statistics";
		// Stings to use in anomaly reporting
		const string kIssueBadValue = "Bad value for ";
		const string kIssueGoodValue = "Good value for ";
		const string kIssueHighConsumption = "High power consumption";
		const string kIssueLowConsumption = "Low power consumption";
		// A flag indicating whether all statistics requested are being obtained right now.
		// Used for easier checking when the set of available ones changes.
		bool gotAllRequested = false;
		// The names of the statistics to watch
		vector<string> 	statisticsNames;
		// Ontology and topology IDs of the statistics to watch
		vector<OntologyAttributeID> requestedOntologyIDs;
		vector<TopologyObjectId> requestedTopologyIDs;
		// Pointers to the actual statistics objects currently being watched;
		// each object comes with a sliding history of values at different granularities
		vector<shared_ptr<Statistic>> statistics;

		/*
		 * Fields to hold the statistics values delivered to us
		 */
		// Minimum, maximum and upper and lower quantiles at WARN_PERCENT point
		vector<double> statisticsValuesMin;
		vector<double> statisticsValuesMax;
		vector<double> statisticsValuesQuantileLow;
		vector<double> statisticsValuesQuantileHigh;
		// Number of observations for statistic up to now
		vector<uint64_t> statisticsObservationCount;

		// A field holding the indices of the statistics that are available at the moment.
		// Used to iterate only over those available when updating values.
		vector<uint> availableStatisticsIndices;

		// Total energy consumed
		double totalEnergyConsumed = 0.0;	// TODO: Make this available via the proper ways!

		/*
		 * Fields to provide the estimated-deviation-from-expected-energy-consumption-anomaly feature
		 */
		// energy consumed
		const string kNameEnergyConsumedOnt = "power/rapl"; // TODO: Do this properly!
		const string kNameEnergyConsumedTop = "@localhost";
		const string kNameEnergyConsumed = kNameEnergyConsumedOnt + kNameEnergyConsumedTop;
		uint64_t nEnergyConsumedValues = 0;
		// OntologyAttributeID idEnergyConsumedOnt;
		// TopologyObjectId idEnergyConsumedTop;
		// shared_ptr<Statistic> statEnergyConsumed;
		double energyConsumed;
		// cpu utilization
		const string kNameCpuUtilizationOnt = "utilization/cpu"; // TODO: Do this properly!
		const string kNameCpuUtilizationTop = "@localhost";
		const string kNameCpuUtilization = kNameCpuUtilizationOnt + kNameCpuUtilizationTop;
		uint64_t nCpuUtilizationValues = 0;
		// OntologyAttributeID idCpuUtilizationOnt;
		// TopologyObjectId idCpuUtilizationTop;
		// shared_ptr<Statistic> statCpuUtilization;
		double cpuUtilization;
		// Values for parameters of estimated Gaussian
		// uint64_t efficiencyCount = 0;
		double efficiencyMean = 0.0;
		double efficiencyM2 = 0.0;
		double efficiencyVar;
		// Stings to use in anomaly reporting
		const string kIssueHighEfficiency = "High power efficiency";
		const string kIssueLowEfficiency = "Low power efficiency";
};


StatisticsHealthADPI::~StatisticsHealthADPI(){
}


void StatisticsHealthADPI::initPlugin() throw() {
	// Retrieve options and any other module links necessary
	StatisticsHealthADPIOptions &  options = getOptions<StatisticsHealthADPIOptions>();
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


	// OUTPUT( "Got " << options.requestedStatistics.size() << " statistics requests." );
	for ( auto request : options.requestedStatistics )
	{
		/*
		 * Look up and remember information for all requested statistics
		 */
		// OUTPUT( "Got a statistics request: [" << request.first << "," << request.second << "]" );
		// Find and remember ontology id
		OntologyAttribute ontologyAttribute = facade->lookup_attribute_by_name( kStatisticsDomain, request.first );
		// OUTPUT( "OntologyID: " << ontologyAttribute.aID );
		requestedOntologyIDs.push_back( ontologyAttribute.aID );
		// Find and remember topology id
		TopologyObject topologyObject = topology->lookupObjectByPath( request.second );
		// OUTPUT( "TopologyID: " << topologyObject.id() );
		requestedTopologyIDs.push_back( topologyObject.id() );
		// Remember a handy name for statistic; used for readable output
		statisticsNames.push_back( request.first + request.second );

		// Prepare a shared_ptr to reference the actual statistic later on
		statistics.push_back( shared_ptr<Statistic>( nullptr ) );
		// Prepare variables to hold the statistic's characteristic values
		statisticsValuesMin.push_back( 0.0 );
		statisticsValuesMax.push_back( 0.0 );
		statisticsValuesQuantileLow.push_back( 0.0 );
		statisticsValuesQuantileHigh.push_back( 0.0 );
		statisticsObservationCount.push_back( 0 );
	}
}


ComponentOptions* StatisticsHealthADPI::AvailableOptions() {
	return new StatisticsHealthADPIOptions();
}


/*
 * The set of statistics available to us has changed.
 */
void StatisticsHealthADPI::notifyAvailableStatisticsChange( const vector<shared_ptr<Statistic> > & offeredStatistics, bool addedStatistics, bool removedStatistics ) throw(){

	// OUTPUT( "Fresh statistics catalogue with " << offeredStatistics.size() << " entries." );
	if( gotAllRequested && !removedStatistics ) // We're happy - no need for action.
		return;

	// Remove all statistics, and start anew with whatever we're offered,
	// picking only those statistics we are to watch as per our options.
	availableStatisticsIndices.clear();
	for( auto s: offeredStatistics )
	{
		// OUTPUT( "Checking statistic [ontID=" << s->ontologyId << ",topID=" << s->topologyId << "]." );
		for( uint i = 0; i < requestedOntologyIDs.size(); i++ )
		{
			if( s->ontologyId == requestedOntologyIDs[i]
			   && s->topologyId == requestedTopologyIDs[i] )
			{
				// OUTPUT( "Found a match for index " << i << ": " << (uintmax_t)&*s << " [" << statisticsNames[i] << ",ontID=" << s->ontologyId << ",topID=" << s->topologyId << ", of type " << s->curValue.getTypeAsString() << "]!" );
				// Assign the statistic object to the proper index
				statistics[i] = s;
				// Remember this statistic's index amongst those being available.
				// Later, we will iterate over the statistics indexed here when updating.
				availableStatisticsIndices.push_back( i );
				break;
			}
		}
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

	gotAllRequested = ( requestedOntologyIDs.size() == availableStatisticsIndices.size() );
}


void StatisticsHealthADPI::newDataAvailable() throw(){
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
	// OUTPUT( "Statistics being watched: " << availableStatisticsIndices.size() );
	// Copy values received into local store
	for(uint i=0; i < availableStatisticsIndices.size() ; i++)
	{
		uint j = availableStatisticsIndices[i];
		double value = statistics[j]->curValue.toDouble();
		// OUTPUT( "Current node statistic[" << j << ": " << statisticsNames[j] << "]: " << value );
		// FIXME:
		// Log file varies dramatically in length.

		uint64_t count = ++statisticsObservationCount[j];
		bool updateQuantiles = false;

		if( count > kMinObservationCount )
		{
			// Necessary number of training values has been observed:
			// Test value for problems
			if( value < statisticsValuesQuantileLow[j] )
			{
				// Flag any problems found to reasoner
				OUTPUT( "Flagging Anomaly for " << statisticsNames[j] << ": " << value << "<" << statisticsValuesQuantileLow[j] << " => " << HealthState::ABNORMAL_GOOD );
				addObservation( cid, HealthState::ABNORMAL_GOOD,  kIssueGoodValue + statisticsNames[j], 0 );
			}
			if( value > statisticsValuesQuantileHigh[j] )
			{
				// Flag any problems found to reasoner
				OUTPUT( "Flagging Anomaly for " << statisticsNames[j] << ": " << value << ">" << statisticsValuesQuantileHigh[j] << " => " << HealthState::ABNORMAL_BAD );
				addObservation( cid, HealthState::ABNORMAL_BAD,  kIssueBadValue + statisticsNames[j], 0 );
			}

			// Update Min?
			if( value < statisticsValuesMin[j] ){
				statisticsValuesMin[j] = value;
				updateQuantiles = true;
				// OUTPUT( "New minimum: " << value );
			}
			// Update Max?
			if( value > statisticsValuesMax[j] )
			{
				statisticsValuesMax[j] = value;
				updateQuantiles = true;
				// OUTPUT( "New maximum: " << value );
			}
		}
		else
		{
			// Very first values?
			if( count == 1 )
			{
				// Set preliminary Min and Max values
				statisticsValuesMax[j] = value;
				statisticsValuesMin[j] = value;
				// OUTPUT( "First value: " << value );
			}

			// Update Min?
			if( value < statisticsValuesMin[j] )
			{
				statisticsValuesMin[j] = value;
			}
			// Update Max?
			if( value > statisticsValuesMax[j] )
			{
				statisticsValuesMax[j] = value;
			}

			// Last value before actual assessment starts?
			if( count == kMinObservationCount )
			{
				// Set quantile values
				updateQuantiles = true;
			}
		}

		// Update quantiles?
		if( updateQuantiles )
		{
			double quantileDelta = (statisticsValuesMax[j] - statisticsValuesMin[j]) * kWarnQuantile;
			statisticsValuesQuantileLow[j] = statisticsValuesMin[j] + quantileDelta;
			statisticsValuesQuantileHigh[j] = statisticsValuesMax[j] - quantileDelta;
			// OUTPUT( "New limits set for statistic: ["
			       // << statisticsValuesMin[j] << ","
			       // << statisticsValuesQuantileLow[j] << ","
			       // << statisticsValuesQuantileHigh[j] << ","
			       // << statisticsValuesMax[j] << "]" );
		}

		// Finally, if we are treating the energy consumed, remember the value and keep a running total
		if ( statisticsNames[j] == kNameEnergyConsumed )
		{
			energyConsumed = value;
			nEnergyConsumedValues ++;
			totalEnergyConsumed += value; // Quick hack to supply a meter for energy consumed here.
			// OUTPUT( "EnergyConsumed value # " << nEnergyConsumedValues << ": " << energyConsumed );
			// OUTPUT( "Total energy consumed up to now: " << totalEnergyConsumed );
		}
		// If we are treating the cpu utilization, remember the value
		if ( statisticsNames[j] == kNameCpuUtilization )
		{
			cpuUtilization = value;
			nCpuUtilizationValues ++;
			// OUTPUT( "CpuUtilization value # "<< nCpuUtilizationValues << ": " << cpuUtilization );
		}

	}

	// Once we have all values, compute energy efficiency and react to it
	if ( nCpuUtilizationValues == nEnergyConsumedValues ) // Do we have the same number of values for both?
	{	// Compute and test efficiency metric
		double efficiency = cpuUtilization / energyConsumed;
		// OUTPUT( "efficiency = " << cpuUtilization << " / " << energyConsumed << " = " << efficiency );

		// If past the settling phase, test for anomalies
		if ( nCpuUtilizationValues > kMinObservationCount )
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
	else
	{	// No efficiency metric for this cycle;
		// effectively the ignore lone value for this cycle by discounting it
		if( nCpuUtilizationValues > nEnergyConsumedValues )
			nCpuUtilizationValues --;
		else
			nEnergyConsumedValues --;
	}
}


#undef OUTPUT
#undef ERROR


extern "C" {
	void* MONITORING_STATISTICS_MULTIPLEXER_PLUGIN_INSTANCIATOR_NAME() {
		return new StatisticsHealthADPI();
	}
}
