#include <assert.h>
#include <unistd.h> // For sleep()

#include <iostream>
#include <mutex>
#include <condition_variable>

#include <util/TestHelper.hpp>

#include <core/module/ModuleLoader.hpp>
#include <core/comm/CommunicationModule.hpp>

#include <knowledge/reasoner/Reasoner.hpp>
#include <knowledge/reasoner/AnomalyPlugin.hpp>
#include <monitoring/activity_multiplexer/plugins/anomaly-injector/AnomalyInjector.cpp>

#include <knowledge/reasoner/modules/ReasonerStandardImplementation.hpp>
#include <knowledge/reasoner/modules/ReasonerStandardImplementationOptions.hpp>
#include <knowledge/reasoner/modules/ReasoningDatatypesSerializable.hpp>

#include <knowledge/reasoner/modules/ReasonerCommunication.hpp>

using namespace std;

using namespace core;
using namespace knowledge;

/*
 Teststubs
 */
class TestAnomalyTrigger : public AnomalyTrigger {
	private:
		mutex                   clock;
		condition_variable      cond;
	public:
		int anomaliesTriggered = 0;

		void clearAnomalyStatus(){

		}

		void triggerResponseForAnomaly(bool anomalyStillOngoing) {
			clock.lock();
			anomaliesTriggered++;
			cond.notify_one();
			clock.unlock();
			// cout << "Triggered " << anomaliesTriggered << endl;
		}

		bool waitForAnomalyCount( int i ) {
			while( 1 ) {
				auto timeout = chrono::system_clock::now() + chrono::milliseconds( 300000 );

				unique_lock<mutex> lock( clock );
				if( anomaliesTriggered >= i ) {
					return true;
				}
				if( cond.wait_until( lock, timeout ) == cv_status::timeout ) {
					return false;
				}
			}
		}
};

class TestAnomalyPlugin : public AnomalyPlugin {
	public:
		void injectObservation( ComponentID cid, HealthState state,  const string & issue, int32_t delta_time_ms ) {
			addObservation(cid, state, issue, delta_time_ms);
		}
};


class MyReasoningDataReceivedCB : public ReasoningDataReceivedCB, public ProtectRaceConditions{
public:
	ReasonerMessageReceived received_data;
	ProcessHealth received_ph;
	SystemHealth received_sh;
	NodeHealth received_nh;

	shared_ptr<NodeHealth> nh;
	shared_ptr<SystemHealth> sh;
	shared_ptr<ProcessHealth> ph;

	void receivedReasonerProcessHealth(ReasonerMessageReceived & data, ProcessHealth & health){
		received_data = data;
		received_ph = health;
		// cout << "receivedReasonerProcessHealth: " << health.overallState << endl;
		sthHappens();
	}

	void receivedReasonerNodeHealth(ReasonerMessageReceived & data, NodeHealth & health){
		received_data = data;
		received_nh = health;
		// cout << "receivedReasonerNodeHealth: " << health.overallState << endl;
		sthHappens();
	}

	void receivedReasonerSystemHealth(ReasonerMessageReceived & data, SystemHealth & health){
		received_data = data;
		received_sh = health;
		// cout << "receivedReasonerSystemHealth: " << health.overallState << endl;
		sthHappens();
	}

	shared_ptr<NodeHealth> getNodeHealth(){
		return nh;
	}

	shared_ptr<SystemHealth> getSystemHealth(){
		return sh;
	}

	shared_ptr<ProcessHealth> getProcessHealth(){
		return ph;
	}

	MyReasoningDataReceivedCB() : received_data("Undefined"){
		// fill dummy data
		{
		NodeHealth * h = new NodeHealth();
		nh = shared_ptr<NodeHealth>(h);
		}
		{
		SystemHealth * h = new SystemHealth();
		sh = shared_ptr<SystemHealth>(h);
		}
		{
		ProcessHealth * h = new ProcessHealth();
		ph = shared_ptr<ProcessHealth>(h);
		}
	}
};


