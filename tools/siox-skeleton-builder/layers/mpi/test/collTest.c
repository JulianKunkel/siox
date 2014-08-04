#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>

#include <mpi.h>

int main( int argc, char * argv[] ){
	int rank, size;
	MPI_File fh;
	MPI_Datatype etype;
	MPI_Status status;
	char* buf;
	MPI_Info info;

	MPI_Init( &argc, &argv );
	MPI_Comm_rank( MPI_COMM_WORLD, &rank );
	MPI_Comm_size( MPI_COMM_WORLD, &size );

 	int val[10];

	int ret = MPI_Bcast(val, 10, MPI_INT, 0, MPI_COMM_WORLD);
	assert( ret == MPI_SUCCESS );

	MPI_Finalize();

	return 0;
}

