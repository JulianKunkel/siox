#include <stdio.h>
#include "mpi.h"

#include <sys/types.h>
#include <unistd.h>


int main( int argc, char * argv[] )
{
	int rank, size;
	MPI_File fh;
	MPI_Datatype etype;
	MPI_Status status;
	char* buf;
	MPI_Info info;

	pid_t pid = getpid();

	printf( "==========MPI_Init()==========\n" );
	MPI_Init( &argc, &argv );
	printf( "==========MPI_Comm_rank()==========\n" );
	MPI_Comm_rank( MPI_COMM_WORLD, &rank );
	printf( "==========MPI_Comm_size()==========\n" );
	MPI_Comm_size( MPI_COMM_WORLD, &size );
	printf( "Hello, world, I am %d of %d, pid: %lld .\n",
	        rank, size, (long long int) pid );

	etype = MPI_CHAR;
	printf( "==========MPI_File_open()==========\n" );

	MPI_Info_create( &info );
	MPI_Info_set( info, "testhint1", "test.txt" );
	MPI_Info_set( info, "testhint2", "2" );
	MPI_Info_set( info, "cb_buffer_size", "1024" );

	MPI_File_open( MPI_COMM_WORLD, "mpi_wrapper_test_file", MPI_MODE_CREATE | MPI_MODE_RDWR, info, &fh );
	printf( "==========MPI_File_write()==========\n" );
	{
		//Tell the GenericHistory what hint combinations it should evaluate.
		buf = malloc(1);
		buf[0] = 0;

		MPI_Info_set( info, "cb_buffer_size", "1048576" );
		MPI_File_set_info( fh, info );
		MPI_File_write( fh, buf, 1, etype, &status );

		MPI_Info_set( info, "cb_buffer_size", "1024" );
		MPI_File_set_info( fh, info );
		MPI_File_write( fh, buf, 1, etype, &status );

		MPI_Info_set( info, "cb_buffer_size", "111" );
		MPI_File_set_info( fh, info );
		MPI_File_write( fh, buf, 1, etype, &status );

		free(buf);
	}
	//Do some "sensible" writing.
	for(size_t i = 1024; i <= 1024*1024; i *= 2) {
		buf = malloc(i);
		for(size_t j = i; j--; ) buf[j] = j;
		for(size_t j = 5; j--; ) MPI_File_write( fh, buf, i, etype, &status );
		free(buf);
	}

	MPI_Datatype darray;
	int array_of_gsizes[4] = {20,20,20,20};
	int array_of_psizes[4] = {2, 2, 2, 2};
	int array_of_distribs[4] = {MPI_DISTRIBUTE_CYCLIC,  MPI_DISTRIBUTE_BLOCK , MPI_DISTRIBUTE_BLOCK, MPI_DISTRIBUTE_BLOCK };
	int array_of_dargs[4] = {10, 10, 10, 10};
	MPI_Type_create_darray(16, 1, 4, array_of_gsizes, array_of_distribs, array_of_dargs, array_of_psizes, MPI_ORDER_C, MPI_INT, & darray);
	MPI_Type_commit(& darray);

	MPI_Aint displs[3];
	MPI_Datatype oldtypes[3];
	int blocklens[3];
	int i;
	for (i=0; i < 3; i++) {
		blocklens[i] = 2*i;
		oldtypes[i] = MPI_INT;
		displs[i] = i * 10;
	}
	oldtypes[2] = darray;
	MPI_Datatype structType;
	MPI_Type_struct( 3, blocklens, displs, oldtypes, & structType );
	MPI_Type_commit(& structType);

	MPI_File_set_view( fh, 30, MPI_INT, structType, "native", MPI_INFO_NULL );
	int data = 40;
	MPI_File_seek(fh, 2, MPI_SEEK_CUR);
	MPI_File_write( fh, & data, 1, MPI_INT, &status );


	printf( "==========MPI_File_close()==========\n" );
	MPI_File_close( &fh );
	printf( "==========MPI_Finalize()==========\n" );
	MPI_Finalize();

	return 0;
}
