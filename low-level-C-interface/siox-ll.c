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

/** Die Default-Ontologie, falls keine andere per siox_set_ontology() gesetzt worden sein sollte. */
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
siox_register_attribute( siox_unid       unid,
                         siox_dtid      dtid,
                         const void *   value )
{
    printf( "# UNID %ld registered the following additional attributes:\n",
        (*unid).id );
    printf( "\t%s:\t", siox_ont_datatype_get_name( siox_ont_find_datatype_by_dtid( dtid ) ) );
    printf( "%s\n", siox_ont_data_to_string( dtid, value ) );
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
siox_create_descriptor( siox_aid        aid,
                        siox_dtid       dtid,
                        const void *    descriptor )
{
    printf( "\n= AID %ld reports creation of descriptor >%s< of DTID %s.\n",
        aid->id, siox_ont_data_to_string( dtid, descriptor ), siox_ont_dtid_to_string( dtid ) );
}


void
siox_send_descriptor( siox_aid      aid,
                      const char *  child_swid,
                      siox_dtid     dtid,
                      const void *  descriptor )
{
    printf( "= AID %ld reports sending of descriptor >%s< of DTID %s to child node >%s<.\n",
        aid->id, siox_ont_data_to_string( dtid, descriptor ), siox_ont_dtid_to_string( dtid ), child_swid );
}


void
siox_receive_descriptor( siox_aid       aid,
                         siox_dtid      dtid,
                         const void *   descriptor )
{
    printf( "\n= AID %ld reports reception of descriptor >%s< of DTID %s.\n",
        aid->id, siox_ont_data_to_string( dtid, descriptor ), siox_ont_dtid_to_string( dtid ) );
}


void
siox_map_descriptor( siox_aid       aid,
                     siox_dmid      dmid,
                     const void *   source_descriptor,
                     const void *   target_descriptor )
{
    printf( "= AID %ld reports application of DMID %ld: %p -> %p.\n",
        (*aid).id, (*dmid).id, source_descriptor, target_descriptor );
    /** @TODO Look up actual @em DTIDs from @em DMID. */
}


void
siox_release_descriptor( siox_aid       aid,
                         siox_dtid      dtid,
                         const void *   descriptor )
{
    printf( "= AID %ld reports release of descriptor >%s< of DTID %s.\n\n",
        (*aid).id, siox_ont_data_to_string( dtid, descriptor ), siox_ont_dtid_to_string( dtid ) );
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
                      const void *          descriptor,
                      siox_mid              mid,
                      void *                value,
                      const char *          details )
{
    printf( "- AID %ld, identified by >%s< of DTID %s, was measured as follows:\n",
        aid->id, siox_ont_data_to_string( dtid, descriptor ), siox_ont_dtid_to_string( dtid ) );
    printf( "\t%s:\t", siox_ont_metric_get_name(
                        siox_ont_find_metric_by_mid( mid ) ) );
    printf( "%s\n", siox_ont_metric_data_to_string( mid, value ) );
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
             void *                 value,
             const char *           details )
{
    printf( "- UNID %ld was measured as follows:\n",
        (*unid).id );
    printf( "\t%s:\t", siox_ont_metric_get_name(
                        siox_ont_find_metric_by_mid( mid ) ) );
    printf( "%s\n", siox_ont_metric_data_to_string( mid, value ) );
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
    siox_dtid       dtid;
    siox_datatype   datatype;

    if( !initialised_ontology )
        initialise_ontology();

    dtid = siox_ont_find_dtid_by_name( name );

    if( dtid )
    {
        /* Compare found metric to current one, returning the mid if they match and NULL otherwise. */
        datatype = siox_ont_find_datatype_by_dtid( dtid );

        if( ( storage != siox_ont_datatype_get_storage( datatype ) ) )
        {
            fprintf( stderr, "ERROR: Could not register the datatype >%s< "
                             "as it exists already,\n"
                             "with different attributes:\n%s\n",
                             name, siox_ont_datatype_to_string( datatype ) );
            return( NULL );
        }
    }
    else
        dtid = siox_ont_register_datatype( name,
                                           storage );

    return( dtid );
}



siox_mid
siox_register_metric( const char *                 name,
                      const char *                 description,
                      enum siox_ont_unit_type      unit,
                      enum siox_ont_storage_type   storage,
                      enum siox_ont_scope_type     scope )
{
    siox_mid    mid;
    siox_metric metric;

    if( !initialised_ontology )
        initialise_ontology();

    mid = siox_ont_find_mid_by_name( name );

    if( mid )
    {
        /* Compare found metric to current one, returning the mid if they match and NULL otherwise. */
        metric = siox_ont_find_metric_by_mid( mid );

        if( ( unit != siox_ont_metric_get_unit( metric ) )
            || ( storage != siox_ont_metric_get_storage( metric ) )
            || ( scope != siox_ont_metric_get_scope( metric ) ) )
        {
            fprintf( stderr, "ERROR: Could not register the metric >%s< "
                             "as it exists already,\n"
                             "with different attributes:\n%s\n",
                             name, siox_ont_metric_to_string( metric ) );
            return( NULL );
        }
    }
    else
        mid = siox_ont_register_metric( name,
                                        description,
                                        unit,
                                        storage,
                                        scope );

    return( mid );
}


static void
initialise_ontology()
{
    initialised_ontology = true;

    siox_set_ontology( DEFAULT_ONTOLOGY ); /* Klappt nur, falls noch nicht gesetzt. */

    /* Ontologie öffnen; solange wir keine finalise()-Funktion haben, bleibt sie zu Programmende eben offen. */
    siox_ont_open_ontology( sOntology );
}
