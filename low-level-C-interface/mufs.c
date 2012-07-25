/**
 * @file    mufs.c
 *          Implementation des Mock-Up-File-System.
 *
 * @authors Julian Kunkel & Michaela Zimmer
 * @date    2012
 *          GNU Public License.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "mufs.h"
#include "siox-ll.h"


/** Größe der Blöcke, auf welche die zu schreibenden Daten aufgeteilt werden. */
#define BLOCK_SIZE 400


/** Statusvariable, die angibt, ob wir mufs_initialise() bereits durchlaufen haben. */
static int initialised = 0;

/*
 * Variablen für SIOX
 */
/** Die @em UNID der Bibliothek. */
static siox_unid    unid;
/* Die DTIDs der beiden Deskriptortypen, die MUFS kennt. */
static siox_dtid    dtid_fn;   /**< Die @em DTID für FileName-Deskriptoren. */
static siox_dtid    dtid_fp;   /**< Die @em DTID für FilePointer-Deskriptoren. */
/** Die @em DMID der Deskriptorübersetzung, die @em MUFS ausführen kann. */
static siox_dmid    dmid;
/** Die @em MID für unsere Leistungsmetrik */
static siox_mid     mid;


/**
 * Initialisiere die Bibliothek.
 * Bislang ausschließlich genutzt, um sie bei SIOX zu registrieren.
 */
static void mufs_initialise();


long
mufs_putfile( const char * filename, const char * contents )
{
    FILE *          fp;
    long            bytes_to_write;
    long            total_bytes_to_write;
    long            total_bytes_written;
    int             block;
    const char *    pData;
    const char *    pEnd;

    /* Variablen für SIOX */
    siox_aid    aid;
    char        fp_s[20];
    char        sBuffer[200];



    /*
     * If not done yet, initialise MUFS
     */
    if( !initialised )
        mufs_initialise();


    /*
     * SIOX - preliminary negotiations
     */
    sprintf( sBuffer, "Open file %s for writing.", filename );
    aid = siox_start_activity( unid, sBuffer );

    /* Report receiving a new descriptor */
    siox_receive_descriptor( aid, dtid_fn, &filename);

    /*
     * Open or, if necessary, create file via POSIX
     */
    fp = fopen( filename, "wt" );

    siox_end_activity( aid );
    /* Report the mapping of file name to file pointer */
    sprintf( fp_s, "%p", (void *) fp ); /* Turn foreign data type into string */
    siox_map_descriptor( aid, dmid, &filename, &fp );


    /*
     * Write contents in blocks of BLOCK_SIZE characters each via POSIX
     */
    total_bytes_to_write = 0;
    total_bytes_written = 0;
    pData = contents;
    for( pEnd = contents; *pEnd != 0; pEnd++)
        total_bytes_to_write++;

    for( pData = contents,
         block = 0;
            pData < pEnd;
                pData += bytes_to_write,
                total_bytes_written += bytes_to_write,
                block++ )
    {
        /* Notify SIOX that we are starting an activity for which we may later collect performance data. */
        sprintf( sBuffer, "Writing to file pointer %s, block #%d.", fp_s, block );
        aid = siox_start_activity( unid, sBuffer );

        bytes_to_write = pEnd - pData;
        if( bytes_to_write > BLOCK_SIZE )
            bytes_to_write = BLOCK_SIZE;

        if( fwrite( pData, bytes_to_write, 1, fp ) != 1 )
            {
                siox_end_activity( aid );

                fprintf( stderr, "!!! Fehler beim Schreiben von Block %d der Datei >%s<! !!!\n",
                         block, filename );
                return( 0 );
            }


        /* Report the end of the activity, any performance data gathered and close bookkeeping for it. */
        siox_stop_activity( aid );
        siox_report_activity( aid,
                              dtid_fp, &fp,
                              mid, &bytes_to_write,
                              NULL );
        siox_end_activity( aid );
    }


    /*
     * Close file via POSIX
     */
    sprintf( sBuffer, "Closing file pointer %s.", fp_s );
    aid = siox_start_activity( unid, sBuffer );

    if( fclose( fp ) == EOF )
    {
        siox_release_descriptor( aid, dtid_fp, &fp );
        siox_end_activity( aid );

        fprintf( stderr, "!!! Fehler beim Schließen der Datei >%s<! !!!\n", filename );
        return( 0 );
    }

    siox_stop_activity( aid );


    /*
     * SIOX - final negotiations
     */
    /* Mark any descriptors left as unused */
    siox_release_descriptor( aid, dtid_fp, &fp );
    siox_release_descriptor( aid, dtid_fn, &filename );

    siox_end_activity( aid );


    /* Return number of bytes successfully written */
    return ( total_bytes_written );
}




static void mufs_initialise()
{
    int     pid;
    char    pid_s[10];


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

    /* Find own PID and turn it into a string */
    pid = getpid();
    sprintf( pid_s, "%d", pid );
    /* Register node itself */
    unid = siox_register_node( "Michaelas T1500", "MUFS", pid_s );
    /* Register descriptor types we know */
    dtid_fn = siox_register_datatype( "FileName", SIOX_STORAGE_STRING );
    dtid_fp = siox_register_datatype( "MUFS-FilePointer", SIOX_STORAGE_64_BIT_INTEGER );
    /* Register ability to map MUFS-FileName to MUFS-FilePointer */
    dmid = siox_register_descriptor_map( unid, dtid_fn, dtid_fp );

    /* Register our performance metric with the ontology */
    mid = siox_register_metric( "Bytes Written",
                                "",
                                SIOX_UNIT_BYTES,
                                SIOX_STORAGE_64_BIT_INTEGER,
                                SIOX_SCOPE_SUM);
    printf( "Registered performance metric >%s< with ontology.\n",
            "Bytes Written" );
}

