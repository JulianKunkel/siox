#include <knowledge/reasoner/ReasonerImplementation.hpp>

#include <core/component/Component.hpp>

#include <list>
#include <map>
#include <algorithm>    
#include <set>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "ReasonerStandardImplementationOptions.hpp"

using namespace std;
using namespace core;

namespace knowledge{

/*
 Data is pushed into a set of recent PerformanceIssues.
 A background thread fetches these issues and abstracts/refines them into knownIssues.
 */
class ReasonerStandardImplementation : public Reasoner{
private:
	list<AnomalyTrigger*> triggers;
	list<AnomalyPlugin*>  adpis;
	list<Reasoner*>  reasoners; // remote reasoners

	QualitativeUtilization * utilization = nullptr;


	set<PerformanceIssue>   knownIssues;
	set<PerformanceIssue>   recentIssues;


	mutex					remoteIssues_lock;
	// the position in the map depends on the plugin
	map<Reasoner*, set<PerformanceIssue>> remoteIssues;


	thread 					periodicThread;
	mutex					recentIssues_lock;
	condition_variable 		running_condition;

	bool terminated = false;

	uint32_t update_intervall_ms = -1;
protected:
	ComponentOptions * AvailableOptions(){
		return new ReasonerStandardImplementationOptions();
	}

	virtual void PeriodicRun() {	
		while(! terminated){
			//cout << "PeriodicRun" << terminated << endl;
			auto timeout = chrono::system_clock::now() + chrono::milliseconds(update_intervall_ms);

			set<AnomalyPluginObservation> adpiObservations;
			// query recent observations of all connected plugins and integrate them into the local performance issues.
			for(auto itr = adpis.begin(); itr != adpis.end() ; itr++){
				// memory management is going in our responsibility.
				unique_ptr<set<AnomalyPluginObservation>> apo = (*itr)->queryRecentObservations();
				
				// Merge this set of the adpi into the global list.
				mergeObservations(adpiObservations, *apo);
				//cout << adpiObservations.size() << " " << apo->size() << endl;
			}

			// Merge remote issues:
			set<PerformanceIssue> remoteIssues;
			for(auto itr = this->remoteIssues.begin(); itr != this->remoteIssues.end() ; itr++){
				mergeIssues(remoteIssues, itr->second);
			}

			// Think:			
			// Update recent performance issues that are stored locally.
			// Use significiant information as stored in the observations.
			bool anomalyDetected = false;
			// TODO

			// Determine local performance issues based on recent observations and remote issues.
			// TODO

			// For testing:
			if(adpiObservations.size() > 0){
				anomalyDetected = true;
			}
			if(utilization != nullptr){
				StatisticObservation so = utilization->lastObservation(4711);
			}

			// Save recentIssues
			// TODO

			// Update knownIssues based on recentIssues
			// TODO

			// Trigger anomaly if any:
			if (anomalyDetected){
				for(auto itr = triggers.begin(); itr != triggers.end() ; itr++){
					(*itr)->triggerResponseForAnomaly();
				}
			}

			// Forward detected performance issues to connected reasoners.
			for(auto itr = reasoners.begin(); itr != reasoners.end() ; itr++){
				(*itr)->reportRecentIssues(this, recentIssues);
			}

			unique_lock<mutex> lock(recentIssues_lock);
			if(terminated){
				break;
			}
			running_condition.wait_until(lock, timeout);
		}
		//cout << "PeriodicRun finished" << endl;
	}

	void mergeObservations(set<AnomalyPluginObservation> & issues, const set<AnomalyPluginObservation> & newIssues){
		for(auto itr = newIssues.begin(); itr != newIssues.end() ; itr++){
			set<AnomalyPluginObservation>::iterator find = issues.find(*itr);
			const AnomalyPluginObservation & nIssue = *itr;
			// should be the default case.
			if (find == issues.end()){
				// append the item
				issues.insert(nIssue);				
			}else{
				AnomalyPluginObservation & existingIssue = (AnomalyPluginObservation&)(*find);
				existingIssue.occurences += nIssue.occurences;
				existingIssue.delta_time_ms += nIssue.delta_time_ms;
			}
		}
	}

	void mergeIssues(set<PerformanceIssue> & issues, const set<PerformanceIssue> & newIssues){
		for(auto itr = newIssues.begin(); itr != newIssues.end() ; itr++){
			set<PerformanceIssue>::iterator find = issues.find(*itr);
			const PerformanceIssue & nIssue = *itr;
			// should be the default case.
			if (find == newIssues.end()){
				// append the item
				issues.insert(nIssue);				
			}else{
				PerformanceIssue & existingIssue = (PerformanceIssue&)(*find);
				existingIssue.occurences += nIssue.occurences;
				existingIssue.delta_time_ms += nIssue.delta_time_ms;
			}
		}
	}

public:

	~ReasonerStandardImplementation(){
		recentIssues_lock.lock();
			terminated = true;
			running_condition.notify_one();
		recentIssues_lock.unlock();

		periodicThread.join();
	}

	virtual void init(){
		ReasonerStandardImplementationOptions & options = getOptions<ReasonerStandardImplementationOptions>();
		update_intervall_ms = options.update_intervall_ms;
		periodicThread = thread(& ReasonerStandardImplementation::PeriodicRun, this);
	}

	virtual void reportRecentIssues(const Reasoner * reasoner, const set<PerformanceIssue> & issues){
		unique_lock<mutex> lock(remoteIssues_lock);

		// Replace last reported issues in the remote issues set.
		remoteIssues[(Reasoner*) reasoner] = issues;
	}

	virtual unique_ptr<list<PerformanceIssue>> queryRecentPerformanceIssues(){
		auto stats = new list<PerformanceIssue>();
		for(auto itr = recentIssues.begin(); itr != recentIssues.end() ; itr++){
			stats->push_back(*itr);
		}
		return unique_ptr<list<PerformanceIssue> >(stats);
	}

	virtual unique_ptr<list<PerformanceIssue>> queryRuntimePerformanceIssues(){
		auto stats = new list<PerformanceIssue>();
		for(auto itr = knownIssues.begin(); itr != knownIssues.end() ; itr++){
			stats->push_back(*itr);
		}
		return unique_ptr<list<PerformanceIssue>>(stats);
	}

	virtual void connectAnomalyPlugin(AnomalyPlugin * plugin){
		assert(std::find(adpis.begin(), adpis.end(), plugin) ==  adpis.end());
		adpis.push_back(plugin);		
	}

	virtual void connectUtilization(QualitativeUtilization * plugin){
		assert(utilization == nullptr);
		utilization = plugin;
	}

	virtual void connectTrigger(AnomalyTrigger * trigger){	
		// element should not be part of the list so far.
		assert(std::find(triggers.begin(), triggers.end(), trigger) ==  triggers.end());
		triggers.push_back(trigger);
	}

	virtual void connectReasoner(Reasoner * reasoner){
		assert(std::find(reasoners.begin(), reasoners.end(), reasoner) ==  reasoners.end());
		reasoners.push_back(reasoner);
		remoteIssues[reasoner] = set<PerformanceIssue>();
	}

};

} // namespace knowledge
CREATE_SERIALIZEABLE_CLS(ReasonerStandardImplementationOptions)

extern "C"{
	void * KNOWLEDGE_REASONER_INSTANCIATOR_NAME() { return new knowledge::ReasonerStandardImplementation(); }
}
