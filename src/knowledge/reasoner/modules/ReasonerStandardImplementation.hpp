#ifndef KNOWLEDGE_REASONER_MODULES_REASONER_STD_IMPL_HPP_
#define KNOWLEDGE_REASONER_MODULES_REASONER_STD_IMPL_HPP_

#include <list>
#include <map>
#include <algorithm>
#include <set>
#include <thread>
#include <mutex>
#include <condition_variable>

#include <core/component/Component.hpp>
#include <core/reporting/ComponentReportInterface.hpp>

#include <monitoring/statistics/StatisticsCollection.hpp>

#include <knowledge/reasoner/ReasonerImplementation.hpp>
#include <knowledge/reasoner/modules/ReasonerStandardImplementationOptions.hpp>
#include <knowledge/reasoner/modules/ReasonerCommunication.hpp>


using namespace std;
using namespace core;


/*
 Data is pushed into a set of recent PerformanceIssues.
 A background thread fetches these issues and abstracts/refines them into knownIssues.

 The reasoner deduces the local state from local issues taking remote issues into account.
 The deduced information is communicated with remote reasoners.
 There are four roles:
 - process: this reasoner will only aggregate information for the process and forward them.
 - node: this reasoner will aggregate all informations from process reasoners. It forwards the node state upstream and the global information downstream.
 - system: this reasoner will aggregate all node informations and deduct the system state e.g. for file-systems and forward this information to all connected reasoners.
 */

namespace knowledge {

class ReasonerStandardImplementation : public Reasoner, public ReasoningDataReceivedCB, public ComponentReportInterface {
private:
		ReasonerStandardImplementationOptions::Role role; // Our scope: PROCESS, NODE or SYSTEM
		string id;

		// Should anomalies persist for an additional cycle?
		const bool kpersistentAnomalies = true;

		list<AnomalyTrigger *> triggers;
		list<AnomalyPlugin *>  adpis;
		list<Reasoner *>  reasoners; // remote reasoners


		monitoring::StatisticsCollection * nodeStatistics = nullptr;

		// Fields to coordinate threaded behavior
		thread                  periodicThread;
		mutex 					pluginMutex; // To protect state of registered plugins
		mutex 					dataMutex; // To protect state of aggregated data
		mutex 					recentIssuesMutex; // TODO: To protect - what, exactly?
		condition_variable      running_condition;

		bool terminated = false;

		uint32_t update_intervall_ms = -1;

		// Fields used for communicatiion
		ReasonerCommunication * comm = nullptr;
		bool upstreamReasonerExists = false;

		// how often did we send or receive a node count
		uint64_t nPushesSent = 0;
		uint64_t nPushesReceived = 0;

		// internal statistics
		// total number of intervals an anomaly has been triggered
		uint64_t anomaliesTriggered = 0;
		uint64_t cyclesTriggered = 0;

		array<double, NODE_STATISTIC_COUNT> node_statistics;



		// for each host (by ID) we store the latest observation
		// unordered_map<string, pair<Timestamp, set<HealthStatistics>> > remoteIssues;

		/*
		 * Fields to hold current state and past observations
		 */
		// Local or neighbouring reasoners' states, depending on our role
		shared_ptr<ProcessHealth> processHealth;
		shared_ptr<NodeHealth> nodeHealth;
		shared_ptr<SystemHealth> systemHealth;
		// States of possible child reasoners, depending on our role
		unordered_map<string,ProcessHealth> * childProcessesHealthMap = nullptr;
		unordered_map<string,NodeHealth> * childNodesHealthMap  = nullptr;
		// Aggregators for past and current issues and health statistics
		shared_ptr<HealthStatistics> gatheredStatistics;
		uint64_t observationTotal = 0;
		array<uint64_t, HEALTH_STATE_COUNT> observationCounts;
		array<int, HEALTH_STATE_COUNT> observationRatios;
		uint64_t oldObservationTotal = 0;
		array<uint64_t, HEALTH_STATE_COUNT> oldObservationCounts;
		array<int, HEALTH_STATE_COUNT> oldObservationRatios;

		// Methods to compute local health - according to our scope - from observations
		void assessProcessHealth();
		void assessNodeHealth();
		void assessSystemHealth();

protected:
	ComponentOptions * AvailableOptions() {
			return new ReasonerStandardImplementationOptions();
	}

	virtual void PeriodicRun();

public:

	friend std::ostream & operator<<( std::ostream& os, const ReasonerStandardImplementation * r );

	virtual void receivedReasonerProcessHealth(ReasonerMessageReceived & data, ProcessHealth & health) override;
	virtual void receivedReasonerNodeHealth(ReasonerMessageReceived & data, NodeHealth & health) override;
	virtual void receivedReasonerSystemHealth(ReasonerMessageReceived & data, SystemHealth & health) override;
	virtual shared_ptr<NodeHealth> getNodeHealth() override;
	virtual shared_ptr<SystemHealth> getSystemHealth() override;
	virtual shared_ptr<ProcessHealth> getProcessHealth() override;

	void stop() override;
	void start() override;

	ReasonerStandardImplementation() : gatheredStatistics(new HealthStatistics) {
		childProcessesHealthMap = nullptr;
		childNodesHealthMap = nullptr;

		for (int i = 0; i < HEALTH_STATE_COUNT; ++i){
			observationCounts[i] = 0;
			oldObservationCounts[i] = 0;
			observationRatios[i] = 0;
			oldObservationRatios[i] = 0;
		}

		for (int i = 0; i < NODE_STATISTIC_COUNT; ++i){
			node_statistics[i] = 0;
		}

		observationTotal = 0;
		oldObservationTotal = 0;
	}

	~ReasonerStandardImplementation();

	void init() override;

	shared_ptr<HealthStatistics> queryRuntimePerformanceIssues() override;

	ComponentReport prepareReport() override;

	void connectAnomalyPlugin( AnomalyPlugin * plugin ) override {
		unique_lock<mutex> pluginLock( pluginMutex );
		// Element should not be in the list yet
		assert( std::find( adpis.begin(), adpis.end(), plugin ) ==  adpis.end() );
		adpis.push_back( plugin );
		// cout << "ADPIs: " << adpis.size() << endl;
	}

	void connectTrigger( AnomalyTrigger * trigger ) override {
		unique_lock<mutex> pluginLock( pluginMutex );
		// Element should not be in the list yet
		assert( std::find( triggers.begin(), triggers.end(), trigger ) ==  triggers.end() );
		triggers.push_back( trigger );
		// cout << "ATriggers: " << triggers.size() << endl;
	}
};

}

#endif