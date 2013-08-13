/**
 * @file    mufs.c
 *          Implementation of the Mock-Up File System.
 *
 * @authors Julian Kunkel & Michaela Zimmer
 * @date    2013-08-08
 *          GNU Public License.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <assert.h>

#include <C/siox-ll.h>

#include "mufs.h"


/** Block size for the data to be written */
#define BLOCK_SIZE 400


// Flag indicating whether we have called mufs_initialize() yet.
static int initialized = 0;

/*
 * Variables for SIOX
 */

// The unique interface identifier for the API we provide (MUFS, implementation OpenMUFS)
static siox_unique_interface       uiid = 0;
// The MUFS library's component identifier
static siox_component *             cid = NULL;

// An identifier for the component-specific activity type 'write'
static siox_component_activity *    act_type_write = NULL;

// Identifiers for the attributes of our activities
static siox_attribute *             att_blocksize = NULL;      // Block size
static siox_attribute *             att_filename = NULL;       // File name
static siox_attribute *             att_bytes2write = NULL;    // Bytes to write
static siox_attribute *             att_byteswritten = NULL;   // Bytes written



/**
 * Initialize the library.
 * Right now, only used to check in with SIOX.
 */
void mufs_initialize();


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
    int             status;
    
    char            sBuffer[200];

    // An identifier for the activity describing this whole function
    siox_activity *     act_write; 


    assert( initialized );
        


    /*
     * SIOX - preliminary negotiations
     */
    sprintf( sBuffer, "Write data to file %s via MUFS.", filename );
    // Start an activity
    act_write = siox_activity_start( act_type_write );

    // Report the remote origin of the activity and its identifying attributes
    // Sadly, we don't know anything about the caller
    siox_activity_started_by_remote_call( act_write, 0, 0, 0 );

    // The file name we are passed to write to
    siox_activity_set_attribute( act_write, att_filename, &filename );

    // Another characteristic of the call, though not explicitly given as a parameter:
    // The amount of data to write
    total_bytes_to_write = 0;
    for( pEnd = contents; *pEnd != 0; pEnd++)
        total_bytes_to_write++;
    siox_activity_set_attribute( act_write, att_bytes2write, &total_bytes_to_write );


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

        status = fwrite( pData, bytes_to_write, 1, fp );
        if( status != 1 )
            {
                // Inform SIOX of the activity's status and end it
                siox_activity_report_error( act_write, status );
                siox_activity_end( act_write );

                fprintf( stderr, "!!! Error writing block %d of file >%s<! !!!\n",
                         block, filename );
                return( 0 );
            }
    }


    /*
     * Close file via POSIX
     */
    status = fclose( fp );
    if( status == EOF )
    {
        // Inform SIOX of the activity's status and end it
        siox_activity_report_error( act_write, status );
        siox_activity_end( act_write );

        fprintf( stderr, "!!! Error closing file >%s<! !!!\n", filename );
        return( 0 );
    }

    // Now all the work is done, stop the activity's clock and move on to the reporting phase.
    siox_activity_stop( act_write );


    /*
     * SIOX - final negotiations
     */
    // Report our performance metrics for this activity.
    siox_activity_set_attribute( act_write, att_byteswritten, &total_bytes_written );
    siox_activity_end( act_write );


    /* Return number of bytes successfully written */
    return ( total_bytes_written );
}




void mufs_initialize()
{
    /* Test status and flag as initialized */
    assert(!(initialized++));

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

    // Register the interface we provide and our implementation of it.
    uiid = siox_system_information_lookup_interface_id( "MUFS", "OpenMUFS" );
    // Register our component as provider of this interface.
    // The empty parameter could carry a string describing the instance.
    cid = siox_component_register( uiid, ""  );

    // Register the activity types we will use
    act_type_write = siox_component_register_activity( uiid, "write" );

    // Register the attributes we will need:
    att_blocksize = siox_ontology_register_attribute_with_unit( "io",
                                                                "data volume/block size",
                                                                "Bytes",
                                                                SIOX_STORAGE_64_BIT_INTEGER );
    att_filename = siox_ontology_register_attribute( "filesystem",
                                                     "filename",
                                                     SIOX_STORAGE_STRING );
    att_bytes2write = siox_ontology_register_attribute_with_unit( "io",
                                                                  "data volume/to write",
                                                                  "Bytes",
                                                                  SIOX_STORAGE_64_BIT_INTEGER );
    att_byteswritten = siox_ontology_register_attribute_with_unit( "io",
                                                                   "data volume/written",
                                                                   "Bytes",
                                                                   SIOX_STORAGE_64_BIT_INTEGER );
    /// @todo Mark filename (and b2w?) as descriptors
    /// @todo Register filename twice (POSIX and MUFS)?

    // Set the block size our library uses as a component attribute
    uint64_t block_size = BLOCK_SIZE; // Can't pass a precompiler constant as a void* 
    siox_component_set_attribute( cid, att_blocksize, & block_size );
}