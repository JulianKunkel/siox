#include <assert.h>
#include <iostream>

#include <core/module/module-loader.hpp>

#include <knowledge/reasoner/Reasoner.hpp>


using namespace std;
using namespace knowledge;

/*
 Teststubs
 */
class TestAnomalyTrigger : public AnomalyTrigger{
public:
	int anomaliesTriggered = 0;

	void triggerResponseForAnomaly(){
		anomaliesTriggered++;
	}
};

class TestQualitativeUtilization : public QualitativeUtilization{
public:
	StatisticObservation nextObservation = StatisticObservation::AVERAGE;

	StatisticObservation lastObservation(monitoring::OntologyAttributeID id) const throw(NotFoundError){
		return nextObservation;
	}
};


int main(int argc, char const *argv[]){

	// Obtain a FileOntology instance from module loader
	Reasoner * r = core::module_create_instance<Reasoner>("", "ReasonerStandardImplementation", KNOWLEDGE_REASONER_INTERFACE);
	assert( r != nullptr);

	TestAnomalyTrigger at1;
	TestAnomalyTrigger at2;
	QualitativeUtilization qu;


	r->init();
	r->connectTrigger(& at1);
	r->connectTrigger(& at2);

	r->connectUtilization(& qu);

	list<PerformanceIssue> lst 		= r->queryRecentPerformanceIssues();
	list<PerformanceIssue> stats 	= r->queryRuntimePerformanceIssues();

	cout << "Anomalies: " << at1.anomaliesTriggered << endl;
	cout << "sizeof(PerformanceIssue): " << sizeof(PerformanceIssue) << endl;

	delete(r);

	return 0;
}


