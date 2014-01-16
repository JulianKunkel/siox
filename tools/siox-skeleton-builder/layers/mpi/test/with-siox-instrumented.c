#include <mpi.h>
#include <C/siox.h>

#include <stdio.h>

// mpicc with-siox-instrumented.c  -I /home/siox/install/include/ -L /home/siox/install/lib -l siox-monitoring-siox-ll

static siox_component          *siox_c = NULL;
static siox_unique_interface   *siox_uiid = NULL;
static siox_component_activity *siox_c_activity = NULL;

int main(int argc, char ** argv){
	open("test.dat", 22); // just to make sure we have linked against the posix wrapper

	int rank;
	MPI_Init(& argc, & argv);
	MPI_Comm_rank(MPI_COMM_WORLD, & rank);

	printf("Hello from %d\n", rank);
	siox_activity *siox_activity = NULL;

        siox_uiid = siox_system_information_lookup_interface_id("MPI", "OMPIO");
        siox_c = siox_component_register(siox_uiid, "");
        siox_c_activity = siox_component_register_activity(siox_uiid, "GPFS_hints");

	siox_activity = siox_activity_start(siox_c, siox_c_activity);

        siox_activity_stop(siox_activity);
        siox_activity_end(siox_activity);

	MPI_Finalize();

	return 0;
}
