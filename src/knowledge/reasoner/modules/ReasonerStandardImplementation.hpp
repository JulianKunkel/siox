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

class ReasonerStandardImplementation : public Reasoner, ReasoningDataReceivedCB{
private:
		list<AnomalyTrigger *> triggers;
		list<AnomalyPlugin *>  adpis;
		list<Reasoner *>  reasoners; // remote reasoners

		QualitativeUtilization * utilization = nullptr;


		shared_ptr<HealthStatistics>   runtimeStatistics;

		// for each host (by ID) we store the latest observation
		unordered_map<string, pair<Timestamp, set<HealthStatistics>> > remoteIssues;


		thread                  periodicThread;
		mutex                   recentIssues_lock;
		condition_variable      running_condition;

		bool terminated = false;

		uint32_t update_intervall_ms = -1;

		ReasonerCommunication comm;		
protected:
	ComponentOptions * AvailableOptions() {
			return new ReasonerStandardImplementationOptions();
	}

	virtual void PeriodicRun();

public:

	virtual void receivedReasonerProcessHealth(ReasonerMessageReceived & data, ProcessHealth & health) override;
	virtual void receivedReasonerNodeHealth(ReasonerMessageReceived & data, NodeHealth & health) override;
	virtual void receivedReasonerSystemHealth(ReasonerMessageReceived & data, SystemHealth & health) override;
	virtual shared_ptr<NodeHealth> getNodeHealth() override;
	virtual shared_ptr<SystemHealth> getSystemHealth() override;
	virtual shared_ptr<ProcessHealth> getProcessHealth() override;

	ReasonerStandardImplementation() : comm(*this){

	}

	~ReasonerStandardImplementation();

	virtual void init() override;

	virtual shared_ptr<HealthStatistics> queryRuntimePerformanceIssues() override;

	virtual void connectAnomalyPlugin( AnomalyPlugin * plugin ) override {
		assert( std::find( adpis.begin(), adpis.end(), plugin ) ==  adpis.end() );
		adpis.push_back( plugin );
	}

	virtual void connectUtilization( QualitativeUtilization * plugin ) override {
		assert( utilization == nullptr );
		utilization = plugin;
	}

	virtual void connectTrigger( AnomalyTrigger * trigger ) override {
		// element should not be part of the list so far.
		assert( std::find( triggers.begin(), triggers.end(), trigger ) ==  triggers.end() );
		triggers.push_back( trigger );
	}
};

}

#endif