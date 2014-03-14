/**
 *	A test for the SIOX FileSurveyor plug-in
 *
 * @author Michaela Zimmer
 * @date 2014-03-14
 */

#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

#include <sys/types.h>
#include <unistd.h>
#include <assert.h>

int main( int argc, char * argv[] )
{
	/*
	 * Variables for POSIX
	 */
	FILE * 			posixFile;
	const char	 	posixFileName[] = "filesurveyor_test_file.posix";
	size_t 			intSize = sizeof(int);
	int 			posixBlockNum = 10; // Number of blocks to write and read
	int				posixBlockSize = 10; // Number of integers in each block
	size_t			posixBufSize;


	/*
	 * Variables for MPI
	 */
	int 			mpiCommRank;
	int 			mpiCommSize;
	char		 	mpiFileName[] = "filesurveyor_test_file.mpi";
	MPI_File 		mpiFile;
	MPI_Datatype 	mpiDataType;
	MPI_Status 		mpiStatus;
	MPI_Info 		mpiInfo;


	/*
	 * General Variables
	 */
	pid_t pid = getpid();
	int * buf;


	/*
	 * POSIX only
	 * ----------
	 * This part of the test should account for
	 * (2 * posixBlockNum - 1) Accesses,
	 * (posixBlockNum) Writes,
	 * (posixBlockNum - 1) Reads,
	 * (posixBlockNum + 3) Managements,
	 * 2 Opens,
	 * 2 Closes,
	 * (posixBlockNum -1) Seeks,
	 * 		[long/short counts determined by
	 *		 (posixBlockSize - 1) * intSize [once]
	 *		 (posixBlockSize - 2) * intSize [on (posixBlockNum - 2) counts]
	 * 		 and seekSizeLimit as set in siox.conf]
	 * ([the sum of the next two]) Bytes accessed,
	 * (posixBlockNum * posixBlockSize * intSize) Bytes written,
	 * ((posixBlockNum - 1) * 2 * intSize) Bytes read,
	 * (((posixBlockNum - 1) * (posixBlockSize - 2) + 1) * intSize) Total seek distance, [all from reading]
	 */

	// Print values to be expected
	{
		printf("\nFileSurveyor-Test\n");
		printf("=================\n");
		printf("Expected values for the POSIX layer:\n");
		printf("Accesses:\t%i\n", (2 * posixBlockNum - 1));
		printf("Reads:\t%i\n", (posixBlockNum - 1));
		printf("Writes:\t%i\n", posixBlockNum);
		printf("Managements:\t%i\n", (posixBlockNum + 3));
		printf("Opens:\t%i\n", 2);
		printf("Closes:\t%i\n", 2);
		printf("Seeks:\t%i\n", (posixBlockNum -1));
		size_t pbw = posixBlockNum * posixBlockSize * intSize;
		size_t pbr = (posixBlockNum - 1) * 2 * intSize;
		printf("Bytes accessed:\t%ld\n", (pbw + pbr));
		printf("Bytes written:\t%ld\n", pbw);
		printf("Bytes read:\t%ld\n", pbr);
		size_t ptsd = ((posixBlockNum - 1) * (posixBlockSize - 2) + 1) * intSize;
		printf("Total seek distance (all from read):\t%ld\n", ptsd);
		double pasd = ptsd / (double) (posixBlockNum - 1);
		printf("Average seek distance (reading):\t%f\n", pasd);
	}

	// Open file for writing
	posixFile = fopen(posixFileName, "w");
	assert(posixFile != NULL);
	// Write posixBlockNum x (posixBufSize x "i")
	posixBufSize = posixBlockSize * intSize;
	buf = (int *) malloc(posixBufSize);
	for(int i = 1; i<= posixBlockNum; i++)
	{
		for(int j=0; j< posixBlockSize; j++)
			buf[j] = i;

		fwrite( (void *) buf, intSize, posixBlockSize, posixFile);
	}
	free(buf);
	// Close file
	fclose(posixFile);

/*
	posixFile = fopen(posixFileName, "r");
	int posixDummy;
	for (int i = 0; i < posixBlockNum; i++)
	{
		printf( "Block %i:", i);
		for(int j=0; j<posixBlockSize; j++)
		{
			fread( (void *) & posixDummy, intSize, 1, posixFile);
			printf( "\t%i", posixDummy );
		}
		printf("\n");
	}
	fclose(posixFile);
*/

	// Open file for reading
	posixFile = fopen(posixFileName, "r");
	assert(posixFile != NULL);
	// Read (posixBlockNum - 1) x the block borders (last int of current block and first int of next block)
	int posixPos = -intSize;
	int posixReadBuf[2];
	for (int i = 1, j = 2; j <= posixBlockNum; i++, j++)
	{
		posixPos += posixBufSize;
		fseek(posixFile, posixPos, SEEK_SET);
		assert( posixPos == ftell(posixFile) );
		// printf( "Seek #%i von %i auf Position %i - wie gefordert.\n", j, (posixBlockNum - 1), posixPos );
		fread( (void *) & posixReadBuf, intSize, 2, posixFile);
		// printf( "Buffer at border of blocks %i and %i: [%i,%i]\n", i, j, posixReadBuf[0], posixReadBuf[1] );
		assert( posixReadBuf[0] == i );
		assert( posixReadBuf[1] == j );
	}

	// Close file
	fclose(posixFile);


	/*
	 * MPI (and POSIX)
	 */

	//printf( "==========MPI_Init()==========\n" );
	MPI_Init( &argc, &argv );

	//printf( "==========MPI_Comm_rank()==========\n" );
	MPI_Comm_rank( MPI_COMM_WORLD, &mpiCommRank );

	//printf( "==========MPI_Comm_size()==========\n" );
	MPI_Comm_size( MPI_COMM_WORLD, &mpiCommSize );
	//printf( "Hello, world, I am %d of %d, pid: %lld .\n", mpiCommRank, mpiCommSize, (long long int) pid );

	mpiDataType = MPI_CHAR;

	//printf( "==========MPI_File_open()==========\n" );

	MPI_Info_create( &mpiInfo );
	MPI_Info_set( mpiInfo, "testhint1", "test.txt" );
	MPI_Info_set( mpiInfo, "testhint2", "2" );
	MPI_Info_set( mpiInfo, "cb_buffer_size", "1024" );

	MPI_File_open( MPI_COMM_WORLD, mpiFileName, MPI_MODE_CREATE | MPI_MODE_RDWR, mpiInfo, &mpiFile );


/*
	//printf( "==========MPI_File_write()==========\n" );
	{
		//Tell the GenericHistory what hint combinations it should evaluate.
		buf = malloc(1);
		buf[0] = 0;

		MPI_Info_set( mpiInfo, "cb_buffer_size", "1048576" );
		MPI_File_set_info( mpiFile, mpiInfo );
		MPI_File_write( mpiFile, buf, 1, mpiDataType, &mpiStatus );

		MPI_Info_set( mpiInfo, "cb_buffer_size", "1024" );
		MPI_File_set_info( mpiFile, mpiInfo );
		MPI_File_write( mpiFile, buf, 1, mpiDataType, &mpiStatus );

		MPI_Info_set( mpiInfo, "cb_buffer_size", "111" );
		MPI_File_set_info( mpiFile, mpiInfo );
		MPI_File_write( mpiFile, buf, 1, mpiDataType, &mpiStatus );

		free(buf);
	}
*/

/*
	//Do some "sensible" writing.
	for(size_t i = 1024; i <= 1024*1024; i *= 2) {
		buf = malloc(i);
		for(size_t j = i; j--; ) buf[j] = j;
		for(size_t j = 10; j--; ) MPI_File_write( mpiFile, buf, i, mpiDataType, &mpiStatus );
		free(buf);
	}
*/

/*
	//Do some "sensible" writing.
	for(size_t i = 1024; i <= 1024*1024; i *= 2) {
		buf = malloc(i);
		for(size_t j = i; j--; ) buf[j] = j;
		for(size_t j = 10; j--; ) MPI_File_write( mpiFile, buf, i, mpiDataType, &mpiStatus );
		free(buf);
	}
*/

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

	MPI_File_set_view( mpiFile, 30, MPI_INT, structType, "native", MPI_INFO_NULL );
	int data = 40;
	MPI_File_seek(mpiFile, 2, MPI_SEEK_CUR);
	MPI_File_write( mpiFile, & data, 1, MPI_INT, &mpiStatus );

	//printf( "==========MPI_File_close()==========\n" );
	MPI_File_close( &mpiFile );

	printf( "==========MPI_Finalize()==========\n" );
	MPI_Finalize();

	return 0;
}
