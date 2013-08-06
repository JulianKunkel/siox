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

	FileAssociationMapperOptions & op = o->getOptions<FileAssociationMapperOptions>();
	op.filename = "association-mapper.dat";
	o->init();

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

	const string & ref = o->lookup_instance_mapping(aid1);
	cout << ref << endl;

	assert( o->lookup_instance_mapping(aid1) == i1 );

	// PROCESS TEST

	ProcessID pid;
	OntologyAttribute a1;
	a1.aID = 1;

	OntologyAttribute a2;
	a2.aID = 2;

	OntologyValue v1(34);
	OntologyValue v2(35);
	o->set_process_attribute(pid, a1, v1);
	o->set_process_attribute(pid, a1, v1);

	try{
		 o->set_process_attribute(pid, a1, v2);
		 assert(false);
	}catch(IllegalStateError & e){}

	try{
		const OntologyValue & vp = o->lookup_process_attribute(pid, a2);
		assert(false);
	}catch(NotFoundError & e){

	}
	
	OntologyValue vp = o->lookup_process_attribute(pid, a1);

	assert(vp == v1 );


	// COMPONENT TEST

	ComponentID cid;
	
	o->set_component_attribute(cid, a1, v1);
	o->set_component_attribute(cid, a1, v1);

	try{
		o->set_component_attribute(cid, a1, v2);
		assert(false);
	}catch(IllegalStateError & e){}

	try{
		vp = o->lookup_component_attribute(cid, a2);
		assert(false);
	}catch(NotFoundError & e){}
	

	vp = o->lookup_component_attribute(cid, a1);
	assert(vp == v1 );

	delete(o);

	cout << "OK" << endl;

	return 0;
}


