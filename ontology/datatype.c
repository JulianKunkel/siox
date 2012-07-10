/**
 * @file    datatype.c
 *          Implementation der Datentypen @em DTID und @em datatype
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
#include "datatype.h"


/*
 * Type Definitions
 * ================
 */


struct siox_dtid_t
{
   unsigned int     id; /**< The actual Data Type ID. */
};

struct siox_datatype_t
{
    int                         id;     /**< The datatype's <em>DTID</em>. */
    char *                      name;       /**< The datatype's unique name. */
    enum siox_ont_storage_type  storage;    /**< The minimal data type needed to store the datatype's data. */
};

/**
 * A list element for a simple list of datatypes.
 */
struct siox_ont_datatype_node_t
{
    siox_datatype                       datatype; /**< The datatype's actual data. */
    struct siox_ont_datatype_node_t     *next;  /**< The next list element. */
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
siox_ont_dtid_to_string( siox_dtid dtid )
{
    char * sResult;

    if ( dtid )
    {
        sResult = malloc( 20 );

        if( sResult )
        {
            sprintf( sResult, "%u", dtid->id );
            return( sResult );
        }
    }

    return( NULL );
}


void
siox_ont_free_dtid( siox_dtid dtid )
{
    free( dtid );

    return;
}


siox_datatype
siox_ont_new_datatype( void )
{
    siox_datatype datatype;

    /* Draw fresh metric */
    datatype = malloc( sizeof( struct siox_datatype_t ) );
    if ( !datatype )
    {
        fprintf( stderr, "ERROR: Memory allocation error in new_datatype()!\n" );
        return( NULL );
    }

    /* Initialise fields */
    datatype->name = NULL;
    datatype->storage = SIOX_STORAGE_UNASSIGNED;

    return( datatype );
}


void
siox_ont_free_datatype( siox_datatype datatype )
{
    if( datatype )
    {
        free( datatype->name );

        free( datatype );
    }

    return;
}


siox_dtid
siox_ont_datatype_get_dtid( siox_datatype datatype )
{
    if ( datatype )
        return siox_ont_dtid_from_id( datatype->id );
    else
        return( NULL );
}


const char *
siox_ont_datatype_get_name( siox_datatype datatype )
{
    if( datatype )
        return datatype->name;
    else
        return( NULL );
}


enum siox_ont_storage_type
siox_ont_datatype_get_storage( siox_datatype datatype )
{
    if( datatype )
        return( datatype->storage );
    else
        return( SIOX_UNIT_UNASSIGNED );
}


bool
siox_ont_dtid_is_equal( siox_dtid dtid1, siox_dtid dtid2 )
{
    if( dtid1 && dtid2 )
        return( dtid1->id == dtid2->id );
    else
        return( dtid1 == dtid2 );
}


char *
siox_ont_datatype_to_string( siox_datatype datatype )
{
    char*   sResult;
    int     nLength;


    nLength = strlen( datatype->name ) + 50;
    sResult = malloc( sizeof( char[nLength] ) );

    if ( !sResult )
    {
        fprintf( stderr, "ERROR: Memory allocation error in siox_ont_datatype_to_string()!\n" );
        return( NULL );
    }

    sprintf( sResult, "DataType:\t%s\n"
                      "Storage: %u\n",
                      datatype->name,
                      datatype->storage );

    return( sResult );
}


siox_dtid
siox_ont_dtid_from_id( int id )
{
    siox_dtid dtid;

    /* Draw fresh dtid */
    dtid = malloc( sizeof( struct siox_dtid_t ) );
    if ( !dtid )
    {
        fprintf( stderr, "ERROR: Memory allocation error in new_dtid_from_id()!\n" );
        return( NULL );
    }
    dtid->id = id;

    return( dtid );
}
