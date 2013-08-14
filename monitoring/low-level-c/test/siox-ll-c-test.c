#include <C/siox.h>

#include <assert.h>
#include <stdio.h>


int main(){

	printf("Registering unique interface...");
	siox_unique_interface * i1 = siox_system_information_lookup_interface_id("test", "Impl1");
	assert(i1 != 0);
	printf("success!\n");

	printf("Registering same interface again...");
	siox_unique_interface * i1a = siox_system_information_lookup_interface_id("test", "Impl1");
	assert(i1 != 0);
	assert(i1 == i1a);
	printf("success!\n");

		printf("Registering component...");
	siox_component * c1 = siox_component_register(i1, "localhost4711");
	assert(c1 != NULL);
	printf("success!\n");
	
	printf("Registering attributes...");
	siox_attribute * a1 = siox_ontology_register_attribute("meta", "test int attribute", SIOX_STORAGE_64_BIT_UINTEGER);
	assert(a1 != NULL);
	siox_attribute * a2 = siox_ontology_register_attribute("local", "test str attribute", SIOX_STORAGE_STRING);
	assert(a2 != NULL);
	printf("success!\n");

	printf("Registering metrics...");
	siox_attribute * m1 = siox_ontology_register_attribute_with_unit("local", "throughput/network", "Bytes/s", SIOX_STORAGE_64_BIT_UINTEGER);
	printf("success!\n");

	uint64_t testValue = 4711;

	printf("Adding a meta attribute...");
	siox_ontology_set_meta_attribute(m1, a1, & testValue);
	printf("success!\n");

	printf("Provoking error looking up non-existant attribute...");
	siox_attribute * mNotExisting = siox_ontology_lookup_attribute_by_name("local", "throughput");
	assert(mNotExisting == NULL);
	printf("success!\n");

	printf("Looking up existing attribute...");
	siox_attribute * m2 = siox_ontology_lookup_attribute_by_name("local", "throughput/network");
	assert(m1 == m2);
	printf("success!\n");

	printf("Unregistering component...");
	siox_component_unregister(c1);
	// assert(c1 == NULL);
	printf("success!\n");


	printf("Registering same component again...");
	siox_component * c1a = siox_component_register(i1, "localhost4711");
	assert(c1a != NULL);
	printf("success!\n");
	
	printf("Unregistering component...");
	siox_component_unregister(c1a);
	// assert(c1a == NULL);
	printf("success!\n");


	printf("Registering another unique interface...");
	siox_unique_interface * i2 = siox_system_information_lookup_interface_id("test", "InvalidImpl");
	assert(i2 != 0);
	assert(i2 != i1);
	printf("success!\n");

	printf("Provoking error registering invalid component...");
	siox_component * c2 = siox_component_register(i2, "");
	assert(c2 == NULL);
	printf("success!\n");
	

	return 0;
}
