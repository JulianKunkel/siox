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
#define PACKETSIZE 100


int
mufs_putfile(const char * filename, const char * contents )
{
	/*
	 * Open or, if necessary, create file via POSIX
	 */
	 
	/*
	 * Write contents in packets of PACKETSIZE characters each via POSIX
	 */
	 
	/*
	 * Close file via POSIX
	 */
	 

	/* Return number of bytes successfully written */
	return ( strlen( contents ) );
}

