/**
 * An ActivityMultiplexerPlugin to periodically inject random anomalies for testing.
 *
 * @author Michaela Zimmer
 * @date 2014-01-23
 */

#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <random>

#include <core/reporting/ComponentReportInterface.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginImplementation.hpp>
#include <monitoring/system_information/SystemInformationGlobalIDManager.hpp>
#include <knowledge/reasoner/ReasoningDatatypes.hpp>

#include <knowledge/reasoner/AnomalyPlugin.hpp>

#include <workarounds.hpp>

#include "AnomalyInjectorOptions.hpp"


using namespace std;
using namespace monitoring;
using namespace core;
using namespace knowledge;


#define OUTPUT(...) do { cout << "[AnomalyInjector] " << __VA_ARGS__ << "\n"; } while(0)



class AnomalyInjectorPlugin: public ActivityMultiplexerPlugin, public ComponentReportInterface, public AnomalyPlugin {
	public:
		AnomalyInjectorPlugin();

		void initPlugin() override;
		ComponentOptions * AvailableOptions() override;

		//No notify functions as this plugin can create its own problems.

		virtual ComponentReport prepareReport() override;

		~AnomalyInjectorPlugin();

		friend ostream & operator<<( ostream & os, AnomalyInjectorPlugin & aipi );


	private:

		// ComponentID to attribute anomalies to
		ComponentID cid;

		// Name for issue to inject
		string issueName;
		// Random generator for intervals and
		random_device rd;
		// Mean of Exponential distribution for interval between two anomaly injections in microseconds
		float intervalMean;
		// Mean and variance of Gaussian distribution for delta_time_ms
		float deltaTimeMean;
		float deltaTimeVariance;

		// Mutex and condition variable for the generator thread time loop and termination
		thread 				generatorThread;
		mutex 				generatorMutex;
		condition_variable 	generatorSleep;
		bool				terminate;

		// Plugin statistics
		uint64_t 	nAnomaliesGenerated;
		float		sumSleep; // Summed up sleep time
		uint64_t	numSleep; // Number of sleep intervals


		void generateAnomaly();
};



ComponentOptions * AnomalyInjectorPlugin::AvailableOptions() {
  return new AnomalyInjectorPluginOptions();
}


AnomalyInjectorPlugin::AnomalyInjectorPlugin(){

	cid = {{0,0,0}, 0};

	intervalMean = 10.0;
	deltaTimeMean = 0.0;
	deltaTimeVariance = 10.0;

	terminate = false;

	nAnomaliesGenerated = 0;
	sumSleep = 0.0;
	numSleep = 0;
}


void AnomalyInjectorPlugin::initPlugin()
{
	// fprintf(stderr, "AnomalyInjectorPlugin::initPlugin(), this = 0x%016jx\n", (intmax_t)this);

	// Retrieve options
	AnomalyInjectorPluginOptions & o = getOptions<AnomalyInjectorPluginOptions>();

	// Retrieve parameters for anomaly generation
	issueName = o.issueName;
	intervalMean = (o.intervalMean > 0 ? o.intervalMean : -o.intervalMean );
	if (intervalMean < 10.0)
		intervalMean = 10.0;
	deltaTimeMean = o.deltaTimeMean;
	deltaTimeVariance =  o.deltaTimeVariance;

	// TODO: Either set the proper value, or use a random number
	cid.id = 42;

	// Spawn generator thread
	generatorThread = thread( & AnomalyInjectorPlugin::generateAnomaly, this );
}


AnomalyInjectorPlugin::~AnomalyInjectorPlugin()
{
	// Safely notify generator thread of termination
	generatorMutex.lock();
	terminate = true;
	// cout << "Generator stop..." << endl;
	generatorSleep.notify_one();
	generatorMutex.unlock();
	generatorThread.join();
	// cout << "Generator stopped!" << endl;
}


/*
 * Sleep for a while, then generate an anomaly; repeat until closed down
 */
void AnomalyInjectorPlugin::generateAnomaly()
{
	unsigned int sleepTime;

	HealthState state = HealthState::OK;
	int32_t delta_time_ms = 50;

	// Create distribution for interval until next injection
	geometric_distribution<> dGeom( 1.0/intervalMean );
	// Create distribution for delta_time_ms of HealthIssue to be injected
	normal_distribution<> dNorm( deltaTimeMean, deltaTimeVariance );

	cout << "Generator started!" << endl;

	while( ! terminate )
	{
		// Generate random sleep time and next wakeup point
		sleepTime = dGeom(rd);
		auto wakeupTime = chrono::system_clock::now() + chrono::milliseconds( sleepTime );

		// Generate random delta and from that a fitting state
		delta_time_ms = dNorm(rd);
		if ( delta_time_ms > 50 )
			state = HealthState::ABNORMAL_SLOW;
		else if ( delta_time_ms > 10 )
			state = HealthState::SLOW;
		else if ( delta_time_ms < -50 )
			state = HealthState::ABNORMAL_FAST;
		else if ( delta_time_ms < -10 )
			state = HealthState::SLOW;
		else
			state = HealthState::OK;

		// Actually inject the issue generated
		addObservation( cid, state,  issueName, delta_time_ms );

		// Unless termination has been requested, go to sleep until the next injection is due
		unique_lock<mutex> generatorLock( generatorMutex );
		nAnomaliesGenerated++;
		sumSleep += sleepTime;
		if( terminate )
			break;
		generatorSleep.wait_until( generatorLock, wakeupTime );
	}
}


ComponentReport AnomalyInjectorPlugin::prepareReport()
{
	ComponentReport result;

	result.addEntry( "AnomalyInjectorPlugin", ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( " Anomalies generated: " + nAnomaliesGenerated ) ));

	return result;
}

inline ostream & operator<<( ostream & os, AnomalyInjectorPlugin & aipi )
{
	{ // Disallow generator's access to data fields
		lock_guard<mutex> generatorLock(aipi.generatorMutex);

		os << "[AnomalyInjector(" << aipi.intervalMean << ",(" << aipi.deltaTimeMean << "," << aipi.deltaTimeVariance << ")): ";
		os << aipi.nAnomaliesGenerated << "x \"" << aipi.issueName << "\" in ";
		os << aipi.sumSleep << " ms; average nap: " << (aipi.sumSleep / aipi.nAnomaliesGenerated) << " ms]";
	}
	return os;
}




extern "C" {
	void * MONITORING_ACTIVITY_MULTIPLEXER_PLUGIN_INSTANCIATOR_NAME()
	{
		return new AnomalyInjectorPlugin();
	}
}
