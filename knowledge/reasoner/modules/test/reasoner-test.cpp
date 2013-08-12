#include <assert.h>
#include <iostream>
#include <mutex>
#include <condition_variable>

#include <core/module/module-loader.hpp>

#include <knowledge/reasoner/Reasoner.hpp>
#include <knowledge/reasoner/AnomalyPlugin.hpp>


using namespace std;
using namespace knowledge;

/*
 Teststubs
 */
class TestAnomalyTrigger : public AnomalyTrigger{
private:
	mutex					clock;
	condition_variable 		cond;
public:
	int anomaliesTriggered = 0;


	void triggerResponseForAnomaly(){				
		clock.lock();
		anomaliesTriggered++;			
		cond.notify_one();
		clock.unlock();
		cout << "Triggered" << anomaliesTriggered << endl;
	}

	bool waitForAnomalyCount(int i){
		while(1){
			auto timeout = chrono::system_clock::now() + chrono::milliseconds(30);

			unique_lock<mutex> lock(clock);
			//cout << "A" <<anomaliesTriggered << endl;
			if(anomaliesTriggered >= i){
				//cout << "T" << endl;
				return true;
			}
			if( cond.wait_until(lock, timeout) == cv_status::timeout){
				//cout << "F" << endl;
				return false;
			}
		}		
	}
};

class TestQualitativeUtilization : public QualitativeUtilization{
public:
	StatisticObservation nextObservation = StatisticObservation::AVERAGE;

	StatisticObservation lastObservation(monitoring::OntologyAttributeID id) const throw(NotFoundError){
		return nextObservation;
	}
};

class TestAnomalyPlugin : public AnomalyPlugin{
public:
	set<AnomalyPluginObservation> * recentObservations = new set<AnomalyPluginObservation>();

	unique_ptr<set<AnomalyPluginObservation>> queryRecentObservations(){
		unique_ptr<set<AnomalyPluginObservation>> ret(recentObservations);
		recentObservations = new set<AnomalyPluginObservation>();
		return ret;
	}

	void injectObservation(const AnomalyPluginObservation & observation){
		recentObservations->insert(observation);
	}
};


int main(int argc, char const *argv[]){

	// Obtain a FileOntology instance from module loader
	Reasoner * r = core::module_create_instance<Reasoner>("", "ReasonerStandardImplementation", KNOWLEDGE_REASONER_INTERFACE);
	assert( r != nullptr);

	TestAnomalyTrigger at1;
	TestAnomalyTrigger at2;
	TestQualitativeUtilization qu;

	TestAnomalyPlugin adpi1;

	r->init();
	r->connectTrigger(& at1);
	r->connectTrigger(& at2);

	r->connectUtilization(& qu);
	r->connectAnomalyPlugin(& adpi1);

	// in this initial test we have no anomaly:
	assert(at1.waitForAnomalyCount(0));
	assert(at2.waitForAnomalyCount(0));

	// Now we inject an observation which will trigger an reaction:
	adpi1.injectObservation(AnomalyPluginObservation(AnomalyObservation::UNEXPECTED_FAST, "", 1, 2, {2,3,4}, 5, 30 ));

	assert(at1.waitForAnomalyCount(1));
	assert(at2.waitForAnomalyCount(1));

	// Now we will query the reactions:
	unique_ptr<list<PerformanceIssue>> lst 		= r->queryRecentPerformanceIssues();
	unique_ptr<list<PerformanceIssue>> stats 	= r->queryRuntimePerformanceIssues();

	cout << "Anomalies: " << at1.anomaliesTriggered << endl;
	cout << "sizeof(PerformanceIssue): " << sizeof(PerformanceIssue) << endl;


	delete(r);

	return 0;
}


