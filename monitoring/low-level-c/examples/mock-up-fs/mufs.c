/**
 * @file    mufs.c
 *          Implementation of the Mock-Up File System.
 *
 * @authors Julian Kunkel & Michaela Zimmer
 * @date    2013-08-06
 *          GNU Public License.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>

#include <C/siox-ll.h>

#include "mufs.h"


/** Block size for the data to be written */
#define BLOCK_SIZE 400


/** Flag indicating whether we have called mufs_initialise() yet. */
static int initialised = 0;

/*
 * Variables for SIOX
 */
/** The library's @em UNID */
//static siox_unid    unid;
/* The @em DTIDs of the data types known to MUFS */
//static siox_dtid    dtid_fn;   /**< The @em DTID for file names */
//static siox_dtid    dtid_fp;   /**< The @em DTID for file pointers */
//static siox_dtid    dtid_b2w;  /**< The @em DTID for the amount of bytes to write */
/** The @em MID for our performance metric */
// static siox_mid     mid;


/**
 * Initialise the library.
 * Right now, only used to check in with SIOX.
 */
static void mufs_initialise();


unsigned long
mufs_putfile( const char * filename, const char * contents )
{
    FILE *          fp;
    long int        bytes_to_write;
    long int        total_bytes_to_write;
    long int        total_bytes_written;
    int             block;
    const char *    pData;
    const char *    pEnd;
    
    char            sBuffer[200];

    /* Variables for SIOX */
    //siox_aid    aid;        /** @em AID for an activity describing this whole function */




    /*
     * If not done yet, initialise MUFS
     */
    if( !initialised )
        mufs_initialise();


    /*
     * SIOX - preliminary negotiations
     */
    sprintf( sBuffer, "Write data to file %s via MUFS.", filename );
    //aid = siox_start_activity( unid, NULL, sBuffer );

    /* Report the identifying attributes of the call received */
    //siox_remote_call_receive( aid, dtid_fn, &filename);
    /* Another characteristic of the call, though not explicitly given as a parameter:
       The amount of data to write. */
    total_bytes_to_write = 0;
    for( pEnd = contents; *pEnd != 0; pEnd++)
        total_bytes_to_write++;
    //siox_remote_call_receive( aid, dtid_b2w, &total_bytes_to_write );


    /*
     * Open or, if necessary, create file via POSIX
     */
    fp = fopen( filename, "wt" );


    /*
     * Write contents in blocks of BLOCK_SIZE characters each via POSIX
     */
    total_bytes_written = 0;

    for( pData = contents,
         block = 0;
            pData < pEnd;
                pData += bytes_to_write,
                total_bytes_written += bytes_to_write,
                block++ )
    {
        bytes_to_write = pEnd - pData;
        if( bytes_to_write > BLOCK_SIZE )
            bytes_to_write = BLOCK_SIZE;

        if( fwrite( pData, bytes_to_write, 1, fp ) != 1 )
            {
                //siox_end_activity( aid, NULL );

                fprintf( stderr, "!!! Error writing block %d of file >%s<! !!!\n",
                         block, filename );
                return( 0 );
            }
    }


    /*
     * Close file via POSIX
     */
    if( fclose( fp ) == EOF )
    {
        //siox_end_activity( aid, NULL );

        fprintf( stderr, "!!! Error closing file >%s<! !!!\n", filename );
        return( 0 );
    }

    /* Now all the work is done, stop the activity's clock and move on to the reporting phase. */
    //siox_stop_activity( aid, NULL );


    /*
     * SIOX - final negotiations
     */
    /* Report our performance metrics for this activity. */
    //siox_report_activity( aid, mid, &total_bytes_written );
    //siox_end_activity( aid, NULL );


    /* Return number of bytes successfully written */
    return ( total_bytes_written );
}




static void
mufs_initialise()
{
    /* Variables for handling our HWID, SWID and IID and their string representations */
    char*   swid_s = "MUFS";


    /* Flag status as initialised */
    initialised++;

    /*
     * Info Message
     * ============
     */
     printf( "\n"
             "=================================\n"
             "= MUFS, the Mock-Up File System =\n"
             "=================================\n"
             "==== Simplicity & Efficiency ====\n"
             "=================================\n"
             "\n" );

    /*
     * Checking in with SIOX
     * =====================
     */

    //unid = siox_register_node( hwid_s, swid_s, iid_s );

    /* Register descriptor types we know */
    //dtid_fn = siox_register_datatype( "FileName", SIOX_STORAGE_STRING );
    //dtid_fp = siox_register_datatype( "MUFS-FilePointer", SIOX_STORAGE_64_BIT_INTEGER );
    //dtid_b2w = siox_register_datatype( "Bytes to write", SIOX_STORAGE_64_BIT_INTEGER );

    /* Register our performance metric with the ontology */
/*    mid = siox_register_metric( "Bytes Written",
                                SIOX_UNIT_BYTES,
                                SIOX_STORAGE_64_BIT_INTEGER,
                                SIOX_SCOPE_ACTUAL);
*/}

