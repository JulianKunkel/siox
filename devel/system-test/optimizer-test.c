#include <stdio.h>

#include <assert.h>
#include <C/siox.h>

#include <mpi.h>
// mpicc optimizer-test.c -l siox-monitoring-siox-ll -g

int main(int argc, char ** argv){
	MPI_Init(& argc, & argv);
	siox_attribute * infoReadBuffSize;
	infoReadBuffSize = siox_ontology_register_attribute( "MPI", "hints/noncollReadBuffSize", SIOX_STORAGE_64_BIT_UINTEGER ); 

	assert(infoReadBuffSize != NULL);
	
	siox_component * global_component = NULL;
    siox_unique_interface * global_uid = NULL;
      
	global_uid = siox_system_information_lookup_interface_id("MPI", "Generic");
	global_component = siox_component_register(global_uid, """");

	assert( global_component );

	char value[MPI_MAX_INFO_VAL];
	int ret = siox_suggest_optimal_value_str( global_component, infoReadBuffSize, value, MPI_MAX_INFO_VAL );

	printf("%d\n", ret);
	printf("VAL: %s\n", value);

	MPI_Finalize();
}