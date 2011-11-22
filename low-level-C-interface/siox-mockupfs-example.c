/**
 * @file 	siox-mockupfs-example.c
 * 			Beispiel für die Nutzung des SIOX-Low-Level-Interfaces mit einem fiktiven Dateisystem,
 * 			welches ebenfalls für SIOX intrumentiert wurde.
 *
 * @authors	Julian Kunkel & Michaela Zimmer
 * @date	2011
 * 			GNU Public License.
 */
 
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include "siox-ll.h"
#include "mufs.h"


/**
 * Using SIOX
 *
 * An example of a client utilising a mock-up file system.
 *
 * @return	EXIT_FAILURE, falls ein Fehler auftrat, sonst EXIT_SUCCESS.
 */
int
main(){

	/* Vars for SIOX */
	siox_unid	unid;			/* This node's UNID */
	siox_aid	aid_write;		/* An AID for the write activity */
	int			bytes_written;	/* A collector for performance data */
	
	/* Vars for handling our PID */
	pid_t		pid;
	char		pid_s[10];		/* A printable representation of pid */
	
	/* Vars for accessing MUFS */
	char		mufs_file_name[] = "EECummings.mufs";
	char		data[] =	"as freedom is a breakfastfood\n"
							"or truth can live with right and wrong\n"
							"or molehills are from mountains made\n"
							"–long enough and just so long\n"
							"will being pay the rent of seem\n"
							"and genius please the talentgang\n"
							"and water most encourage flame\n\n"
							"as hatracks into peachtrees grow\n"
							"or hopes dance best on bald men’s hair\n"
							"and every finger is a toe\n"
							"and any courage is a fear\n"
							"–long enough and just so long\n"
							"will the impure think all things pure\n"
							"and hornets wail by children stung\n\n"
							"or as the seeing are the blind\n"
							"and robins never welcome spring\n"
							"nor flatfolk prove their world is round\n"
							"nor dingsters die at break of dong\n"
							"and common’s rare and millstones float\n"
							"–long enough and just so long\n"
							"tomorrow will not be too late\n\n"
							"worms are the words but joy’s the voice\n"
							"down shall go which and up come who\n"
							"breasts will be breasts and thighs will be thighs\n"
							"deeds cannot dream what dreams can do\n"
							"–time is a tree(this life one leaf)\n"
							"but love is the sky and i am for you\n"
							"just so long and long enough\n";
							

	
	/*
	 * Preparations
	 * ============
	 */
	 	
	/* Find own PID and turn it into a string */
	pid = getpid();
	sprintf( pid_s, "%d", pid );
	
	
	/*
	 * Info Message
	 * ============
	 */
	printf( "\n"
			"SIOX Mock-Up File System Example\n"
			"================================\n"
			"\n" );


	/*
	 * Checking in with SIOX
	 * =====================
	 */
	
	/* Register node itself */
	unid = siox_register_node( "Michaelas T1500", "SIOX-MockUpFS-Example", pid_s );
	
	/* Register link to child node "MUFS" */
	siox_register_edge( unid, "MUFS" );
	
	
	/*
	 * Writing via MUFS
	 * ================
	 */
	 
	/* Notify SIOX that we are starting an activity for which we may later collect performance data. */
	aid_write = siox_start_activity( unid, "Write via MUFS" );
	 
	/* Report creation of a new descriptor - the file name */
	siox_create_descriptor( unid, "FileName", mufs_file_name );

	/* Report the imminent transfer of the new descriptor to a node with SWID "MUFS" */
	siox_send_descriptor( unid, "MUFS", "FileName", mufs_file_name);


	/* The actual call to MUFS to create a file with the given name and write the character data to it */
	bytes_written = mufs_putfile( mufs_file_name, data );
	if ( bytes_written != strlen( data ) )
	{
		fprintf( stderr, "!!! Fehler beim Schreiben über MUFS! !!!" );
		exit( EXIT_FAILURE );
	}
	
	/* Stop the writing activity */
	siox_stop_activity( aid_write );


	/*
	 * Collecting and Reporting Performance Data
	 * =========================================
	 */
	 
	/* Report the data we collected. This could take place anytime between siox_start_activity()
	   and siox_end_activity() and happen more than once per activity.  */
	siox_report_activity( aid_write,
						  "FileName", mufs_file_name,
						  "Bytes Written", SIOX_TYPE_INTEGER, &bytes_written,
						  "Including opening & closing the file, as usual with MUFS." );
	 
	/* Notify SIOX that all pertinent data has been sent and the activities can be closed */
	siox_end_activity( aid_write );
	
	 
	/*
	 * Cleaning up
	 * ===========
	 */
	 
	/* Mark any descriptors left as unused */
	siox_release_descriptor( unid, "FileName", mufs_file_name );
	
	/* Unregister node from SIOX */
	siox_unregister_node( unid );
	
	
	exit( EXIT_SUCCESS );
}

