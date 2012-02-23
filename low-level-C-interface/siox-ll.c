/**
 * @file    siox-ll.c
 *          Implementation des SIOX-Low-Level-Interfaces.
 *
 *
 * \mainpage SIOX
 *
 * \section intro_sec Einführung
 * Das SIOX-Interface wurde aus den Rahmenbedingungen entwickelt,
 * die die Abstraktion des I/O-Pfadmodells @em IOPm an das System stellt.
 *
 * @authors Michaela Zimmer, Julian Kunkel & Marc Wiedemann
 * @date    2012
 *          GNU Public License.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "siox-ll.h"
#include "../ontology/ontology.h"


#define DEFAULT_ONTOLOGY "testontology"

/** Haben wir initialise_ontology() bereits durchlaufen? */
static bool initialised_ontology = false;

/** Die zu nutzende Ontologie. Kann vor dem ersten Aufruf von initialise_ontology() per siox_set_ontology()
 *  gesetzt werden; ansonsten wird DEFAULT_ONTOLOGY verwendet. */
static char sOntology[80] = "";



/** The next unassigned UNID. */
static unsigned long int    current_unid = 0L;

struct siox_unid_t {
    unsigned long int   id; /**< The actual ID. */
    };


/** The next unassigned AID. */
static unsigned long int    current_aid = 0L;

struct siox_aid_t {
    unsigned long int   id; /**< The actual ID. */
    };


/** The next unassigned DMID. */
static unsigned long int    current_dmid = 0L;

struct siox_dmid_t {
    unsigned long int   id; /**< The actual ID. */
    };



/**
 * Initialisiere die Ontologie-Bibliothek.
 * Wird nur beim ersten Zugriff auf aus der Ontologie zu lesende Daten (etwa die siox_register_...-Funktionen)
 * aufgerufen.
 */
static void initialise_ontology();



siox_unid
siox_register_node( const char * hwid,
                    const char * swid,
                    const char * iid)
{
    /* Draw fresh UNID */
    siox_unid unid = malloc( sizeof( struct siox_unid_t ) );
    (*unid).id = current_unid++;


    printf( "\n# Node registered as UNID %ld with hwid >%s<, swid >%s< and iid >%s<.\n",
        (*unid).id, hwid, swid, iid );

    return( unid );
}


void
siox_unregister_node( siox_unid unid )
{
    printf( "# UNID %ld unregistered.\n\n", (*unid).id );
}


void
siox_register_attribute( siox_unid              unid,
                         const char *           key,
                         enum siox_value_type   value_type,
                         void *                 value )
{
    printf( "# UNID %ld registered the following additional attributes:\n",
        (*unid).id );
    printf( "\t%s:\t", key );
    switch ( value_type ){
        case SIOX_TYPE_INTEGER:
            printf( "%d.\n", *((int*) value) );
            break;
        case SIOX_TYPE_LONG:
            printf( "%ld.\n", *((long*) value) );
            break;
        case SIOX_TYPE_FLOAT:
            printf( "%f.\n", *((float*) value) );
            break;
        case SIOX_TYPE_STRING:
            printf( "%s.\n", (char*) value );
            break;
    }
}


void
siox_register_edge( siox_unid       unid,
                    const char *    child_swid )
{
    printf( "# UNID %ld registered edge to child node >%s<.\n", (*unid).id, child_swid );
}


siox_dmid
siox_register_descriptor_map( siox_unid  unid,
                              siox_dtid  source_dtid,
                              siox_dtid  target_dtid)
{
    /* Draw fresh DMID */
    siox_dmid dmid = malloc( sizeof( struct siox_dmid_t ) );
    dmid->id = current_dmid++;


    printf( "# UNID %ld registered DMID %ld: %s -> %s.\n",
        unid->id, dmid->id, siox_ont_dtid_to_string( source_dtid ), siox_ont_dtid_to_string( target_dtid ) );

    return( dmid );
}


void
siox_create_descriptor( siox_unid       unid,
                        siox_dtid       dtid,
                        const char *    descriptor )
{
    printf( "\n= UNID %ld created descriptor >%s< of DTID %s.\n",
        unid->id, descriptor, siox_ont_dtid_to_string( dtid ) );
}


void
siox_send_descriptor( siox_unid     unid,
                      const char *  child_swid,
                      siox_dtid     dtid,
                      const char *  descriptor )
{
    printf( "= UNID %ld sent descriptor >%s< of DTID %s to child node >%s<.\n",
        unid->id, descriptor, siox_ont_dtid_to_string( dtid ), child_swid );
}


void
siox_receive_descriptor( siox_unid      unid,
                         siox_dtid      dtid,
                         const char *   descriptor )
{
    printf( "\n= UNID %ld received descriptor >%s< of DTID %s.\n",
        unid->id, descriptor, siox_ont_dtid_to_string( dtid ) );
}


