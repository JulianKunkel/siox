#include <assert.h>
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


