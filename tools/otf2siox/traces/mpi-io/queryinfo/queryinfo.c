/*
 * %Id: queryinfo.c,v 1.1 2003/10/27 18:19:10 gustav Exp %
 *
 * %Log: queryinfo.c,v %
 * Revision 1.1  2003/10/27 18:19:10  gustav
 * Initial revision
 *
 * mpiexec -n 4 queryinfo -f test -v
 */

#include <stdio.h>     /* printf and relatives live here */
#include <stdlib.h>    /* exit lives here */
#include <unistd.h>    /* getopt lives here */
#include <string.h>    /* strlen lives here */
#include <mpi.h>       /* all MPI stuff lives here (including MPI-IO) */

#define MASTER_RANK 0
#define TRUE 1
#define FALSE 0
#define BOOLEAN int
#define SYNOPSIS printf ("synopsis: %s -f <file> [-v] [-h]\n", argv[0])

int main(argc, argv)
     int argc;
     char *argv[];
{
  /* my variables */

	int my_rank, pool_size, i, file_name_length;
	BOOLEAN i_am_the_master = FALSE, input_error = FALSE, verbose = FALSE;
	char *file_name = NULL;

	/* MPI variables */

	MPI_File fh;
	MPI_Info info_used;
	int nkeys;
	BOOLEAN exists;
	char key[MPI_MAX_INFO_KEY], value[MPI_MAX_INFO_VAL];

	/* getopt variables */

	extern char *optarg;
	int c;

	/* ACTION */

	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &pool_size);
	
	if (my_rank == MASTER_RANK) 
		i_am_the_master = TRUE;
	
	if (i_am_the_master) {
		
		while ((c = getopt(argc, argv, "f:vh")) != EOF)
			
			switch(c) {
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
			
		if (file_name == NULL) 
			input_error = TRUE;
		
		if (input_error)
			SYNOPSIS;
		else {
			file_name_length = strlen(file_name) + 1;
			
			if (verbose) {
				printf("file_name         = %s\n", file_name);
				printf("file_name_length  = %d\n", 
				       file_name_length);
			}
		}
		
	} 

	MPI_Bcast(&input_error, 1, MPI_INT, MASTER_RANK, MPI_COMM_WORLD);

	if (!input_error) {
		
		MPI_Bcast(&verbose, 1, MPI_INT, MASTER_RANK, MPI_COMM_WORLD);
		MPI_Bcast(&file_name_length, 1, MPI_INT, MASTER_RANK, 
			  MPI_COMM_WORLD);
		
		if (!i_am_the_master) 
			file_name = (char*) malloc(file_name_length);
		
		MPI_Bcast(file_name, file_name_length, MPI_CHAR, MASTER_RANK, 
			  MPI_COMM_WORLD);

		MPI_File_open(MPI_COMM_WORLD, file_name, MPI_MODE_CREATE | 
			      MPI_MODE_RDWR, MPI_INFO_NULL, &fh);
		MPI_File_get_info(fh, &info_used);
		MPI_Info_get_nkeys(info_used, &nkeys);

		for (i = 0; i < nkeys; i++) {
			
			MPI_Info_get_nthkey(info_used, i, key);
			MPI_Info_get(info_used, key, MPI_MAX_INFO_VAL, value, 
				     &exists);
			
			if (verbose)
				if (exists)
					printf("%3d: key = %s, value = %s\n", 
					       my_rank, key, value);
		}

		MPI_File_close(&fh);
		MPI_Info_free(&info_used);
		
	}

	MPI_Finalize();
	exit(0);
	
}