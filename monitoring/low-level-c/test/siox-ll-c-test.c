#include <monitoring/low-level-c/siox-ll.h>
#include <assert.h>
#include <stdio.h>


int main(){
	siox_attribute * a1 = siox_attribute_register("test int attribute", SIOX_STORAGE_64_BIT_UINTEGER);
	siox_attribute * a2 = siox_attribute_register("test str attribute", SIOX_STORAGE_STRING);

	siox_metric * m1 = siox_ontology_register_metric("throughput/network", "Bytes/s", SIOX_STORAGE_64_BIT_UINTEGER);

	uint64_t testValue = 4711;

	siox_metric_set_attribute(m1, a1, & testValue);


	siox_metric * mNotExisting = siox_ontology_find_metric_by_name("throughput");
	assert(mNotExisting == NULL);

	siox_metric * m2 = siox_ontology_find_metric_by_name("throughput/network");
	assert(m1 == m2);
}