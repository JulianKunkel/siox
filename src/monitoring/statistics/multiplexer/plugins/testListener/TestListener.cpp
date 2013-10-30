/**
 * @file TestListener.cpp
 *
 * @author Nathanael Hübbe
 * @date 2013
 */

#include <monitoring/statistics/multiplexer/StatisticsMultiplexer.hpp>
#include <monitoring/ontology/Ontology.hpp>

#include "TestListener.hpp"
#include "TestListenerOptions.hpp"

using namespace std;
using namespace monitoring;
using namespace core;

class TestListenerImplementation : public TestListener {
	public:
		virtual void init();
		virtual ComponentOptions * AvailableOptions();

		//virtual const std::vector<std::pair<OntologyAttributeID, Topology::ObjectId> >& requiredMetrics() throw();
		virtual const std::vector<std::pair<OntologyAttributeID, std::vector< std::pair< std::string, std::string> > > >& requiredMetrics() throw();
		virtual void notify(const std::vector<std::shared_ptr<Statistic> >& statistics) throw();

		bool registeredValidInput();
	private:
		Ontology* ontology = 0;
		StatisticsMultiplexer* smux = 0;
		std::vector<std::pair<OntologyAttributeID, std::vector< std::pair< std::string, std::string> > > > requests;
		size_t curValue = 0;
		bool gotInput = false, valid = true;
};

void TestListenerImplementation::init() {
	TestListenerOptions& o = getOptions<TestListenerOptions>();
	ontology =  GET_INSTANCE(Ontology, o.ontology);
	smux = GET_INSTANCE(StatisticsMultiplexer, o.smux);
}

ComponentOptions* TestListenerImplementation::AvailableOptions() {
	return new TestListenerOptions();
}

//const std::vector<std::pair<OntologyAttributeID, Topology::ObjectId> >& TestListenerImplementation::requiredMetrics() throw() {
const std::vector<std::pair<OntologyAttributeID, std::vector< std::pair< std::string, std::string> > > >& TestListenerImplementation::requiredMetrics() throw() {
	if(!requests.size()) {
		OntologyAttributeID anId = ontology->lookup_attribute_by_name( "Statistics", "test/metrics" ).aID;
		std::vector< std::pair< std::string, std::string> > aTopology = {{"node", LOCAL_HOSTNAME}, {"semantics", "testing"}};
		requests.push_back(std::pair<OntologyAttributeID, std::vector< std::pair< std::string, std::string> > >( anId, aTopology));
	}
	return requests;
}

void TestListenerImplementation::notify(const std::vector<std::shared_ptr<Statistic> >& statistics) throw() {
	if( Statistic* myStatistic = &*statistics[0] ) {
		gotInput = true;
		if( myStatistic->curValue != ++curValue ) valid = false;
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
