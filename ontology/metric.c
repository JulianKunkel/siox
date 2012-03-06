/**
 * @file    metric.c
 *          Implementation der Datentypen @em MID und @em metric
 *          im SIOX-Ontology-Interface
 *
 * @authors Michaela Zimmer
 * @date    2012
 *          GNU Public License
 */

/** Switch off assertions unless compiled with -DDEBUG. */
#define NDEBUG
#ifdef  DEBUG
#undef  NDEBUG
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "enums.h"
#include "metric.h"


/*
 * Type Definitions
 * ================
 */


struct siox_mid_t
{
    unsigned int    id; /**< The actual Metric ID. */
};

struct siox_metric_t
{
    int                         id;     /**< The metric's <em>MID</em>. */
    char *                      name;       /**< The metric's unique name. */
    char *                      description;    /**< The metric's description. */
    enum siox_ont_unit_type     unit;       /**< The metric's unit of measurement. */
    enum siox_ont_storage_type  storage;    /**< The minimal data type needed to store the metric's data. */
    enum siox_ont_scope_type    scope;      /**< The temporal scope of the metric's data. */
};

/**
 * A list element for a simple list of metrics.
 */
struct siox_ont_metric_node_t
{
    siox_metric                     metric; /**< The metric's actual data. */
    struct siox_ont_metric_node_t   *next;  /**< The next list element. */
};


/*
 * Lib-local Variables
 * ===================
 */


/*
 * Lib-local Function Declarations
 * ===============================
 */


/*
 * Function Definitions
 * ====================
 */

const char*
siox_ont_mid_to_string( siox_mid mid )
{
    char * sResult;

    if ( mid )
    {
        sResult = malloc( 20 );

        if ( sResult )
        {
            sprintf( sResult, "%u", mid->id );
            return( sResult );
        }
    }

    return( NULL );
}


void
siox_ont_free_mid( siox_mid mid )
{
    free( mid );

    return;
}


void
siox_ont_free_metric( siox_metric metric )
{
    if( metric )
    {
        free( metric->name );
        free( metric->description );

        free( metric );
    }

    return;
}


siox_metric
siox_ont_new_metric( void )
{
    siox_metric metric;

    /* Draw fresh metric */
    metric = malloc( sizeof( struct siox_metric_t ) );
    if ( !metric )
    {
        fprintf( stderr, "ERROR: Memory allocation error in new_metric()!\n" );
        return( NULL );
    }

    /* Initialise fields */
    metric->name = NULL;
    metric->description = NULL;
    metric->unit = SIOX_UNIT_UNASSIGNED;
    metric->storage = SIOX_STORAGE_UNASSIGNED;
    metric->scope = SIOX_SCOPE_UNASSIGNED;

    return( metric );
}


siox_mid
siox_ont_metric_get_mid( siox_metric metric )
{
    if ( metric )
        return siox_ont_mid_from_id( metric->id );
    else
        return( NULL );
}


const char *
siox_ont_metric_get_name( siox_metric metric )
{
    if( metric )
        return metric->name;
    else
        return( NULL );
}


const char *
siox_ont_metric_get_description( siox_metric metric )
{
    if( metric )
        return metric->description;
    else
        return( NULL );
}


enum siox_ont_unit_type
siox_ont_metric_get_unit( siox_metric metric )
{
    if( metric )
        return( metric->unit );
    else
        return( SIOX_UNIT_UNASSIGNED );
}


enum siox_ont_storage_type
siox_ont_metric_get_storage( siox_metric metric )
{
    if( metric )
        return( metric->storage );
    else
        return( SIOX_UNIT_UNASSIGNED );
}


enum siox_ont_scope_type
siox_ont_metric_get_scope( siox_metric metric )
{
    if( metric )
        return( metric->scope );
    else
        return( SIOX_SCOPE_UNASSIGNED );
}


bool
siox_ont_mid_is_equal( siox_mid mid1, siox_mid mid2 )
{
    if( mid1 && mid2 )
        return( mid1->id == mid2->id );
    else
        return( mid1 == mid2 );
}


char *
siox_ont_metric_to_string( siox_metric metric )
{
    char*   sResult;
    int     nLength;

    nLength = strlen( metric->name ) + strlen( metric->description ) + 50;
    sResult = malloc( sizeof( char[nLength] ) );

    if ( !sResult )
    {
        fprintf( stderr, "ERROR: Memory allocation error in siox_ont_metric_to_string()!\n" );
        return( NULL );
    }

    sprintf( sResult, "Metric:\t%s\n"
                      "%s\n"
                      "Unit:    %u\n"
                      "Storage: %u\n"
                      "Scope:   %u\n",
                      metric->name,
                      metric->description,
                      metric->unit,
                      metric->storage,
                      metric->scope );

    return( sResult );
}


siox_mid
siox_ont_mid_from_id( int id )
{
    siox_mid mid;

    /* Draw fresh MID */
    mid = malloc( sizeof( struct siox_mid_t ) );
    if ( !mid )
    {
        fprintf( stderr, "ERROR: Memory allocation error in new_mid_from_id()!\n" );
        return( NULL );
    }
    mid->id = id;

    return( mid );
}
