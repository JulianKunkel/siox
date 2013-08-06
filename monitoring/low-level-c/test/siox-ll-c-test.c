#include <C/siox.h>

#include <assert.h>
#include <stdio.h>


int main(){
	printf("Registering attributes...");
	siox_attribute * a1 = siox_ontology_register_attribute("meta", "test int attribute", SIOX_STORAGE_64_BIT_UINTEGER);
	siox_attribute * a2 = siox_ontology_register_attribute("local", "test str attribute", SIOX_STORAGE_STRING);
	printf("success!\n");

	printf("Registering metrics...");
	siox_attribute * m1 = siox_ontology_register_attribute_with_unit("local", "throughput/network", "Bytes/s", SIOX_STORAGE_64_BIT_UINTEGER);
	printf("success!\n");

	uint64_t testValue = 4711;

	printf("Adding a meta attribute...");
	siox_ontology_set_meta_attribute(m1, a1, & testValue);
	printf("success!\n");

	printf("Looking up first attribute...");
	siox_attribute * mNotExisting = siox_ontology_lookup_attribute_by_name("local", "throughput");
	assert(mNotExisting == NULL);
	printf("success!\n");

	printf("Looking up second attribute...");
	siox_attribute * m2 = siox_ontology_lookup_attribute_by_name("local", "throughput/network");
	assert(m1 == m2);
	printf("success!\n");

	siox_unique_interface * i1 = siox_system_information_lookup_interface_id("test", "Impl1");
	siox_unique_interface * i2 = siox_system_information_lookup_interface_id("test", "InvalidImpl");

	assert(i1 != NULL);
	assert(i2 != NULL);

	siox_component * c1 = siox_component_register(i1, "localhost4711");
	assert(c1 != NULL);

	siox_component * c2 = siox_component_register(i2, NULL);
	assert(c2 == NULL);


	siox_component_unregister(c1);

	return 0;
}
