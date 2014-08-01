#include <stdio.h>
#include <mpi.h>

// mpicc test-mpi-behavior.c -g -std=c99 -pedantic -pedantic-errors -Werror

int main( int argc, char * argv[] ){
	MPI_Init( &argc, &argv );

	MPI_Comm cw;
	MPI_Comm_dup( MPI_COMM_WORLD, & cw );

	MPI_Comm c1 = cw;

#ifdef INTEGER_TEST
	if ( (int) c1 != (int) MPI_COMM_WORLD && (int) c1 == (int) cw ){
		printf("Integers!\n");
	}
#else
	if ( (void*) c1 != (void*) MPI_COMM_WORLD && (void*) c1 == (void*) cw ){
		printf("Pointers!\n");
	}
#endif

	MPI_Finalize();
	return 0;
}