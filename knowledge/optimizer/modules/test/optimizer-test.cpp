#include <assert.h>
#include <iostream>
#include <list>

#include <knowledge/optimizer/Optimizer.hpp>
#include <core/module/module-loader.hpp>
#include <monitoring/ontology/OntologyImplementation.hpp>

#include "../../../../monitoring/ontology/modules/file-ontology/FileOntologyOptions.hpp"

using namespace std;
using namespace knowledge;

OntologyAttribute att1;
OntologyAttribute att2;


class OptimizerTestPlugin: public OptimizerPlugin {

private:
	mutable uint64_t count = 0;
	mutable uint64_t fix = 42;
public:

	void init() {

	}

	ComponentOptions * AvailableOptions(){
		return new ComponentOptions();
	}

	OntologyValue optimalParameter(const OntologyAttribute & attribute) const throw (NotFoundError){
		if (attribute == att1)
			return OntologyValue(fix);
		if (attribute == att2)
			return OntologyValue(count++);
		throw NotFoundError("Illegal attribute!");
	}

};


int main(int argc, char const *argv[]){

	// Obtain a FileOntology instance from module loader
	Ontology * ont = core::module_create_instance<Ontology>("", "FileOntology", ONTOLOGY_INTERFACE);
	assert( ont != nullptr);

	// Create and set its options object
	FileOntologyOptions * ont_opt = new FileOntologyOptions();
	ont_opt->filename = "optimizer-ontology-example.dat";
	ont->init(ont_opt);

	//Create some attributes
	string domain("test");
	string s_att1("Attribute 1");
	string s_att2("Attribute 2");
	att1 = ont->register_attribute(domain, s_att1, VariableDatatype::Type::UINT32);
	assert(att1.aID != 0);
	att2 = ont->register_attribute(domain, s_att2, VariableDatatype::Type::UINT32);
	assert(att2.aID != 0);
	assert(att1.aID != att2.aID);


	// Obtain an Optimizer instance from module loader
	Optimizer * opt = core::module_create_instance<Optimizer>("", "OptimizerStandardImplementation", KNOWLEDGE_OPTIMIZER_INTERFACE);
	assert( opt != nullptr);

	opt->init();

	// Create new OptimizerPlugin instance
	OptimizerPlugin * opt_pi = new OptimizerTestPlugin();

	// Register example plug-in for some attributes
	opt->registerPlugin(att1, opt_pi);
	assert(opt->isPluginRegistered(att1));
	opt->registerPlugin(att2, opt_pi);
	assert(opt->isPluginRegistered(att2));

	// Poll optimizer for values plug-in provides
	assert(opt->optimalParameter(att1).uint64() == 42);
	assert(opt->optimalParameter(att2).uint64() == 0);
	assert(opt->optimalParameter(att1).uint64() == 42);
	assert(opt->optimalParameter(att2).uint64() == 1);
	assert(opt->optimalParameter(att1).uint64() == 42);
	assert(opt->optimalParameter(att2).uint64() == 2);
	assert(opt->optimalParameter(att1).uint64() == 42);


	// Unregister example plug-in
	opt->unregisterPlugin(att1);
	assert(! opt->isPluginRegistered(att1));
	opt->unregisterPlugin(att2);
	assert(! opt->isPluginRegistered(att2));

	delete(ont);
	delete(opt);

	return 0;
}


