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
	FileOntologyOptions * op = new FileOntologyOptions();
	op->filename = "ontology-example.dat";
	o->init(op);

	string domain("test");
	string s_a1("a1");
	string s_a2("a2");
	string s_a3("stringAttr");

	OntologyAttribute * a1 = o->register_attribute(domain, s_a1, VariableDatatype::UINT32);
	assert(a1->aID != 0);

	OntologyAttribute * a2 = o->register_attribute(domain, s_a2, VariableDatatype::UINT32);

	OntologyAttribute * a3 = o->register_attribute(domain, s_a1, VariableDatatype::UINT32);
	assert(a3 == a1);
	OntologyAttribute * a4 = o->register_attribute(domain, s_a1, VariableDatatype::UINT64);
	assert(a4 == NULL);

	OntologyAttribute * a5 = o->register_attribute(domain, s_a3, VariableDatatype::STRING);

	cout << "ID1: " << a1->aID << endl;
	cout << "ID2: " << a2->aID << endl;


	a4 = o->lookup_attribute_by_name(domain, s_a1);
	assert(a4 == a1);


	a4 = o->lookup_attribute_by_ID(a1->aID);
	assert(a4 == a1);
 
	OntologyValue val((uint32_t) 35);
	OntologyValue vText("test&my");
	OntologyValue vText2("test&my");
	assert(vText == vText2);

	assert(a5 != nullptr);

	assert(o->attribute_set_meta_attribute(a1, a2, val));
	assert(o->attribute_set_meta_attribute(a1, a2, val));
	assert(o->attribute_set_meta_attribute(a1, a5, vText));


	OntologyValue val2((uint32_t) 36);
	assert(false == o->attribute_set_meta_attribute(a1, a2, val2));

	assert(o->enumerate_meta_attributes(a1).size() == 2);

	const OntologyValue * ret = o->lookup_meta_attribute(a1, a2);
	cout << "Type: " << ret->type() << " "<< endl;
	cout << "Val: " << ret->uint32() << endl;

	o->shutdown();

	cout << "OK" << endl;

	delete(o);

	return 0;
}


