#include <assert.h>
#include <iostream>

#include <core/module/module-loader.hpp>

#include <monitoring/ontology/Ontology.hpp>

#include "../FileOntologyOptions.hpp"

using namespace std;

using namespace monitoring;
using namespace core;

int main(int argc, char const *argv[]){
	Ontology * o = core::module_create_instance<Ontology>("", "FileOntology", ONTOLOGY_INTERFACE);

	// init plugin
	// not necessary, but for testing...
	FileOntologyOptions op;
	op.filename = "ontology-example.txt";
	o->init(& op);

	string domain("test");
	string s_a1("a1");
	string s_a2("a2");

	OntologyAttribute * a1 = o->register_attribute(domain, s_a1, SIOX_STORAGE_32_BIT_UINTEGER);
	assert(a1->aID != 0);

	OntologyAttribute * a2 = o->register_attribute(domain, s_a2, SIOX_STORAGE_32_BIT_UINTEGER);

	OntologyAttribute * a3 = o->register_attribute(domain, s_a1, SIOX_STORAGE_32_BIT_UINTEGER);
	assert(a3 == a1);
	OntologyAttribute * a4 = o->register_attribute(domain, s_a1, SIOX_STORAGE_64_BIT_UINTEGER);
	assert(a4 == NULL);

	cout << "ID1: " << a1->aID << endl;
	cout << "ID2: " << a2->aID << endl;


	a4 = o->lookup_attribute_by_name(domain, s_a1);
	assert(a4 == a1);


	a4 = o->lookup_attribute_by_ID(a1->aID);
	assert(a4 == a1);

	OntologyValue val(35);
	assert(o->attribute_set_meta_attribute(a1, a2, val));
	assert(o->attribute_set_meta_attribute(a1, a2, val));


	OntologyValue val2(36);
	assert(false == o->attribute_set_meta_attribute(a1, a2, val2));

	assert(o->enumerate_meta_attributes(a1).size() == 1);

	const OntologyValue * ret = o->lookup_meta_attribute(a1, a2);
	cout << "Value: " << *ret << endl;

	o->shutdown();

	cout << "OK" << endl;

	delete(o);

	return 0;
}


