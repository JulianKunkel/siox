#include "ListenerSkeleton.hpp"

using namespace std;
using namespace monitoring;
using namespace core;

class ListenerSkeletonImplementation : public ListenerSkeleton {
	public:
		virtual void init();
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

void ListenerSkeletonImplementation::init() {
	ListenerSkeletonOptions& o = getOptions<ListenerSkeletonOptions>();
	ontology =  GET_INSTANCE(Ontology, o.ontology);
	smux = GET_INSTANCE(StatisticsMultiplexer, o.smux);
}

//const std::vector<std::pair<OntologyAttributeID, Topology::ObjectId> >& ListenerSkeletonImplementation::requiredMetrics() throw() {
const std::vector<std::pair<OntologyAttributeID, std::vector< std::pair< std::string, std::string> > > >& ListenerSkeletonImplementation::requiredMetrics() throw() {
	if(!requests.size()) {
		OntologyAttributeID anId = ontology->lookup_attribute_by_name( "Statistics", "test/metrics" );
		std::vector< std::pair< std::string, std::string> > aTopology = {{"node", LOCAL_HOSTNAME}, {"semantics", "testing"}};
		requests.push_back(std::pair<OntologyAttributeID, std::vector< std::pair< std::string, std::string> > >( inId, aTopology));
	}
	return requests;
}

void ListenerSkeletonImplementation::notify(const std::vector<std::shared_ptr<Statistic> >& statistics) throw() {
	if( Statistic* myStatistic = &*statistics[0] ) {
		gotInput = true;
		if( ++curValue != myStatistic->curValue ) valid = false;
	}
}

bool ListenerSkeletonImplementation::registeredValidInput() {
	return gotInput && valid;
}

extern "C" {
	void* MONITORING_STATISTICS_LISTENER_PLUGIN_INSTANCIATOR_NAME() {
		return new ListenerSkeletonImplementation();
	}
}
