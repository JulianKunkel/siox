/**
 * @file siox-hdf5-example.c
 *
 * Beispiel für die Nutzung des SIOX-Low-Level-Interfaces mit HDF5.
 * Es demonstriert, wie ein Client für SIOX instrumentiert wird.
 *
 * @authors Julian Kunkel & Michaela Zimmer
 * @date    2012
 *          GNU Public License.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>

#include <hdf5.h>
#include <hdf5_hl.h>

#include "siox-ll.h"


/**
 * Using SIOX at Client Level
 *
 * An example of a client utilising SIOX with the HDF5 High Level Interface.
 *
 * @return  EXIT_FAILURE, falls ein Fehler auftrat, sonst EXIT_SUCCESS.
 */
int
main(){

    /* Vars for SIOX */
    siox_unid   unid;           /* This node's UNID */
    siox_dtid   dtid_h5fn;      /* A DTID for HDF5 file names */
    siox_dtid   dtid_h5id;      /* A DTID for HDF5 file ids */
    siox_dtid   dtid_b2write;   /* A DTID for the number of bytes to write in a call */
    siox_aid    aid_open;       /* An AID for the open activity */
    siox_aid    aid_write;      /* An AID for the write activity */
    siox_aid    aid_close;      /* An AID for the close activity */
    siox_mid    mid;            /* An MID for the performance metric we want to measure */
    long int    bytes_written;  /* A collector for performance data */

    /* Vars for accessing HDF5 */
    char*       hdf5_file_name = "Datei.h5";
    hid_t       hdf5_file_id;
    hsize_t     dims[2]={2,3};
    int         data[6]={1,2,3,4,5,6};
    herr_t      hdf5_status;



    /*
     * Info Message
     * ============
     */
     printf( "\n"
             "SIOX HDF5 Example\n"
             "=================\n"
             "\n" );


    /*
     * Preparations
     * ============
     */

    /* Find own HWID, SWID and IID and turn them into strings */
    char  hwid[HOST_NAME_MAX];
    char* swid = "SIOX-HDF5-Example";
    char  iid[20];

    pid_t   pid = getpid();
    pid_t   tid = syscall(SYS_gettid);
    gethostname((char *) &hwid, (size_t) HOST_NAME_MAX);
    sprintf((char*) &iid, "%d-%d", (int) pid, (int) tid);


    /*
     * Checking in with SIOX
     * =====================
     */

    /* Register node itself */
    unid = siox_register_node(hwid, swid, iid);

    /* Register data types we will use */
    dtid_h5fn = siox_register_datatype( "HDF5-FileName", SIOX_STORAGE_STRING );
    dtid_h5id = siox_register_datatype( "HDF5-FileId", SIOX_STORAGE_64_BIT_INTEGER );
    dtid_b2write = siox_register_datatype( "Bytes to write", SIOX_STORAGE_64_BIT_INTEGER );

    /* Indicate which data types serve as descriptors */
    siox_register_datatype_as_descriptor( dtid_h5fn );
    siox_register_datatype_as_descriptor( dtid_h5id );

    /* Register our performance metric with the ontology */
    mid = siox_register_metric( "Bytes Written",
                                SIOX_UNIT_BYTES,
                                SIOX_STORAGE_64_BIT_INTEGER,
                                SIOX_SCOPE_ACTUAL );



    /*
     * Opening a File for Writing
     * ==========================
     */

    /* Notify SIOX that we are starting an activity for which we may later collect performance data.
       For this example, we start one activity each for the open(), write() and close() calls.
       In place of the NULL argument, for all the activity functions, an artificial time stamp could
       be supplied to facilitate simulation runs; NULL will draw a current time stamp. */
    aid_open = siox_start_activity( unid, NULL, "Open HDF5 file" );

    /*
     * Prepare remote call parameters for SIOX
     * This serves to reconstruct the call raph and is only necessary for calls that may traverse
     * system boundaries, as local call hierarchies can easily be reconstructed using the system stack.
     */

    /* First, open a parameter block, supplying target HWID, SWID and IID (if known; otherwise NULL). */
    siox_rcid rcid_open = siox_describe_remote_call_start( aid_open, NULL, "HDF5", NULL );

    /* Next,  indicta ethe call's attributes that may serve to identify it on the callee's side
       (in this case: only the file name). */
    siox_remote_call_attribute( rcid_open, dtid_h5fn, &hdf5_file_name );

    /* Finally, close the block. */
    siox_describe_remote_call_end( rcid_open );

    /* The actual call to HDF5 to create a file with the name "Datei.h5", returning an HDF5 file id */
    hdf5_file_id = H5Fcreate ( hdf5_file_name, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT );

    /* Tell SIOX the activity has enden. As there are no performance metrics to report here, we
       can do without a call to siox_stop_activity() to separate active and reporting phases. */
    siox_end_activity( aid_open, NULL );



    /*
     * Writing into the File
     * =====================
     */

    /* Start another activity which will cover the writing part */
    aid_write = siox_start_activity( unid, NULL, "Write HDF5 file" );

    /* This time, we also inform SIOX that our current acivity is causally related to the one of
       the open() call, as we reuse the same file then prepared for us. */
    siox_link_activity( aid_write, aid_open );

    /* Once again, prepare the call's attributes for SIOX.
       This time, we also compute the total number of bytes to write, an identifying attribute of the
       call that is not directly passed as a parameter. */
    siox_rcid rcid_write = siox_describe_remote_call_start( aid_open, NULL, "HDF5", NULL );
    siox_remote_call_attribute( rcid_write, dtid_h5fn, &hdf5_file_name );
    long int b2write = sizeof( data );
    siox_remote_call_attribute( rcid_write, dtid_b2write, &b2write );
    siox_describe_remote_call_end( rcid_write );

    /* The actual call to create the file and write the dataset to it */
    hdf5_status = H5LTmake_dataset( hdf5_file_id, "/dset", 2, dims, H5T_NATIVE_INT, data );

    /* Stop the writing activity; this will mark the end of the active phase and begin reporting */
    siox_stop_activity( aid_write, NULL );

    /*
     * Collecting and Reporting Performance Data
     */

    /* Collect data */
    bytes_written = sizeof( data );

    /* Report the data we collected. This could take place anytime between siox_start_activity()
       and siox_end_activity() and happen more than once per activity. */
    siox_report_activity( aid_write,
                          mid, &bytes_written );

    /* Notify SIOX that all pertinent data has been sent and the write activity can be closed */
    siox_end_activity( aid_write, NULL );



    /*
     * Closing the File
     * ================
     */

    /* One more activity, this time for the close() call. Instead of being placed in one activity
       each, any or all calls could also be grouped together in activities, as program logic dictates.  */
    aid_close = siox_start_activity( unid, NULL, "Close HDF5 file" );

    /* The actual call to close the file */
    hdf5_status = H5Fclose( hdf5_file_id );
    if( hdf5_status < 0 )
        fprintf(stderr, "!!! Error writing via HDF5! !!!\n");

    /* Now close the activity covering the close(), too */
    siox_end_activity( aid_close, NULL );


    /*
     * Cleaning up
     * ===========
     */

    /* Unregister node from SIOX */
    siox_unregister_node( unid );


    exit( EXIT_SUCCESS );
}

