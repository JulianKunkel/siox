#include <knowledge/optimizer/OptimizerImplementation.hpp>

using namespace core;
using namespace knowledge;

class OptimizerDefaultImpl : Optimizer{
protected:
	ComponentOptions * AvailableOptions(){
		return new ComponentOptions();
	}
public:
	
	virtual OntologyValue optimalParameter(const OntologyAttribute & attribute){
		// return invalid type
		return OntologyValue();
	}

	virtual bool isPluginRegistered(const OntologyAttribute & attribute){
		return false;
	}

	virtual void unregisterPlugin(const OntologyAttribute & attribute){

	}

	virtual void init(){

	}

	virtual void shutdown(){

	}
};

COMPONENT(OptimizerDefaultImpl)