/**
 * @file TestListener.cpp
 *
 * @author Nathanael Hübbe
 * @date 2013
 */

#include <iostream>

#include <monitoring/statistics/multiplexer/StatisticsMultiplexer.hpp>
#include <monitoring/ontology/Ontology.hpp>
#include <monitoring/topology/Topology.hpp>
#include <knowledge/activity_plugin/ActivityPluginDereferencing.hpp>

#include "TestListener.hpp"
#include "TestListenerOptions.hpp"

using namespace std;
using namespace monitoring;
using namespace core;

class TestListenerImplementation : public TestListener {
	public:
		virtual void initPlugin() throw();
		virtual ComponentOptions * AvailableOptions();

		virtual void notifyAvailableStatisticsChange( const vector<shared_ptr<Statistic> > & statistics, bool addedStatistics, bool removedStatistics ) throw();
		virtual void newDataAvailable() throw();

		bool registeredValidInput();
	private:
		shared_ptr<Statistic> testStatistic;
		Ontology* ontology = 0;
		Topology* topology = 0;
		vector<pair<OntologyAttributeID, vector< pair< string, string> > > > requests;
		int32_t curValue = 0;
		bool gotInput = false;
		bool valid = true;
};

void TestListenerImplementation::initPlugin() throw() {
	TestListenerOptions& o = getOptions<TestListenerOptions>();
	ontology =  GET_INSTANCE(Ontology, o.ontology);
	assert( ontology );
	topology = GET_INSTANCE(ActivityPluginDereferencing, o.dereferencingFacade)->topology();
}

ComponentOptions* TestListenerImplementation::AvailableOptions() {
	return new TestListenerOptions();
}

void TestListenerImplementation::notifyAvailableStatisticsChange( const vector<shared_ptr<Statistic> > & statistics, bool addedStatistics, bool removedStatistics ) throw(){

	OntologyAttributeID anId = ontology->lookup_attribute_by_name( kStatisticsDomain, "test/metrics" ).aID;
	TopologyObject localHost = topology->lookupObjectByPath( "@localhost" );

	for( auto itr = statistics.begin(); itr != statistics.end(); itr++){
		if((*itr)->ontologyId == anId){
			// test if the vector matches.
			if( (*itr)->topologyId == localHost.id() ){
				testStatistic = *itr;
				return;
			}
		}
	}

	// no match
	testStatistic = nullptr;
}

void TestListenerImplementation::newDataAvailable() throw(){
	if ( testStatistic != nullptr ){
		gotInput = true;
		valid = (testStatistic->curValue == ++curValue);
	}else{
		gotInput = false;
		valid = false;
	}
}

bool TestListenerImplementation::registeredValidInput() {
	return gotInput && valid;
}

extern "C" {
	void* MONITORING_STATISTICS_MULTIPLEXER_PLUGIN_INSTANCIATOR_NAME() {
		return new TestListenerImplementation();
	}
}