/*
 * Test full reasoner tree with ADPIs and AnomalyTriggers.
 */
void testReasoner(){

	cout << endl;
	cout << "Test: Reasoner" << endl;
	cout << "==============" << endl;

/*
	// TODO: Add utilization once it becomes available
	TestQualitativeUtilization qu;
	r->connectUtilization( & qu );
*/

	/*
	 * Obtain some instances from module loader
	 */

	// Communication Module
	CommunicationModule * comm = core::module_create_instance<CommunicationModule>( "", "siox-core-comm-gio", CORE_COMM_INTERFACE );
	assert(comm != nullptr);
	comm->init();

	// System Reasoner
	Reasoner * rS = core::module_create_instance<Reasoner>( "", "siox-knowledge-ReasonerStandardImplementation", KNOWLEDGE_REASONER_INTERFACE );
	assert( rS != nullptr );
	{
		ReasonerStandardImplementationOptions & r_options = rS->getOptions<ReasonerStandardImplementationOptions>();
		r_options.role = Role::SYSTEM;
		r_options.communicationOptions.comm.componentPointer = comm;
		r_options.communicationOptions.serviceAddress = "ipc://reasonerS";
		r_options.communicationOptions.reasonerID = "system";
		rS->init(); // This will start a separate Reasoner thread
	}
	TestAnomalyTrigger atS;
	rS->connectTrigger( & atS );

	// Node 1 Reasoner
	Reasoner * rN1 = core::module_create_instance<Reasoner>( "", "siox-knowledge-ReasonerStandardImplementation", KNOWLEDGE_REASONER_INTERFACE );
	assert( rN1 != nullptr );
	{
		ReasonerStandardImplementationOptions & r_options = rN1->getOptions<ReasonerStandardImplementationOptions>();
		r_options.role = Role::NODE;
		r_options.communicationOptions.comm.componentPointer = comm;
		r_options.communicationOptions.serviceAddress = "ipc://reasonerN1";
		r_options.communicationOptions.upstreamReasoner = "ipc://reasonerS";
		r_options.communicationOptions.reasonerID = "node1";
		rN1->init(); // This will start a separate Reasoner thread
	}

	TestAnomalyTrigger atN1a;
	rN1->connectTrigger( & atN1a );
	TestAnomalyTrigger atN1b;
	rN1->connectTrigger( & atN1b );
	TestAnomalyPlugin adpiN1a;
	rN1->connectAnomalyPlugin( & adpiN1a );
	// AnomalyInjectorPlugin * adpiN1b = core::module_create_instance<AnomalyInjectorPlugin>( "", "siox-monitoring-activityPlugin-AnomalyInjector", ACTIVITY_MULTIPLEXER_PLUGIN_INTERFACE );
	AnomalyInjectorPlugin adpiN1b;
	{
		AnomalyInjectorPluginOptions & aipOptions = adpiN1b.getOptions<AnomalyInjectorPluginOptions>();
		aipOptions.issueName = "Gaussian Issue";
		aipOptions.intervalMean = 75.0;
		aipOptions.intervalType = "geometric";
		aipOptions.deltaTimeMean = 15.0;
		aipOptions.deltaTimeVariance = 15.0;
		rN1->connectAnomalyPlugin( & adpiN1b );
		adpiN1b.initPlugin();
	}

	// Process 11 Reasoner
	// One ADPI, one AT
	Reasoner * rP11 = core::module_create_instance<Reasoner>( "", "siox-knowledge-ReasonerStandardImplementation", KNOWLEDGE_REASONER_INTERFACE );
	assert( rP11 != nullptr );
	{
		ReasonerStandardImplementationOptions & r_options = rP11->getOptions<ReasonerStandardImplementationOptions>();
		r_options.role = Role::PROCESS;
		r_options.communicationOptions.comm.componentPointer = comm;
		r_options.communicationOptions.serviceAddress = "ipc://reasonerP11";
		r_options.communicationOptions.upstreamReasoner = "ipc://reasonerN1";
		r_options.communicationOptions.reasonerID = "process11";
		rP11->init(); // This will start a separate Reasoner thread
	}
	TestAnomalyTrigger atP11a;
	rP11->connectTrigger( & atP11a );
	TestAnomalyPlugin adpiP11a;
	rP11->connectAnomalyPlugin( & adpiP11a );

	// Process 12 Reasoner
	// Two ADPIs, one AT
	Reasoner * rP12 = core::module_create_instance<Reasoner>( "", "siox-knowledge-ReasonerStandardImplementation", KNOWLEDGE_REASONER_INTERFACE );
	assert( rP12 != nullptr );
	{
		ReasonerStandardImplementationOptions & r_options = rP12->getOptions<ReasonerStandardImplementationOptions>();
		r_options.role = Role::PROCESS;
		r_options.communicationOptions.comm.componentPointer = comm;
		r_options.communicationOptions.serviceAddress = "ipc://reasonerP12";
		r_options.communicationOptions.upstreamReasoner = "ipc://reasonerN1";
		r_options.communicationOptions.reasonerID = "process12";
		rP12->init(); // This will start a separate Reasoner thread
	}

	TestAnomalyTrigger atP12a;
	rP12->connectTrigger( & atP12a );
	TestAnomalyPlugin adpiP12a;
	rP12->connectAnomalyPlugin( & adpiP12a );
	TestAnomalyPlugin adpiP12b;
	rP12->connectAnomalyPlugin( & adpiP12b );

	// Process 13 Reasoner
	// No ADPIs, one AT
	Reasoner * rP13 = core::module_create_instance<Reasoner>( "", "siox-knowledge-ReasonerStandardImplementation", KNOWLEDGE_REASONER_INTERFACE );
	assert( rP13 != nullptr );
	{
		ReasonerStandardImplementationOptions & r_options = rP13->getOptions<ReasonerStandardImplementationOptions>();
		r_options.role = Role::PROCESS;
		r_options.communicationOptions.comm.componentPointer = comm;
		r_options.communicationOptions.serviceAddress = "ipc://reasonerP13";
		r_options.communicationOptions.upstreamReasoner = "ipc://reasonerN1";
		r_options.communicationOptions.reasonerID = "process13";
		rP13->init(); // This will start a separate Reasoner thread
	}
	TestAnomalyTrigger atP13a;
	rP13->connectTrigger( & atP13a );
	TestAnomalyTrigger atP13b;
	rP13->connectTrigger( & atP13b );

	/*
	 * Inject setup states
	 */
	// TODO: Create these via the ADPIs

/*
	cout << "Injecting states..." << endl;

	ReasonerMessageReceived rmrInject("INJECT"); // Magic word to set reasoner's own state

	SystemHealth sh = { HealthState::OK, {{0,1,5,1,0,0}}, {}, {} };
	((ReasonerStandardImplementation *) rS)->receivedReasonerSystemHealth( rmrInject, sh );
	cout << ((ReasonerStandardImplementation *) rS) << endl;

	NodeHealth nh;
	nh.utilization[UtilizationIndex::CPU] = 20;
	nh.utilization[UtilizationIndex::NETWORK] = 40;
	nh.utilization[UtilizationIndex::IO] = 30;
	nh.utilization[UtilizationIndex::MEMORY] = 10;
	nh.positiveIssues = { { "optimal access pattern type 1", 2, 10 }, {"cache hits", 3, 0} };
	((ReasonerStandardImplementation *) rN1)->receivedReasonerNodeHealth( rmrInject, nh );
	cout << ((ReasonerStandardImplementation *) rN1) << endl;


	ProcessHealth p1 = { HealthState::BAD
, 	{{0,1,5,5,0,0}}, { {"cache hits", 5, 0} }, { {"cache misses", 4, 0} } };
	((ReasonerStandardImplementation *) rP11)->receivedReasonerProcessHealth( rmrInject, p1 );
	cout << ((ReasonerStandardImplementation *) rP11) << endl;

	ProcessHealth p2 = { HealthState::OK, 		{{0,1,5,1,0,0}}, { { "suboptimal access pattern type 1", 2, 10 }, {"cache hits", 2, -1} }, { {"cache misses", 1, +1} } };
	((ReasonerStandardImplementation *) rP12)->receivedReasonerProcessHealth( rmrInject, p2 );
	cout << ((ReasonerStandardImplementation *) rP12) << endl;

	ProcessHealth p3 = { HealthState::GOOD
, 	{{0,5,5,1,0,0}}, { { "optimal access pattern type 1", 2, 10 }, {"cache hits", 3, 0} }, { {"cache misses", 3, 0} } };
	((ReasonerStandardImplementation *) rP13)->receivedReasonerProcessHealth( rmrInject, p3 );
	cout << ((ReasonerStandardImplementation *) rP13) << endl;
*/
	rP13->start();
	rP12->start();
	rP11->start();
	rN1->start();
	rS->start();

	// We begin with a clean slate
	assert( atS.waitForAnomalyCount( 0 ) );
	assert( atN1a.waitForAnomalyCount( 0 ) );
	assert( atN1b.waitForAnomalyCount( 0 ) );
	assert( atP11a.waitForAnomalyCount( 0 ) );
	assert( atP12a.waitForAnomalyCount( 0 ) );
	assert( atP13a.waitForAnomalyCount( 0 ) );


	// Now we inject an observation which will trigger a reaction:
	adpiP11a.injectObservation( {{1,1,1},1}, HealthState::BAD
, "SlownessIssue", 10 );
	usleep(500);
	adpiN1a.injectObservation( {{3,3,3},3}, HealthState::GOOD
, "Quickness Issue", -70 );
	usleep(500);
	adpiP11a.injectObservation( {{2,2,2},2}, HealthState::BAD
, "Slowness Issue", 10 );
	usleep(500);
	adpiP11a.injectObservation( {{1,1,1},1}, HealthState::ABNORMAL_BAD
, "Standstill Issue", 10 );

/*
	assert( at2.waitForAnomalyCount( 1 ) );
	// at1.waitForAnomalyCount( 1 );
	// at2.waitForAnomalyCount( 1 );

	// Now we will query the reactions:
	shared_ptr<HealthStatistics> stats = r->queryRuntimePerformanceIssues();
	cout << "HealthStatistics: " << stats->to_string() << endl;
*/

	assert( atP11a.waitForAnomalyCount( 1 ) );

	cout << "Setup ended, going to sleep..." << endl;
	sleep(1);
	cout << "...waking up!" << endl;

	cout << "...result:" << endl;

	cout << endl << ((ReasonerStandardImplementation *) rP11);
	shared_ptr<HealthStatistics> statsP11 = rP11->queryRuntimePerformanceIssues();
	cout << "HealthStatistics gathered by P11: " << *statsP11 << endl;
	cout << "Anomalies at atP11a: " << atP11a.anomaliesTriggered << endl;

	cout << endl << ((ReasonerStandardImplementation *) rN1);
	shared_ptr<HealthStatistics> statsN1 = rN1->queryRuntimePerformanceIssues();
	cout << "HealthStatistics gathered by N1: " << *statsN1 << endl;
	cout << "Anomalies at atN1a: " << atN1a.anomaliesTriggered << endl;
	cout << "Anomalies at atN1b: " << atN1b.anomaliesTriggered << endl;
	// cout << adpiN1b << endl;

	cout << endl << ((ReasonerStandardImplementation *) rS);


	/*
	 * Cleaning up
	 */
	rP13->stop();
	rP12->stop();
	rP11->stop();
	rN1->stop();
	rS->stop();

	delete(rP13);
	delete(rP12);
	delete(rP11);
	delete(rN1);
	delete(rS);

	delete(comm);

	ComponentID cid = {{1,2,3},4};
	cout << "Cid=" << cid << endl;
}


int main( int argc, char const * argv[] )
{
	testReasoner();

	cout << endl << "=== TEST FINISHED ===" << endl;
	return 0;
}


