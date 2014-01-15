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

class ReasonerStandardImplementation : public Reasoner, ReasoningDataReceivedCB, ComponentReportInterface {
private:
		// Our scope: PROCESS, NODE or SYSTEM
		ReasonerStandardImplementationOptions::Role role;
		string id;

		list<AnomalyTrigger *> triggers;
		list<AnomalyPlugin *>  adpis;
		list<Reasoner *>  reasoners; // remote reasoners

		QualitativeUtilization * utilization = nullptr;


		thread                  periodicThread;
		mutex 					pluginMutex; // To protect state of registered plugins
		mutex 					dataMutex; // To protect state of aggregated data
		mutex 					recentIssuesMutex; // TODO: To protect - what, exactly?
		condition_variable      running_condition;

		bool terminated = false;

		uint32_t update_intervall_ms = -1;

		ReasonerCommunication comm;
		bool upstreamReasonerExists = false;

		// for each host (by ID) we store the latest observation
		// unordered_map<string, pair<Timestamp, set<HealthStatistics>> > remoteIssues;

		// Fields to hold current state and past observations
		//
		// Local or neighbouring reasoners' states, depending on our role
		shared_ptr<ProcessHealth> processHealth;
		shared_ptr<NodeHealth> nodeHealth;
		shared_ptr<SystemHealth> systemHealth;
		// States of possible child reasoners, depending on our role
		unordered_map<string,ProcessHealth> * childProcessesHealthMap = nullptr;
		unordered_map<string,NodeHealth> * childNodesHealthMap = nullptr;
		// Aggregators for past and current issues and health statistics
		shared_ptr<HealthStatistics> gatheredStatistics;
		uint64_t observationTotal = 0;
		array<uint64_t, HEALTH_STATE_COUNT> observationCounts;
		array<float, HEALTH_STATE_COUNT> observationRatios;

		// Methods to compute local health - according to our scope - from observations
		void assessProcessHealth();
		void assessNodeHealth();
		void assessSystemHealth();

		// void mergeObservations(list<ProcessHealth> & l, Health & health, array<uint8_t, HEALTH_STATE_COUNT> & observationRatios, uint64_t & observationCount);
protected:
	ComponentOptions * AvailableOptions() {
			return new ReasonerStandardImplementationOptions();
	}

	virtual void PeriodicRun();

public:

	virtual void injectNodeHealth( const NodeHealth & health ); // TODO: Remove after testing!

	virtual void receivedReasonerProcessHealth(ReasonerMessageReceived & data, ProcessHealth & health) override;
	virtual void receivedReasonerNodeHealth(ReasonerMessageReceived & data, NodeHealth & health) override;
	virtual void receivedReasonerSystemHealth(ReasonerMessageReceived & data, SystemHealth & health) override;
	virtual shared_ptr<NodeHealth> getNodeHealth() override;
	virtual shared_ptr<SystemHealth> getSystemHealth() override;
	virtual shared_ptr<ProcessHealth> getProcessHealth() override;

	ReasonerStandardImplementation() :  comm(*this), gatheredStatistics(new HealthStatistics) {
	}

	~ReasonerStandardImplementation();

	virtual void init() override;

	virtual shared_ptr<HealthStatistics> queryRuntimePerformanceIssues() override;

	virtual ComponentReport prepareReport() override;

	virtual void connectAnomalyPlugin( AnomalyPlugin * plugin ) override {
		unique_lock<mutex> pluginLock( pluginMutex );
		// Element should not be in the list yet
		assert( std::find( adpis.begin(), adpis.end(), plugin ) ==  adpis.end() );
		adpis.push_back( plugin );
		cout << "ADPIs: " << adpis.size() << endl;
	}

	virtual void connectUtilization( QualitativeUtilization * plugin ) override {
		unique_lock<mutex> pluginLock( pluginMutex );
		// There can be only one!
		assert( utilization == nullptr );
		utilization = plugin;
		cout << "Utilization: " << ((utilization != nullptr) ? "TRUE" : "FALSE") << endl;
	}

	virtual void connectTrigger( AnomalyTrigger * trigger ) override {
		unique_lock<mutex> pluginLock( pluginMutex );
		// Element should not be in the list yet
		assert( std::find( triggers.begin(), triggers.end(), trigger ) ==  triggers.end() );
		triggers.push_back( trigger );
		cout << "ATriggers: " << triggers.size() << endl;
	}
};

}

#endif