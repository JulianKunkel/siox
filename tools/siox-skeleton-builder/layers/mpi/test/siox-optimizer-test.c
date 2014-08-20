#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <mpi.h>

#include <sys/types.h>
#include <unistd.h>

// compile with mpicc siox-optimizer-test.c -std=c99

int main( int argc, char * argv[] )
{
	int rank, size;
	MPI_File fh;
	char* buf;
	MPI_Info info;

	MPI_Init( &argc, &argv );
	MPI_Comm_rank( MPI_COMM_WORLD, &rank );
	MPI_Comm_size( MPI_COMM_WORLD, &size );

	MPI_Info_create(& info);

	MPI_File_open( MPI_COMM_WORLD, "test.dat", MPI_MODE_CREATE | MPI_MODE_RDWR, info, &fh );
	
	MPI_Info infoUsed;
	int nkeys;

	MPI_File_get_info( fh, & infoUsed );
   MPI_Info_get_nkeys( infoUsed, &nkeys );
   int vallen = MPI_MAX_INFO_VAL;
   for (int i=0; i<nkeys; i++) {
   	  int exists;
   	  char key[MPI_MAX_INFO_KEY];
   	  char val[MPI_MAX_INFO_VAL + 1];
        MPI_Info_get_nthkey( infoUsed, i, key );
 
        vallen = MPI_MAX_INFO_VAL;
        MPI_Info_get( infoUsed, key, vallen, val, & exists );
        printf("Key: %s=%s\n", key, val);
    }

	MPI_File_close( &fh );
	MPI_Finalize();

	return 0;
}
