#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>

#include <mpi.h>

/*
 This benchmark can be used to measure the instrumentation overhead of SIOX.

*/

uint64_t gettime( void )
{
	struct timespec tp;
	if( clock_gettime( CLOCK_REALTIME, &tp ) != 0 ) {
		return ( uint64_t ) - 1;
	} else {
		return (tp.tv_sec * 1000000000ull) + tp.tv_nsec;
	}
}

double time_in_s( uint64_t time ){
	return time / 1000000000.0;
}

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


	assert(size == 2);
	int num = 2;

	MPI_Barrier(MPI_COMM_WORLD);

	uint64_t start = gettime();

	uint64_t end = gettime();
	int ops = 0;

	if (rank == 0){
		while( time_in_s (end - start) < 10){
			for(int i=0; i < 100; i++){
				MPI_Send( &num, 1, MPI_INT, 1, 4713, MPI_COMM_WORLD);				
			}
			ops+= 100;
			end = gettime();
		}

		num = -1;
		MPI_Send( &num, 1, MPI_INT, 1, 4713, MPI_COMM_WORLD);						
		// MPI_Abort(MPI_COMM_WORLD, 1);
	}
	if (rank == 1){			
		while(num == 2){
			MPI_Recv( &num, 1, MPI_INT, 0, 4713, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
	}

	MPI_Barrier(MPI_COMM_WORLD);

	if (rank == 0){
		end = gettime();
		printf("%fs operations: %d perOperation: %.2f ns/op\n", time_in_s (end - start), ops, 
		time_in_s (end - start) / ops *1000*1000*1000 );
	}


	MPI_Finalize();

	return 0;
}

