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
/*#include <limits.h>*/

#include <hdf5.h>
#include <hdf5_hl.h>

#include "siox-ll.h"

/*#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 64
#endif*/

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
    /*siox_dtid   dtid_h5fn;*/      /* A DTID for HDF5 file names */
    /*siox_dtid   dtid_h5id;*/      /* A DTID for HDF5 file ids */
    siox_aid    aid_write;      /* An AID for the write activity only */
    siox_aid    aid_all;        /* Another AID for an activity encompassing the whole file access */
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
    /*dtid_h5fn = siox_register_datatype( "HDF5-FileName", SIOX_STORAGE_STRING );*/
    /*dtid_h5id = siox_register_datatype( "HDF5-FileId", SIOX_STORAGE_64_BIT_INTEGER );*/

    /* Indicate which data types serve as descriptors */
    /*siox_register_datatype_as_descriptor( dtid_h5id );*/


    /* Register our performance metric with the ontology */
    mid = siox_register_metric( "Bytes Written",
                                SIOX_UNIT_BYTES,
                                SIOX_STORAGE_64_BIT_INTEGER,
                                SIOX_SCOPE_SUM );
    printf( "Registered performance metric >Bytes Written< with ontology.\n" );


    /*
     * Opening a File for Writing
     * ==========================
     */

    /* Notify SIOX that we are starting an activity for which we may later collect performance data.
       To compare, we start one activity for the whole file access and one for the writing part only. */
    aid_all = siox_start_activity( unid, NULL, "Write whole file" );

    /* The actual call to HDF5 to create a file with the name "Datei.h5", returning an HDF5 file id */
    hdf5_file_id = H5Fcreate ( hdf5_file_name, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT );



    /*
     * Writing into the File
     * =====================
     */

    /* Start another activity which will cover only the writing part */
    aid_write = siox_start_activity( unid, NULL, "Write data" );

    /* The actual call to create the file and write the dataset to it */
    hdf5_status = H5LTmake_dataset( hdf5_file_id, "/dset", 2, dims, H5T_NATIVE_INT, data );

    /* Stop the writing activity */
    siox_stop_activity( aid_write, NULL );


    /*
     * Closing the File
     * ================
     */

    /* The actual call to close the file */
    hdf5_status = H5Fclose( hdf5_file_id );
    if( hdf5_status < 0 )
        fprintf(stderr, "!!! Fehler beim Schreiben über HDF5! !!!\n");

    /* Now stop the activity covering the whole file access, too */
    siox_stop_activity( aid_all, NULL );


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
                          mid, &bytes_written );
    siox_report_activity( aid_all,
                          mid, &bytes_written );


    /* Notify SIOX that all pertinent data has been sent and the activities can be closed */
    siox_end_activity( aid_write, NULL );
    siox_end_activity( aid_all, NULL );


    /*
     * Cleaning up
     * ===========
     */

    /* Unregister node from SIOX */
    siox_unregister_node( unid );


    exit( EXIT_SUCCESS );
}

