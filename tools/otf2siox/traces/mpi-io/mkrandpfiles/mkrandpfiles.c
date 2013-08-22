/*
 * %Id: mkrandpfiles.c,v 1.2 2003/10/19 19:20:14 gustav Exp %
 *
 * %Log: mkrandpfiles.c,v %
 * Revision 1.2  2003/10/19 19:20:14  gustav
 * Indented the program with Emacs.
 *
 * Revision 1.1  2003/10/19 18:51:41  gustav
 * Initial revision
 *
 * mpiexec -n 8 mkrandpfiles -f test -l 8
 */

#include <stdio.h>   /* all IO stuff lives here */
#include <stdlib.h>  /* exit lives here */
#include <unistd.h>  /* getopt lives here */
#include <errno.h>   /* UNIX error handling lives here */
#include <string.h>  /* strcpy lives here */
#include <mpi.h>     /* MPI and MPI-IO live here */

#define MASTER_RANK 0
#define TRUE 1
#define FALSE 0
#define BOOLEAN int
#define BLOCK_SIZE 1048576
#define MBYTE 1048576
#define SYNOPSIS printf ("synopsis: %s -f <file> -l <blocks>\n", argv[0])

int main( argc, argv )
int argc;
char * argv[];
{
	/* my variables */

	int my_rank, pool_size, number_of_blocks = 0, i;
	int number_of_integers, number_of_bytes;
	long long total_number_of_integers, total_number_of_bytes;
	BOOLEAN i_am_the_master = FALSE, input_error = FALSE,
	        my_file_open_error = FALSE, file_open_error = FALSE,
	        my_write_error = FALSE, write_error = FALSE;
	char * basename = NULL, file_name[BUFSIZ], message[BUFSIZ];
	int basename_length, *junk;
	MPI_File fh;
	double start, finish, io_time, longest_io_time;
	char error_string[BUFSIZ];
	int length_of_error_string, error_class;
	MPI_Status  status;

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

		/* read the command line */

		while( ( c = getopt( argc, argv, "f:l:h" ) ) != EOF )
			switch( c ) {
				case 'f':
					basename = optarg;
					break;
				case 'l':
					if( ( sscanf( optarg, "%d", &number_of_blocks )
					      != 1 ) || ( number_of_blocks < 1 ) )
						input_error = TRUE;
					break;
				case 'h':
					input_error = TRUE;
					break;
				case '?':
					input_error = TRUE;
					break;
			}

		/* Check if the command line has initialized both the basename and
		* the number_of_blocks.
		*/

		if( ( basename == NULL ) || ( number_of_blocks == 0 ) )
			input_error = TRUE;

		if( input_error )
			SYNOPSIS;
		else {
			basename_length = strlen( basename ) + 1;
#ifdef DEBUG
			printf( "basename         = %s\n", basename );
			printf( "basename_length  = %d\n", basename_length );
			printf( "number_of_blocks = %d\n", number_of_blocks );
#endif
		}

	} /* end of if(i_am_the_master) { <read the command line> } */

	/* Transmit the effect of reading the command line to other
	processes. */

	MPI_Bcast( &input_error, 1, MPI_INT, MASTER_RANK, MPI_COMM_WORLD );

	if( !input_error ) {

		/* If we managed to get here, data read from the command line
		is probably OK. */

		MPI_Bcast( &number_of_blocks, 1, MPI_INT, MASTER_RANK,
		           MPI_COMM_WORLD );
		MPI_Bcast( &basename_length, 1, MPI_INT, MASTER_RANK,
		           MPI_COMM_WORLD );

		if( !i_am_the_master )
			basename = ( char * ) malloc( basename_length );

		MPI_Bcast( basename, basename_length, MPI_CHAR, MASTER_RANK,
		           MPI_COMM_WORLD );

#ifdef DEBUG
		printf( "%3d: basename = %s, number_of_blocks = %d\n",
		        my_rank, basename, number_of_blocks );
#endif

		/* Allocate space needed to generate the integers */

		number_of_integers = number_of_blocks * BLOCK_SIZE;
		number_of_bytes = sizeof( int ) * number_of_integers;
		total_number_of_integers = ( long long ) number_of_integers
		                           * ( long long ) pool_size;
		total_number_of_bytes = ( long long ) number_of_bytes
		                        * ( long long ) pool_size;
		junk = ( int * ) malloc( number_of_bytes );

		/* Now every process creates its own file name and attempts
		to open the file. */

		sprintf( file_name, "%s.%d", basename, my_rank );

#ifdef DEBUG
		printf( "%3d: opening file %s\n", my_rank, file_name );
#endif

		my_file_open_error = MPI_File_open( MPI_COMM_SELF, file_name,
		                                    MPI_MODE_CREATE |
		                                    MPI_MODE_WRONLY,
		                                    MPI_INFO_NULL, &fh );

		if( my_file_open_error != MPI_SUCCESS ) {

			MPI_Error_class( my_file_open_error, &error_class );
			MPI_Error_string( error_class, error_string,
			                  &length_of_error_string );
			printf( "%3d: %s\n", my_rank, error_string );

			MPI_Error_string( my_file_open_error, error_string,
			                  &length_of_error_string );
			printf( "%3d: %s\n", my_rank, error_string );

			my_file_open_error = TRUE;

		}

		/* Now we must ALL check that NOBODY had problems
		with opening the file. */

		MPI_Allreduce( &my_file_open_error, &file_open_error, 1, MPI_INT,
		               MPI_LOR, MPI_COMM_WORLD );

#ifdef DEBUG
		if( i_am_the_master )
			if( file_open_error )
				fprintf( stderr, "problem opening output \
					files\n" );
#endif

		/* If all files are open for writing, write to them */

		if( !file_open_error ) {
			srand( 28 + my_rank );

			for( i = 0; i < number_of_integers; i++ )
				*( junk + i ) = rand();

			start = MPI_Wtime();
			my_write_error = MPI_File_write( fh, junk,
			                                 number_of_integers,
			                                 MPI_INT, &status );

			if( my_write_error != MPI_SUCCESS ) {

				MPI_Error_class( my_write_error, &error_class );
				MPI_Error_string( error_class, error_string,
				                  &length_of_error_string );
				printf( "%3d: %s\n", my_rank, error_string );
				MPI_Error_string( my_write_error, error_string,
				                  &length_of_error_string );
				printf( "%3d: %s\n", my_rank, error_string );
				my_write_error = TRUE;

			} else {
				finish = MPI_Wtime();
				io_time = finish - start;
				printf( "%3d: io_time = %f\n", my_rank, io_time );
			}

			/* Check if anybody had problems writing on the file */

			MPI_Allreduce( &my_write_error, &write_error, 1,
			               MPI_INT, MPI_LOR, MPI_COMM_WORLD );

#ifdef DEBUG
			if( i_am_the_master )

				if( write_error )
					fprintf( stderr, "problem writing on \
						files\n" );
#endif

		} /* of if(! file_open_error) { <write on the file> } */

		/* Only processes that were successful opening the files
		need do close them here */

		if( !my_file_open_error ) {
			MPI_File_close( &fh );
#ifdef DEBUG
			printf( "%3d: closed %s\n", my_rank, file_name );
#endif
		}

		/* If we have either write errors or file open errors,
		then processes that managed to open their files
		are requested to throw them away */

		if( write_error || file_open_error ) {
			if( !my_file_open_error ) {
				MPI_File_delete( file_name, MPI_INFO_NULL );
#ifdef DEBUG
				printf( "%3d: deleted %s\n", my_rank, file_name );
#endif
			}

		} else {

			MPI_Reduce( &io_time, &longest_io_time, 1, MPI_DOUBLE,
			            MPI_MAX, MASTER_RANK, MPI_COMM_WORLD );

			if( i_am_the_master ) {
				printf( "longest_io_time       = %f seconds\n",
				        longest_io_time );
				printf( "total_number_of_bytes = %lld\n",
				        total_number_of_bytes );
				printf( "transfer rate         = %f MB/s\n",
				        total_number_of_bytes / longest_io_time
				        / MBYTE );
			}

		} /* end of if (write_error || file_open_error) {<delete open files>} */

	} /* end of if(! input_error) { <open the file and write on it> } */

	MPI_Finalize();
	exit( 0 );

}

