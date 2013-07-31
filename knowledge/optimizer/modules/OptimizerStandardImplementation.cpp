#include <knowledge/optimizer/OptimizerImplementation.hpp>

#include <core/component/Component.hpp>
//#include <monitoring/activity_multiplexer/ActivityMultiplexerPlugin.hpp>
#include <knowledge/optimizer/OptimizerPlugin.hpp>
#include <monitoring/ontology/OntologyDatatypes.hpp>

namespace knowledge{

class OptimizerStandardImplementation : public Optimizer{

protected:

	// unordered_map<OntologyAttribute, OptimizerPlugin> map();

	ComponentOptions * AvailableOptions(){

		return new ComponentOptions();
	}


public:

	virtual void registerPlugin(const OntologyAttribute & attribute, const OptimizerPlugin & plugin){

	}

	
	virtual bool isPluginRegistered(const OntologyAttribute & attribute){

		return false;
	}


	virtual void unregisterPlugin(const OntologyAttribute & attribute){

	}


	virtual OntologyValue optimalParameter(const OntologyAttribute & attribute){

		// return invalid type
		return OntologyValue();
	}


	virtual void init(){

	}


	virtual void shutdown(){

	}
};

} // namespace knowledge

// COMPONENT(OptimizerStandardImplementation)
// BUILD_TEST_INTERFACE knowledge/optimizer/modules/
