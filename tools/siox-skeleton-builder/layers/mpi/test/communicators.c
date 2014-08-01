#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

#include <sys/types.h>
#include <unistd.h>



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

	// split the ranks into two groups: one for odd and one for even procs.
	MPI_Comm oddeven;
	MPI_Comm_split(MPI_COMM_WORLD, rank % 2, 0, & oddeven);


	int oddevenRank;
	MPI_Comm_rank( oddeven, & oddevenRank );
	printf("%d %d\n", rank, oddevenRank);

	MPI_Comm duplicate;
	MPI_Comm_dup( oddeven, & duplicate );

	MPI_Group group;
	MPI_Comm_group( oddeven, & group );

	MPI_Comm lastDuplicate;
	MPI_Comm_create( MPI_COMM_WORLD, group,  &lastDuplicate );


	if (size > 2){
		int num = rank;

		if (rank == 0){
			MPI_Send( &num, 1, MPI_INT, 1, 4713, MPI_COMM_WORLD);
			MPI_Send( &num, 1, MPI_INT, 1, 4711, MPI_COMM_WORLD);
		}
		if (rank == 1){			
			MPI_Status status;
			MPI_Recv( &num, 1, MPI_INT, 0, 4713, MPI_COMM_WORLD, & status);
			MPI_Recv( &num, 1, MPI_INT, 0, 4711, MPI_COMM_WORLD, MPI_STATUS_IGNORE);			
		}
	}

	MPI_Barrier( lastDuplicate );

	MPI_Finalize();

	return 0;
}


