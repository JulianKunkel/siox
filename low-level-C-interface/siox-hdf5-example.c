/**
 * @file siox-hdf5-example.c
 *
 * Beispiel für die Nutzung des SIOX-Low-Level-Interfaces mit HDF5.
 * Es demonstriert, wie ein Client für SIOX instrumentiert wird.
 *
 * @authors	Julian Kunkel & Michaela Zimmer
 * @date	2011
 * 			GNU Public License.
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#include <hdf5.h>
#include <hdf5_hl.h>

#include "siox-ll.h"
#include "../ontology/ontology.h"


/**
 * Using SIOX at Client Level
 *
 * An example of a client utilising SIOX with the HDF5 High Level Interface.
 *
 * @return	EXIT_FAILURE, falls ein Fehler auftrat, sonst EXIT_SUCCESS.
 */
int
main(){

	/* Vars for SIOX */
	siox_unid	unid;			/* This node's UNID */
	siox_dmid	dmid;			/* A DMID for a descriptor mapping this node can perform */
	siox_aid	aid_write;		/* An AID for the write activity only */
	siox_aid	aid_all;		/* Another AID for an activity encompassing the whole file access */
	char		ontology[] = "siox.ont";	/* The SIOX ontology file */
	siox_mid	mid;			/* An MID for the performance metric we want to measure */
	int			bytes_written;	/* A collector for performance data */

	/* Vars for handling our PID */
	pid_t		pid;
	char		pid_s[10];		/* A printable representation of pid */

	/* Vars for accessing HDF5 */
	char		hdf5_file_name[] = "Datei.h5";
    hid_t		hdf5_file_id;
    char		hdf5_file_id_s[10];	/* A printable representation of hdf5_file_id */
	hsize_t		dims[2]={2,3};
	int			data[6]={1,2,3,4,5,6};
	herr_t		hdf5_status;



	/*
	 * Info Message
	 * ============
	 */
	 printf( "SIOX HDF5 Example\n" );
	 printf( "=================\n\n" );


	/*
	 * Preparations
	 * ============
	 */

	/* Find own PID and turn it into a string */
	pid = getpid();
	sprintf( pid_s, "%d", pid );


	/*
	 * Checking in with SIOX
	 * =====================
	 */

	/* Register node itself */
	unid = siox_register_node( "Michaelas T1500", "SIOX-HDF5-Example", pid_s );

	/* Register ability to map HDF5-FileName to HDF5-FileId */
	dmid = siox_register_descriptor_map( unid, "FileName", "HDF5-FileId" );

	/* Register link to child node "HDF5" */
	siox_register_edge( unid, "HDF5" );


	/* Open ontology */
	siox_ont_open_ontology( ontology );

	/* Find MID for our performance metric, if it exists... */
	if( (mid = siox_ont_find_mid_by_name( "Bytes Written" )) )
	{
		printf( "Found performance metric %s in ontotology.\n",
				siox_ont_metric_get_name( siox_ont_find_metric_by_mid( mid ) ) );
	}
	else
	{
		/* ...otherwise, register our performance metric with the ontology */
		mid = siox_ont_register_metric( "Bytes Written",
										"",
										SIOX_UNIT_BYTES,
										SIOX_STORAGE_64_BIT_INTEGER,
										SIOX_SCOPE_SUM );
		printf( "Registered performance metric %s with ontotology.\n",
				siox_ont_metric_get_name( siox_ont_find_metric_by_mid( mid ) ) );
		siox_ont_write_ontology();
	}


	/*
	 * Opening a File for Writing
	 * ==========================
	 */

	/* Notify SIOX that we are starting an activity for which we may later collect performance data.
	   To compare, we start one activity for the whole file access and one for the writing part only. */
	aid_all = siox_start_activity( unid, "Write whole file" );

	/* Report creation of a new descriptor - the file name */
	siox_create_descriptor( unid, "FileName", hdf5_file_name );

	/* Report the imminent transfer of the new descriptor to a node with SWID "HDF5" */
	siox_send_descriptor( unid, "HFD5", "FileName", hdf5_file_name);

	/* The actual call to HDF5 to create a file with the name "Datei.h5", returning a HDF5 file id */
	hdf5_file_id = H5Fcreate ( hdf5_file_name, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT );

 	/* Report the mapping of file name to file id,
 	   as we cannot know whether HDF5 is instrumented for and reporting to SIOX */
 	sprintf( hdf5_file_id_s, "%d", (int) hdf5_file_id ); /* Turn foreign data type into string */
	siox_map_descriptor( unid, dmid, hdf5_file_name, hdf5_file_id_s );


	/*
	 * Writing into the File
	 * =====================
	 */

	/* Start another activity which will cover only the writing part */
	aid_write = siox_start_activity( unid, "Write data" );

	/* Once again, a descriptor will be handed over; inform SIOX */
	siox_send_descriptor( unid, "HDF5", "HDF5-FileId", hdf5_file_id_s );

	/* The actual call to create the file and write the dataset to it */
	hdf5_status = H5LTmake_dataset( hdf5_file_id, "/dset", 2, dims, H5T_NATIVE_INT, data );

	/* Stop the writing activity */
	siox_stop_activity( aid_write );


	/*
	 * Closing the File
	 * ================
	 */

	/* The actual call to close the file */
	hdf5_status = H5Fclose( hdf5_file_id );
	if( hdf5_status < 0 )
		fprintf(stderr, "!!! Fehler beim Schreiben über HDF5! !!!\n");

	/* After closing the file, we will not need the file id descriptor anymore */
	siox_release_descriptor( unid, "HDF5-FileId", hdf5_file_id_s );

	/* Now stop the activity covering the whole file access, too */
	siox_stop_activity( aid_all );


	/*
	 * Collecting and Reporting Performance Data
	 * =========================================
	 */

	/* Collect data */
	bytes_written = sizeof( data );

	/* Report the data we collected. This could take place anytime between siox_start_activity()
	   and siox_end_activity() and happen more than once per activity.
	   As one is mapped to the other, we could attribute the data to either the file name or
	   the file id descriptor; we'll do one each  */
	siox_report_activity( aid_write,
						  "HDF5-FileId", hdf5_file_id_s,
						  mid, SIOX_TYPE_INTEGER, &bytes_written,
						  NULL );
	siox_report_activity( aid_all,
						  "HDF5-FileName", hdf5_file_name,
						  mid, SIOX_TYPE_INTEGER, &bytes_written,
						  "Including opening & closing the file." );


	/* Notify SIOX that all pertinent data has been sent and the activities can be closed */
	siox_end_activity( aid_write );
	siox_end_activity( aid_all );


	/*
	 * Cleaning up
	 * ===========
	 */

	/* Mark any descriptors left as unused */
	siox_release_descriptor( unid, "FileName", hdf5_file_name );

	/* Unregister node from SIOX */
	siox_unregister_node( unid );

	/* Closing SIOX ontology */
	siox_ont_close_ontology();


	exit( EXIT_SUCCESS );
}

