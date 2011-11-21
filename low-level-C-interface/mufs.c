/**
 * @file	mufs.c
 * 			Implementation des Mock-Up-File-System.
 *
 * @authors	Julian Kunkel & Michaela Zimmer
 * @date	2011
 * 			GNU Public License.
 */


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "mufs.h"


/** Size of packets to be processed with each POSIX call */
#define PACKET_SIZE 100


int
mufs_putfile(const char * filename, const char * contents )
{
	FILE	*fp;
	int		bytes_to_write;
	int		bytes_written = 0;
	
	
	/* Open or, if necessary, create file via POSIX */
	fp = fopen( filename, "wt" );
	 
	 
	/*
	 * Write contents in packets of PACKETSIZE characters each via POSIX
	 */
	for( bytes_to_write = strlen( contents );
			bytes_written < bytes_to_write;
				bytes_to_write -= PACKET_SIZE )
	{
		/* fwrite(); */
		/** todo: CONTINUE HERE!!! */
	}
	
	 
	/* Close file via POSIX */
	if( fclose( fp ) == EOF )
	{
		fprintf( stderr, "Fehler beim Schließen der Datei %s!\n", filename );
		exit( EXIT_FAILURE );
	}
	 

	/* Return number of bytes successfully written */
	return ( bytes_written );
}

