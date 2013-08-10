#include <knowledge/reasoner/ReasonerImplementation.hpp>

#include <core/component/Component.hpp>

#include <list>
#include <algorithm>    
#include <set>


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
	QualitativeUtilization * utilization = nullptr;

	set<PerformanceIssue>   knownIssues;

	set<PerformanceIssue>   recentIssues;

protected:
	ComponentOptions * AvailableOptions(){
		return new ComponentOptions();
	}

public:
	virtual void init(){

	}

	virtual void reportObservation(StatisticObservation o, OntologyAttributeID statistics_aid, const IssueLocation & issueLocation){		
	}

	virtual void reportObservation(AnomalyObservation o, const IssueLocation & issueLocation, int32_t	delta_time_ms){
		//PerformanceIssue pi())
		//recentIssues
	}

	virtual void reportObservation(AnomalyObservation o, const IssueLocation & issueLocation,  const IssueCause & claimedCause, const IssueLocation & causeLocation, int32_t delta_time_ms){

	}
	
	virtual list<PerformanceIssue> queryRecentPerformanceIssues(){
		list<PerformanceIssue> stats;
		for(auto itr = recentIssues.begin(); itr != recentIssues.end() ; itr++){
			stats.push_back(*itr);
		}
		return stats;
	}

	virtual list<PerformanceIssue> queryRuntimePerformanceIssues(){
		list<PerformanceIssue> stats;
		for(auto itr = knownIssues.begin(); itr != knownIssues.end() ; itr++){
			stats.push_back(*itr);
		}
		return stats;
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

};

} // namespace knowledge


COMPONENT(knowledge::ReasonerStandardImplementation)
