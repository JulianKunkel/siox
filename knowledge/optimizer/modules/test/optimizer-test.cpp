#include <assert.h>
#include <iostream>
#include <list>

#include <knowledge/optimizer/Optimizer.hpp>
#include <core/module/module-loader.hpp>
#include <monitoring/ontology/OntologyImplementation.hpp>

#include "../../../../monitoring/ontology/modules/file-ontology/FileOntologyOptions.hpp"

using namespace std;
using namespace knowledge;

OntologyAttribute * att1;
OntologyAttribute * att2;


class OptimizerTestPlugin: public OptimizerPlugin {

private:
	uint64_t count = 0;
	uint64_t fix = 42;

public:

	void init() {

	}

	void shutdown() {

	}

	ComponentOptions * AvailableOptions(){

		return new ComponentOptions();
	}

	OntologyValue optimalParameter(const OntologyAttribute * attribute){
		if (attribute == att1)
			return OntologyValue(fix);
		if (attribute == att2)
			return OntologyValue(count++);
		return OntologyValue(); // Returns an explicitly invalid value!
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
	att1 = ont->register_attribute(domain, s_att1, VariableDatatype::UINT32);
	assert(att1->aID != 0);
	att2 = ont->register_attribute(domain, s_att2, VariableDatatype::UINT32);
	assert(att2->aID != 0);
	assert(att1->aID != att2->aID);


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

	opt->shutdown();

	return 0;
}



/*
#include <iostream>

#include <core/module/module-loader.hpp>

#include <monitoring/association_mapper/AssociationMapper.hpp>

#include "../FileAssociationMapperOptions.hpp"

using namespace std;

using namespace monitoring;
using namespace core;

int main(int argc, char const *argv[]){
	AssociationMapper * o = core::module_create_instance<AssociationMapper>("", "FileAssociationMapper", MONITORING_ASSOCIATION_MAPPER_INTERFACE);
	assert(o != nullptr);

	FileAssociationMapperOptions op;
	op.filename = "association-mapper.dat";
	o->init(& op);

	string i1("test 1");
	string i2("ich bin C++");

	AssociateID aid1 = o->create_instance_mapping(i1);
	AssociateID aid2 = o->create_instance_mapping(i2);
	assert(aid1 != aid2);

	cout << aid1 << endl;
	cout << aid2 << endl;

	o->lookup_instance_mapping(444);
	
	AssociateID aid3 = o->create_instance_mapping(i1);
	cout << aid3 << endl;
	
	assert(aid1 == aid3);

	const string * ref = o->lookup_instance_mapping(aid1);
	assert(ref != nullptr);

	cout << (*ref) << endl;

	assert( (*o->lookup_instance_mapping(aid1)) == i1 );

	// PROCESS TEST

	ProcessID pid;
	OntologyAttribute a1;
	a1.aID = 1;

	OntologyAttribute a2;
	a2.aID = 2;

	OntologyValue v1(34);
	OntologyValue v2(35);
	assert(o->set_process_attribute(& pid, & a1, v1));
	assert(o->set_process_attribute(& pid, & a1, v1));

	assert(false == o->set_process_attribute(& pid, & a1, v2));

	OntologyValue * vp = o->lookup_process_attribute(&pid, &a2);
	assert(vp == nullptr);

	vp = o->lookup_process_attribute(&pid, &a1);

	assert(*vp == v1 );


	// COMPONENT TEST

	ComponentID cid;
	
	assert(o->set_component_attribute(& cid, & a1, v1));
	assert(o->set_component_attribute(& cid, & a1, v1));

	assert(false == o->set_component_attribute(& cid, & a1, v2));

	vp = o->lookup_component_attribute(&cid, &a2);
	assert(vp == nullptr);

	vp = o->lookup_component_attribute(&cid, &a1);
	assert(*vp == v1 );



	o->shutdown();

	cout << "OK" << endl;

	delete(o);

	return 0;
}


*/