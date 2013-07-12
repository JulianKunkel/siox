#include <monitoring/low-level-c/siox-ll.h>
#include <assert.h>
#include <stdio.h>


int main(){
	// make test runable using waf which is a different directory.
	if (chdir ("../../../../monitoring") == 0){
		chdir("../");
	}

	siox_attribute * a1 = siox_ontology_register_attribute("meta", "test int attribute", SIOX_STORAGE_64_BIT_UINTEGER);
	siox_attribute * a2 = siox_ontology_register_attribute("local", "test str attribute", SIOX_STORAGE_STRING);

	siox_attribute * m1 = siox_ontology_register_attribute_with_unit("local", "throughput/network", "Bytes/s", SIOX_STORAGE_64_BIT_UINTEGER);

	uint64_t testValue = 4711;

	siox_ontology_set_meta_attribute(m1, a1, & testValue);

	siox_attribute * mNotExisting = siox_ontology_lookup_attribute_by_name("local", "throughput");
	assert(mNotExisting == NULL);

	siox_attribute * m2 = siox_ontology_lookup_attribute_by_name("local", "throughput/network");
	assert(m1 == m2);
	return 0;
}
