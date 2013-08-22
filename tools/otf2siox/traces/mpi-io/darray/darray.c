/*
 * %Id: darray.c,v 1.2 2003/10/26 22:48:13 gustav Exp %
 *
 * %Log: darray.c,v %
 * Revision 1.2  2003/10/26 22:48:13  gustav
 * Expunged any references to "info".
 *
 * Revision 1.1  2003/10/26 22:05:50  gustav
 * Initial revision
 *
 * mpiexec -n $NODES darray -f test -v
 */

#include <stdio.h>     /* printf and relatives live here */
#include <stdlib.h>    /* exit lives here */
#include <unistd.h>    /* getopt lives here */
#include <string.h>    /* strlen lives here */
#include <sys/types.h> /* chmod needs these two */
#include <sys/stat.h>
#include <mpi.h>       /* all MPI stuff lives here (including MPI-IO) */

#define MASTER_RANK 0
#define TRUE 1
#define FALSE 0
#define BOOLEAN int
#define MBYTE 1048576
#define NDIMS  3
#define SIZE 512
#define SYNOPSIS printf ("synopsis: %s -f <file>\n", argv[0])


int main( argc, argv )
int argc;
char * argv[];
{
	/* my variables */

	int my_rank, pool_size, i, ndims, order, file_name_length,
	    array_of_gsizes[NDIMS], array_of_distribs[NDIMS],
	    array_of_dargs[NDIMS], array_of_psizes[NDIMS],
	    *write_buffer, write_buffer_size, count,
	    *read_buffer, read_buffer_size;
	BOOLEAN i_am_the_master = FALSE, input_error = FALSE,
	        file_open_error = FALSE, file_write_error = FALSE,
	        verbose = FALSE, my_read_error = FALSE, read_error = FALSE;
	char * file_name = NULL, message[BUFSIZ];

	/* MPI variables */

	MPI_Offset file_size;
	MPI_File fh;
	MPI_Status status;
	MPI_Datatype file_type;
	MPI_Aint file_type_size, file_type_extent;
	int error_string_length;
	char error_string[BUFSIZ];

	/* getopt variables */

	extern char * optarg;
	int c;

	/* error handling variables */

	extern int errno;

	/* ACTION */

	MPI_Init( &argc, &argv );
	MPI_Comm_rank( MPI_COMM_WORLD, &my_rank );
	MPI_Comm_size( MPI_COMM_WORLD, &pool_size );

	if( my_rank == MASTER_RANK )
		i_am_the_master = TRUE;

	if( i_am_the_master ) {

		while( ( c = getopt( argc, argv, "f:vh" ) ) != EOF )

			switch( c ) {
				case 'f':
					file_name = optarg;
					break;
				case 'v':
					verbose = TRUE;
					break;
				case 'h':
					input_error = TRUE;
					break;
				case '?':
					input_error = TRUE;
					break;
			}

		if( file_name == NULL )
			input_error = TRUE;

		if( input_error )
			SYNOPSIS;
		else {
			file_name_length = strlen( file_name ) + 1;
			if( verbose ) {
				printf( "file_name         = %s\n", file_name );
				printf( "file_name_length  = %d\n",
				        file_name_length );
			}
		}

	}

	MPI_Bcast( &input_error, 1, MPI_INT, MASTER_RANK, MPI_COMM_WORLD );

	if( !input_error ) {

		MPI_Bcast( &verbose, 1, MPI_INT, MASTER_RANK, MPI_COMM_WORLD );
		MPI_Bcast( &file_name_length, 1, MPI_INT, MASTER_RANK,
		           MPI_COMM_WORLD );

		if( !i_am_the_master )
			file_name = ( char * ) malloc( file_name_length );

		MPI_Bcast( file_name, file_name_length, MPI_CHAR, MASTER_RANK,
		           MPI_COMM_WORLD );

		/* Prepare for calling MPI_Type_create_darray */

		ndims = NDIMS;

		for( i = 0; i < ndims; i++ ) {
			array_of_gsizes[i] = SIZE;
			array_of_distribs[i] = MPI_DISTRIBUTE_BLOCK;
			array_of_dargs[i] = MPI_DISTRIBUTE_DFLT_DARG;
			array_of_psizes[i] = 0;
		}

		MPI_Dims_create( pool_size, ndims, array_of_psizes );
		order = MPI_ORDER_C;

		/* Now call MPI_Type_create_darray */

		if( verbose ) {
			printf( "%3d: calling MPI_Type_create_darray with\n",
			        my_rank );
			printf( "%3d:    pool_size         = %d\n", my_rank,
			        pool_size );
			printf( "%3d:    my_rank           = %d\n", my_rank,
			        my_rank );
			printf( "%3d:    ndims             = %d\n", my_rank,
			        ndims );
			printf( "%3d:    array_of_gsizes   = (%d, %d, %d)\n",
			        my_rank, array_of_gsizes[0], array_of_gsizes[1],
			        array_of_gsizes[2] );
			printf( "%3d:    array_of_distribs = (%d, %d, %d)\n",
			        my_rank, array_of_distribs[0],
			        array_of_distribs[1], array_of_distribs[2] );
			printf( "%3d:    array_of_dargs    = (%d, %d, %d)\n",
			        my_rank, array_of_dargs[0], array_of_dargs[1],
			        array_of_dargs[2] );
			printf( "%3d:    array_of_psizes   = (%d, %d, %d)\n",
			        my_rank, array_of_psizes[0], array_of_psizes[1],
			        array_of_psizes[2] );
			printf( "%3d:    order             = %d\n", my_rank,
			        order );
			printf( "%3d:    type              = %d\n", my_rank,
			        MPI_INT );
		}

		MPI_Type_create_darray( pool_size, my_rank, ndims,
		                        array_of_gsizes, array_of_distribs,
		                        array_of_dargs, array_of_psizes, order,
		                        MPI_INT, &file_type );
		MPI_Type_commit( &file_type );

		/* Explore the returned type */

		MPI_Type_extent( file_type, &file_type_extent );
		MPI_Type_size( file_type, &file_type_size );

		if( verbose ) {
			printf( "%3d: file_type_size   = %d\n", my_rank,
			        file_type_size );
			printf( "%3d: file_type_extent = %d\n", my_rank,
			        file_type_extent );
		}

		/* Allocate space for your own write buffer based on the
		return of the MPI_Type_size call. */

		write_buffer_size = file_type_size / sizeof( int );
		write_buffer = ( int * ) malloc( write_buffer_size * sizeof( int ) );

		/* We do this in case sizeof(int) does not divide file_type_size
		exactly. But this should not happen if we have called
		MPI_Type_create_darray with MPI_INT as the original data
		type. */

		if( !write_buffer ) {
			sprintf( message, "%3d: malloc write_buffer", my_rank );
			perror( message );
			MPI_Abort( MPI_COMM_WORLD, errno );

			/* We can still abort, because we have not opened any
			files yet. Notice that since MPI_Type_create_darray
			will fail if SIZE^3 * sizeof(int) exceeds MAX_INT,
			because MPI_Aint on AVIDD is a 32-bit integer,
			we are rather unlikely to fail on this malloc
			anyway. */
		}

		MPI_Barrier( MPI_COMM_WORLD );
		/* We wait here in case some procs have problems with malloc. */

		/* Initialize the buffer */

		for( i = 0; i < write_buffer_size; i++ )
			*( write_buffer + i ) = my_rank * SIZE + i;

		file_open_error = MPI_File_open( MPI_COMM_WORLD, file_name,
		                                 MPI_MODE_CREATE |
		                                 MPI_MODE_WRONLY, MPI_INFO_NULL,
		                                 &fh );

		if( file_open_error != MPI_SUCCESS ) {

			MPI_Error_string( file_open_error, error_string,
			                  &error_string_length );
			fprintf( stderr, "%3d: %s\n", my_rank, error_string );
			MPI_Abort( MPI_COMM_WORLD, file_open_error );

			/* It is still OK to abort, because we have failed to
			    open the file. */

		} else {

			if( i_am_the_master )
				chmod( file_name, S_IRUSR | S_IWUSR | S_IRGRP |
				       S_IROTH );

			MPI_Barrier( MPI_COMM_WORLD );

			/* We should be able to change permissions on the file by setting
			the "file_perm" hint in the info structure before passing
			it to MPI_File_open, but MPICH2 doesn't support this
			yet. All processes have to meet on the barrier before
			further action. */

			MPI_File_set_view( fh, 0, MPI_INT, file_type, "native",
			                   MPI_INFO_NULL );

			file_write_error = MPI_File_write_all( fh, write_buffer,
			                                       write_buffer_size,
			                                       MPI_INT, &status );

			if( file_write_error != MPI_SUCCESS ) {
				MPI_Error_string( file_write_error, error_string,
				                  &error_string_length );
				fprintf( stderr, "%3d: %s\n", my_rank, error_string );
				MPI_File_close( &fh );
				free( write_buffer );

				if( i_am_the_master )
					MPI_File_delete( file_name,
					                 MPI_INFO_NULL );

			} else {

				MPI_Get_count( &status, MPI_INT, &count );
				MPI_File_get_size( fh, &file_size );

				if( verbose ) {
					printf( "%3d: wrote %d integers\n",
					        my_rank, count );
					printf( "%3d: file size is %lld bytes\n",
					        my_rank, file_size );
				}

				MPI_File_close( &fh );

				/* We have managed to open, write on it and close the file.
				Now we're going to read it the same way we wrote it. */

				read_buffer_size = write_buffer_size;
				read_buffer = ( int * ) malloc( read_buffer_size
				                                * sizeof( int ) );

				if( !read_buffer ) {
					sprintf( message,
					         "%3d: malloc read_buffer",
					         my_rank );
					perror( message );
					MPI_Abort( MPI_COMM_WORLD, errno );

					/* We can abort, because the file has been closed and
					we haven't opened it for reading yet. */
				}

				MPI_Barrier( MPI_COMM_WORLD );
				/* We wait here in case some procs have problems with malloc. */

				MPI_File_open( MPI_COMM_WORLD, file_name,
				               MPI_MODE_RDONLY, MPI_INFO_NULL,
				               &fh );

				/* We don't check for errors here, because we've just closed
				this file a moment ago, so it should still be there. */

				MPI_File_set_view( fh, 0, MPI_INT, file_type,
				                   "native", MPI_INFO_NULL );
				MPI_File_read_all( fh, read_buffer,
				                   read_buffer_size, MPI_INT,
				                   &status );
				MPI_Get_count( &status, MPI_INT, &count );

				if( verbose )
					printf( "%3d: read %d integers\n",
					        my_rank, count );

				MPI_File_close( &fh );

				/* Now check that the integers read are the same as the ones
				we wrote. */

				for( i = 0; i < read_buffer_size; i++ ) {
					if( *( write_buffer + i ) !=
					    *( read_buffer + i ) ) {
						printf( "%3d: data read \
							different from data \
							written, i = %d\n",
						        my_rank, i );
						my_read_error = TRUE;
					}
				}

				MPI_Reduce( &my_read_error, &read_error, 1,
				            MPI_INT, MPI_LOR, MASTER_RANK,
				            MPI_COMM_WORLD );

				if( i_am_the_master )
					if( !read_error )
						printf( "--> All data read back \
						       is correct.\n" );

			} /* no problem with file write */

		} /* no problem with file open */

	} /* no input error */

	MPI_Finalize();
	exit( 0 );

}