/**
 * @file    siox-mockupfs-example.c
 *          Beispiel für die Nutzung des SIOX-Low-Level-Interfaces mit einem fiktiven Dateisystem,
 *          welches ebenfalls für SIOX intrumentiert wurde.
 *
 * @authors Julian Kunkel & Michaela Zimmer
 * @date    2012
 *          GNU Public License.
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <string.h>

#include "siox-ll.h"
#include "mufs.h"


/**
 * Using SIOX
 *
 * An example of a client utilising a mock-up file system.
 *
 * @return  EXIT_FAILURE, falls ein Fehler auftrat, sonst EXIT_SUCCESS.
 */
int
main(){

    /* Vars for SIOX */
    siox_unid       unid;           /* This node's UNID */
    siox_dtid       dtid_fn;        /* The DTID of the data type "FileName" */
    siox_dtid       dtid_b2w;       /* The DTID of the data type "Bytes to write" */
    siox_aid        aid;            /* An AID for the write activity */
    unsigned long   bytes_written;  /* A collector for performance data */
    unsigned long   bytes_to_write; /* A collector for a call attribute */
    siox_mid        mid;            /* The MID for our performance metric */

    /* Variables for handling our HWID, SWID and IID and their string representations */
    pid_t   pid;    /* Our process ID */
    pid_t   tid;    /* Our thread ID */
    char    hwid_s[ HOST_NAME_MAX ];
    char*   swid_s = "SIOX-MockUpFS-Example";
    char    iid_s[ 20 ];

    /* Variables for accessing MUFS */
    char*       mufs_file_name = "EECummings.mufs";
    char        data[] =    "as freedom is a breakfastfood\n"
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
                            "–time is a tree (this life one leaf)\n"
                            "but love is the sky and i am for you\n"
                            "just so long and long enough\n";



    /*
     * Preparations
     * ============
     */

    /* Find own HWID, SWID and IID and turn them into strings */
    pid = getpid();
    tid = syscall( SYS_gettid );
    gethostname( (char *) &hwid_s, (size_t) HOST_NAME_MAX );
    sprintf( (char *) &iid_s, "%d-%d", (int) pid, (int) tid );


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
    unid = siox_register_node( hwid_s, swid_s, iid_s );

    /* Register the descriptor type we know */
    dtid_fn = siox_register_datatype( "FileName", SIOX_STORAGE_STRING );
    dtid_b2w = siox_register_datatype( "Bytes to write", SIOX_STORAGE_64_BIT_INTEGER );

    /* Register our performance metric with the ontology */
    mid = siox_register_metric( "Bytes Written",
                                SIOX_UNIT_BYTES,
                                SIOX_STORAGE_64_BIT_INTEGER,
                                SIOX_SCOPE_ACTUAL);


    /*
     * Writing via MUFS
     * ================
     */

    /* Notify SIOX that we are starting an activity for which we may later collect performance data. */
    aid = siox_start_activity( unid, NULL, "Write via MUFS" );

    /* Report the imminent (possibly remote) call (and its attributes) of a node with SWID "MUFS".
       Here, we use the attributes filename and data volume to characterise the call.  */
    siox_rcid rcid = siox_describe_remote_call_start( aid, NULL, "MUFS", NULL );
    siox_remote_call_attribute( rcid, dtid_fn, &mufs_file_name);
    bytes_to_write = strlen( data );
    siox_remote_call_attribute( rcid, dtid_b2w, &bytes_to_write );
    siox_describe_remote_call_end( rcid );

    /* The actual call to MUFS to create a file with the given name and write the character data to it */
    bytes_written = mufs_putfile( mufs_file_name, data );
    if ( bytes_written != (unsigned long) strlen( data ) )
    {
        fprintf( stderr, "!!! Error writing via MUFS! !!!" );
        siox_end_activity( aid, NULL );
        siox_unregister_node( unid );
        exit( EXIT_FAILURE );
    }

    /* Stop the writing activity; this will stop the clock running on the activity without closing it yet. */
    siox_stop_activity( aid, NULL );


    /*
     * Collecting and Reporting Performance Data
     * =========================================
     */

    /* Report the data we collected. This could take place anytime between siox_start_activity()
       and siox_end_activity() and happen more than once per activity.  */
    siox_report_activity( aid, mid, &bytes_written );


    /*
     * Cleaning up
     * ===========
     */

    /* Notify SIOX that all pertinent data has been sent and the activity can be closed */
    siox_end_activity( aid, NULL );

    /* Unregister node from SIOX */
    siox_unregister_node( unid );


    exit( EXIT_SUCCESS );
}

