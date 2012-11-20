/**
 * @file siox-ll.c SIOX low-level interface source code.
 *
 * @date 11/2012
 * @copyright GNU Public License.
 * @authors Michaela Zimmer, Julian Kunkel, Marc Wiedemann & Alvaro Aguilera
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*#include <time.h>*/

#include <glib.h>

#include "siox-ll.h"
#include "../ontology/ontology.h"

/** 
 * Default ontology in case no other ontology has been set using 
 * siox_set_ontology(). */
#define DEFAULT_ONTOLOGY "testontology"

/** 
 * Whether or not the ontology has been already initialized with 
 * initialise_ontology().
 */
static bool initialised_ontology = false;

/** 
 * The ontology to use. Its value is set by initialize_ontology() using the 
 * siox_set_ontology() call. Otherwise DEFAULT_ONTOLOGY will be used.
 */
static char sOntology[80] = "";

/**
 * The format string to use for local date/time values.
 */
static const char sDateTimeFormat[] = "%F-%X %Z";


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


/** The next unassigned RCID. */
static unsigned long int    current_rcid = 0L;

struct siox_rcid_t {
    unsigned long int   id; /**< The actual ID. */
    };


/*struct siox_timestamp_t {
    gint64  ticks; *< The actual time in microseconds since 19700101. 
    };
*/

/**
 * Initialization of the ontology library.
 * This function is called only once during the first read access of the 
 * functions siox_register_*.  */
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
siox_node_attribute( siox_unid       unid,
                     siox_dtid      dtid,
                     const void *   value )
{
    printf( "# UNID %ld registered the following additional attributes:\n",
        (*unid).id );
    printf( "\t%s:\t", siox_ont_datatype_get_name( siox_ont_find_datatype_by_dtid( dtid ) ) );
    printf( "%s\n", siox_ont_data_to_string( dtid, value ) );
}


siox_aid
siox_start_activity( siox_unid          unid,
                     siox_timestamp     timestamp,
                     const char *       comment )
{
    GDateTime*  now;

    if (timestamp)
        now = timestamp;
    else
        now = g_date_time_new_now_local();

    /* Draw fresh AID */
    siox_aid aid = malloc( sizeof( struct siox_aid_t ) );
    (*aid).id = current_aid++;

    gchar* sNow=g_date_time_format(now, sDateTimeFormat);
    printf( "- UNID %ld started AID %ld at %s\n",
        (*unid).id, (*aid).id, sNow);
    if ( comment != NULL )
        printf( "\tKommentar:\t%s\n", comment );
    g_free(sNow);

    return( aid );
}


void
siox_stop_activity( siox_aid        aid,
                    siox_timestamp  timestamp )
{
    GDateTime*  now;

    if (timestamp)
        now = timestamp;
    else
        now = g_date_time_new_now_local();

    gchar* sNow=g_date_time_format(now, sDateTimeFormat);
    printf( "- AID %ld stopped at %s\n",
        (*aid).id, sNow);
    g_free(sNow);
}


void
siox_report_activity( siox_aid              aid,
                      siox_mid              mid,
                      void *                value )
{
    printf( "- AID %ld, was measured as follows:\n", aid->id );
    printf( "\t%s:\t", siox_ont_metric_get_name(
                        siox_ont_find_metric_by_mid( mid ) ) );
    printf( "%s\n", siox_ont_metric_data_to_string( mid, value ) );
}


void
siox_end_activity ( siox_aid          aid,
                    siox_timestamp    timestamp )
{
    GDateTime*  now;

    if (timestamp)
        now = timestamp;
    else
        now = g_date_time_new_now_local();

    gchar* sNow=g_date_time_format(now, sDateTimeFormat);
    printf( "- AID %ld finally ended at %s\n",
        (*aid).id, sNow);
    g_free(sNow);
}


void
siox_report( siox_unid              unid,
             siox_mid               mid,
             void *                 value )
{
    printf( "- UNID %ld was measured as follows:\n",
        (*unid).id );
    printf( "\t%s:\t", siox_ont_metric_get_name(
                        siox_ont_find_metric_by_mid( mid ) ) );
    printf( "%s\n", siox_ont_metric_data_to_string( mid, value ) );
}





siox_rcid
siox_describe_remote_call_start(siox_aid      aid,
                                const char *  target_hwid, 
                                const char *  target_swid,
                                const char *  target_iid)
{
    /* Draw fresh RCID */
    siox_rcid rcid = malloc( sizeof( struct siox_rcid_t ) );
    rcid->id = current_rcid++;

    printf( "\n# AID %ld opened description of remote call %ld to hwid >%s<, swid >%s< and iid >%s<.\n",
        aid->id,
        rcid->id,
        target_hwid?target_hwid:"(unknown)",
        target_swid?target_swid:"(unknown)",
        target_iid?target_iid:"(unknown)" );

    return( rcid );
}


void siox_remote_call_attribute(siox_rcid rcid, siox_dtid dtid, void * value)
{
    printf( "\n# Remote Call RCID %ld will send attribute of data type >%s<:\n\t>%s<.\n",
        rcid->id,
        siox_ont_data_to_string( dtid, value ),
        siox_ont_datatype_get_name( siox_ont_find_datatype_by_dtid( dtid ) ) );
}


void siox_describe_remote_call_end(siox_rcid  rcid)
{
    printf( "\n# Description of remote call RCID %ld closed.\n",
        rcid->id );
}


void siox_remote_call_receive(siox_aid aid, siox_dtid dtid, void * value)
{
    printf( "\n# AID %ld received remote call attribute of data type >%s<:\n\t>%s<.\n",
        aid->id,
        siox_ont_data_to_string( dtid, value ),
        siox_ont_datatype_get_name( siox_ont_find_datatype_by_dtid( dtid ) ) );
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
                                        "(No description yet)",
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