void
siox_map_descriptor( siox_unid      unid,
                     siox_dmid      dmid,
                     const char *   source_descriptor,
                     const char *   target_descriptor )
{
    printf( "= UNID %ld applied DMID %ld: %s -> %s.\n",
        (*unid).id, (*dmid).id, source_descriptor, target_descriptor );
}


void
siox_release_descriptor( siox_unid      unid,
                         siox_dtid      dtid,
                         const char *   descriptor )
{
    printf( "= UNID %ld released descriptor >%s< of DTID %s.\n\n",
        (*unid).id, descriptor, siox_ont_dtid_to_string( dtid ) );
}


siox_aid
siox_start_activity( siox_unid      unid,
                     const char *   comment )
{
    /* Draw timestamp */
    time_t  timeStamp = time(NULL);

    /* Draw fresh AID */
    siox_aid aid = malloc( sizeof( struct siox_aid_t ) );
    (*aid).id = current_aid++;


    printf( "- UNID %ld started AID %ld at %s",
        (*unid).id, (*aid).id, ctime( &timeStamp ));
    if ( comment != NULL )
        printf( "\tKommentar:\t%s\n", comment );

    return( aid );
}


void
siox_stop_activity( siox_aid    aid )
{
    /* Draw timestamp */
    time_t  timeStamp = time(NULL);


    printf( "- AID %ld stopped at %s",
        (*aid).id, ctime( &timeStamp ));
}


void
siox_report_activity( siox_aid              aid,
                      siox_dtid             dtid,
                      const char *          descriptor,
                      siox_mid              mid,
                      enum siox_value_type  value_type,
                      void *                value,
                      const char *          details )
{
    printf( "- AID %ld, identified by >%s< of DTID %s, was measured as follows:\n",
        aid->id, descriptor, siox_ont_dtid_to_string( dtid ) );
    printf( "\t%s:\t", siox_ont_metric_get_name(
                        siox_ont_find_metric_by_mid( mid ) ) );
    switch ( value_type ){
        case SIOX_TYPE_INTEGER:
            printf( "%d\n", *((int*) value) );
            break;
        case SIOX_TYPE_LONG:
            printf( "%ld\n", *((long*) value) );
            break;
        case SIOX_TYPE_FLOAT:
            printf( "%f\n", *((float*) value) );
            break;
        case SIOX_TYPE_STRING:
            printf( "%s\n", (char*) value );
            break;
    }
    if (details != NULL)
        printf( "\tNote:\t%s\n", details );
}


void
siox_end_activity ( siox_aid    aid )
{
    /* Draw timestamp */
    time_t  timeStamp = time(NULL);


    printf( "- AID %ld finally ended at %s\n",
        (*aid).id, ctime( &timeStamp ));
}


void
siox_report( siox_unid              unid,
             siox_mid               mid,
             enum siox_value_type   value_type,
             void *                 value,
             const char *           details )
{
    printf( "- UNID %ld was measured as follows:\n",
        (*unid).id );
    printf( "\t%s:\t", siox_ont_metric_get_name(
                        siox_ont_find_metric_by_mid( mid ) ) );
    switch ( value_type ){
        case SIOX_TYPE_INTEGER:
            printf( "%d\n", *((int*) value) );
            break;
        case SIOX_TYPE_LONG:
            printf( "%ld\n", *((long*) value) );
            break;
        case SIOX_TYPE_FLOAT:
            printf( "%f\n", *((float*) value) );
            break;
        case SIOX_TYPE_STRING:
            printf( "%s\n", (char*) value );
            break;
    }
    if (details != NULL)
        printf( "\tNote:\t%s\n", details );

}


bool
siox_set_ontology( const char * name )
{
    /* Set ontology name if as yet unset. */
    if( strlen( sOntology ) == 0 )
    {
        strcpy( sOntology, name );
        return( true );
    }
    else
        return( false );
}


siox_dtid
siox_register_datatype( const char *                name,
                        enum siox_ont_storage_type  storage )
{
    siox_dtid   result;

    if( !initialised_ontology )
        initialise_ontology();

    result = siox_ont_register_datatype( name, storage );

    return( result );
}



siox_mid
siox_register_metric( const char *                 name,
                      const char *                 description,
                      enum siox_ont_unit_type      unit,
                      enum siox_ont_storage_type   storage,
                      enum siox_ont_scope_type     scope )
{
    siox_mid result;

    if( !initialised_ontology )
        initialise_ontology();

    result = siox_ont_register_metric( name,
                                       description,
                                       unit,
                                       storage,
                                       scope );

    return( result );
}


static void
initialise_ontology()
{
    initialised_ontology = true;

    siox_set_ontology( DEFAULT_ONTOLOGY ); /* Klappt nur, falls noch nicht gesetzt. */

    /* Ontologie öffnen; solange wir keine finalise()-Funktion haben, bleibt sie zu Programmende eben offen. */
    siox_ont_open_ontology( sOntology );
}
