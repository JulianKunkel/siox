/**
 * @file OptimizerStandardImplementation.cpp
 *
 * A simple and straightforward implementation of the abstract Optimizer class.
 *
 * @date 2013-08-01
 * @author Michaela Zimmer
 */

#include <knowledge/optimizer/OptimizerImplementation.hpp>

#include <core/component/Component.hpp>
#include <knowledge/optimizer/OptimizerPlugin.hpp>
#include <monitoring/ontology/OntologyDatatypes.hpp>

#include <unordered_map>


namespace knowledge{

class OptimizerStandardImplementation : public Optimizer{

private:

	// Map to store plugins in, indexed by attributes' IDs
	unordered_map<OntologyAttributeID, OptimizerPlugin*> expert;


protected:

	ComponentOptions * AvailableOptions(){

		return new ComponentOptions();
	}


public:

	virtual void registerPlugin(const OntologyAttribute * attribute, const OptimizerPlugin * plugin){
		assert(attribute != nullptr);
		assert(plugin != nullptr);

		expert[attribute->aID] = (OptimizerPlugin*) plugin;
	}

	
	virtual bool isPluginRegistered(const OntologyAttribute * attribute){
		assert(attribute != nullptr);

		return (expert.find(attribute->aID) != expert.end());
	}


	virtual void unregisterPlugin(const OntologyAttribute * attribute){
		assert(attribute != nullptr);

		expert.erase(attribute->aID);
	}


	virtual OntologyValue optimalParameter(const OntologyAttribute * attribute){
		assert(attribute != nullptr);
		///@todo Check for registered plug-in?
		if (isPluginRegistered(attribute))
			return (expert[attribute->aID]->optimalParameter(attribute));
		else
			return (OntologyValue()); // Will create an explicitly invalid value!
	}


	virtual void init(){

	}


	virtual void shutdown(){

	}
};

COMPONENT(OptimizerStandardImplementation)

} // namespace knowledge

// BUILD_TEST_INTERFACE knowledge/optimizer/modules/
