/**
 * @file    ontology.c
 *          Implementation des SIOX-Ontology-Interfaces
 *
 * @authors Michaela Zimmer, Julian Kunkel & Marc Wiedemann
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
#include <stdarg.h>
#include <assert.h>

#include "enums.h"
#include "ontology.h"


#define MINIMUM_NAME_LENGTH    4    /**< Minimum length for a valid
                                         datatype's or metric's name. */
#define LINE_BUFFER_SIZE    2000    /**< Size (bytes) for buffer to read lines into. */


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

/**
 * The current ontology file name.
 * Also used to determine whether we have an open ontology (!= @c NULL) or not (== @c NULL ).
 */
static const char * current_ontology = NULL;

/**
 * The next free MID to assign to a new metric. Also, the number of metrics in the current ontology.
 */
static int  current_mid = 0;

/**
 * The next free DTID to assign to a new datatype. Also, the number of datatypes in the current ontology.
 */
static int  current_dtid = 0;

/**
 * A pointer to the head of the list of actual metrics data.
 * Simple implementation for the moment, eventually to be replaced by something better.
 *
 * @todo Replace the list with something more sophisticated, such as a btree.
 */
static struct siox_ont_metric_node_t * metric_head;

/**
 * A pointer to the head of the list of actual datatypes data.
 * Simple implementation for the moment, eventually to be replaced by something better.
 *
 * @todo Replace the list with something more sophisticated, such as a btree.
 */
static struct siox_ont_datatype_node_t * datatype_head;


/*
 * Lib-local Function Declarations
 * ===============================
 */

static bool     read_datatype_file( void );
static bool     insert_datatype_into_list( siox_datatype datatype );
static bool     write_datatype_file( void );

static bool     read_metric_file( void );
static bool     insert_metric_into_list( siox_metric metric );
static bool     write_metric_file( void );

static char *   to_string( const char * format, ... );


/*
 * Function Definitions
 * ====================
 */

bool
siox_ont_open_ontology( const char * file )
{
    /* Require closed ontology */
    if( current_ontology )
        siox_ont_close_ontology();

    /* Memorise file name */
    assert( current_ontology == NULL );
    current_ontology = file;

    return( read_datatype_file() && read_metric_file() );
}


bool
siox_ont_write_ontology()
{
    /* Require an open ontology */
    if( !current_ontology )
    {
        fprintf( stderr, "ERROR: No ontology to write!\n" );
        return( false );
    }

    return( write_datatype_file() && write_metric_file() );
}


bool
siox_ont_close_ontology()
{
    struct siox_ont_metric_node_t       *this_metric, *next_metric;
    struct siox_ont_datatype_node_t     *this_datatype, *next_datatype;

    /* Require an open ontology */
    if( !current_ontology )
    {
        fprintf( stderr, "ERROR: No ontology!\n" );
        return( false );
    }

    /* Free metrics storage */
    this_metric = metric_head;
    while( this_metric )
    {
        assert( this_metric->metric != NULL);
        next_metric = this_metric->next;
        siox_ont_free_metric( this_metric->metric );
        this_metric = next_metric;
    }
    metric_head = NULL;

    /* Free datatype storage */
    this_datatype = datatype_head;
    while( this_datatype )
    {
        assert( this_datatype->datatype != NULL);
        next_datatype = this_datatype->next;
        siox_ont_free_datatype( this_datatype->datatype );
        this_datatype = next_datatype;
    }
    datatype_head = NULL;

    /* Forget file name */
    current_ontology = NULL;

    return( true );
}


bool siox_ont_remove_ontology( const char * ontology )
{
    char    filename[80];
    bool    bResult = true;

    /* Remove Metrics file */
    sprintf( filename, "%s.m.ont", ontology );
    bResult = bResult && remove( filename );

    /* Remove DataTypes file */
    sprintf( filename, "%s.dt.ont", ontology );
    bResult = bResult && remove( filename );

    return( bResult );
}



/**
 * Tries to read the datatypes data from a file with the name "<current_ontology>.dt.ont".
 *
 * @returns     @c false if any problems arose, @c true otherwise.
 */
static bool
read_datatype_file( void )
{
    char            filename[80];
    FILE *          fh;
    char            sBuffer[LINE_BUFFER_SIZE];
    siox_datatype   datatype;
    int             line;
    int             id, storage;
    int             length;
    char            *p, *q;

    /* Construct file name */
    assert( current_ontology != NULL );
    sprintf( filename, "%s.dt.ont", current_ontology );

    /* Initialise datatypes field */
    assert( datatype_head == NULL );

    current_dtid = 0;

    /* Open file */
    if( ( fh = fopen( filename, "rt" ) ) != NULL )
    {
        line = 0;
        /* Read line from file */
        while( fgets( sBuffer, LINE_BUFFER_SIZE, fh ) )
        {
            /* Allow for comment lines */
            if( sBuffer[0] == '#' )
                continue;
            switch( line++ % 3 )
            {
                case 0: /* Parse numeric values */
                    /* Set up new datatype object */
                    datatype = siox_ont_new_datatype();
                    if ( !datatype )
                        return( false );
                    if( 4 != sscanf( sBuffer, "%d, %d\n", &id, &storage ) )
                    {
                        fprintf( stderr, "ERROR: Format error in file %s.\n", filename );
                        fclose( fh );
                        siox_ont_free_datatype( datatype );
                        current_ontology = NULL;
                        return( false );
                    }
                    datatype->id = id;
                    datatype->storage = (enum siox_ont_storage_type) storage;
                    break;
                case 1: /* Parse for name */
                    length = strlen( sBuffer );
                    if( length < MINIMUM_NAME_LENGTH )
                    {
                        fprintf( stderr, "ERROR: Datatype name too short (%d < %d) in file %s.\n",
                                 length, MINIMUM_NAME_LENGTH, filename );
                        fclose( fh );
                        siox_ont_free_datatype( datatype );
                        current_ontology = NULL;
                        return( false );
                    }
                    datatype->name = malloc( length );
                    if ( !(datatype->name) )
                    {
                        fprintf( stderr, "ERROR: Memory allocation error in siox_ont_open_ontology()!\n" );
                        fclose( fh );
                        siox_ont_free_datatype( datatype );
                        current_ontology = NULL;
                        return( false );
                    }
                    p = datatype->name + length - 2;
                    q = sBuffer + length - 2;
                    while( p >= datatype->name )
                         *(p--) = *(q--);
                    datatype->name[ length - 1 ] = '\0'; /* Cut off the \n */
                    break;
                case 2: /* Parse for empty line */
                    if( !insert_datatype_into_list( datatype ) )
                    {
                        fprintf( stderr, "ERROR: Cannot store datatype >%s< due to storage limitations.\n",
                                         datatype->name );
                        fclose( fh );
                        siox_ont_free_datatype( datatype );
                        current_ontology = NULL;
                        return( false );
                    }
                    /* Insert datatype into field */
                    current_dtid = datatype->id + 1;
                    break;
                default:    /* ERROR! */
                    /* This should never happen! */
                    fprintf( stderr, "ERROR: Format error in file %s.\n", filename );
                    fclose( fh );
                    current_ontology = NULL;
                    return( false );
            }
        }

        /* Close file */
        if( fclose( fh ) == EOF )
        {
            fprintf( stderr, "ERROR: Couldn't close file %s properly.\n", filename );
            current_ontology = NULL;
            return( false );
        }
    }

    return( true );
}


/**
 * Tries to write the datatype data to a file with the name "<current_ontology>.dt.ont".
 *
 * @returns     @c true on success, @c false otherwise.
 */
static bool
write_datatype_file( void )
{
    FILE *                            fh;
    char                              filename[80];
    char                              sBuffer[LINE_BUFFER_SIZE];
    siox_datatype                     datatype;
    int                               iError = 0;
    struct siox_ont_datatype_node_t  *current;


    sprintf( filename, "%s.dt.ont", current_ontology );

    /* Open file */
    if( ( fh = fopen( filename, "wt" ) ) != NULL )
    {
        /** @todo Write file header comment */

        /* Write datatypes to file */
        for( current = datatype_head; current; current = current->next )
        {
            datatype = current->datatype;
            assert( datatype != NULL );
            /* fprintf( stderr, "Writing to file:\n%s\n", siox_ont_datatype_to_string( datatype ) ); */
            sprintf( sBuffer, "%d, %d\n",
                              datatype->id,
                              datatype->storage );
            if( fputs( sBuffer, fh ) == EOF )
                iError++;
            sprintf( sBuffer, "%s\n", datatype->name );
            if( fputs( sBuffer, fh ) == EOF )
                iError++;
            if( fputs( "\n", fh ) == EOF )
                iError++;
        }
        if( iError )
        {
            fprintf( stderr, "ERROR: Could not write datatypes to file %s.\n", filename );
            fclose( fh );
            return( false );
        }

        /* Close file */
        if( fclose( fh ) == EOF )
        {
            fprintf( stderr, "ERROR: Could not close file %s properly.\n", filename );
            return( false );
        }
    }
    else
    {
        fprintf( stderr, "ERROR: Could not open file %s for writing.\n", filename );
        return( false );
    }

    return( true );
}


siox_mid
siox_ont_find_mid_by_name( const char * name)
{
    struct siox_ont_metric_node_t  *current;

    current = metric_head;

    while( current )
    {
        assert( current->metric != NULL );
        if( strcmp( name, current->metric->name ) == 0 )
            return( siox_ont_mid_from_id( current->metric->id ) );
        current = current->next;
    }

    return( NULL );
}


siox_metric
siox_ont_find_metric_by_mid( siox_mid mid )
{
    int                     id;
    struct siox_ont_metric_node_t  *current;

    /* Require an open ontology */
    if( !current_ontology )
    {
        fprintf( stderr, "ERROR: No ontology!\n" );
        return( NULL );
    }

    if( mid )
        id = mid->id;
    else
    {
        fprintf( stderr, "ERROR: Empty MID object!\n" );
        return( NULL );
    }

    if( id < 0  ||  current_mid <= id )
    {
        fprintf( stderr, "ERROR: MID %d out of range (0 - %d)!\n", id, current_mid );
        return( NULL );
    }

    for( current = metric_head; current; current = current->next )
    {
        assert( current->metric != NULL );
        if( current->metric->id == id )
            return( current->metric );
    }

    return( NULL );
}


int
siox_ont_count_metrics()
{
    struct siox_ont_metric_node_t  *current;
    int                             result;

    /* Require an open ontology */
    if( !current_ontology )
    {
        fprintf( stderr, "ERROR: No ontology!\n" );
        return( -1 );
    }

    result = 0;
    for( current = metric_head; current; current = current->next )
        result++;

    return( result );
}


siox_mid
siox_ont_register_metric( const char *                  name,
                          const char *                  description,
                          enum siox_ont_unit_type       unit,
                          enum siox_ont_storage_type    storage,
                          enum siox_ont_scope_type      scope )
{
    siox_mid    mid;
    siox_metric metric;

    /* Require an open ontology */
    if( !current_ontology )
    {
        fprintf( stderr, "ERROR: No ontology!\n" );
        return( NULL );
    }

    mid = siox_ont_find_mid_by_name( name );
    if( mid )
    {
        fprintf( stderr, "ERROR: The unique name >%s< exists already!\n", name );
        siox_ont_free_mid( mid );
        return( NULL );
    }

    mid = siox_ont_mid_from_id( current_mid );
    if( !mid )
    {
        fprintf( stderr, "ERROR: Cannot register metric >%s< due to storage limitations.\n", name );
        return( NULL );
    }
    current_mid++;

    /* Set up new metric object */
    metric = siox_ont_new_metric();
    if ( !metric )
    {
        fprintf( stderr, "ERROR: Cannot register metric >%s< due to storage limitations.\n", name );
        siox_ont_free_mid( mid );
        return( NULL );
    }
    metric->id = mid->id;
    metric->name = malloc( strlen( name ) + 1 );
    if ( !(metric->name) )
    {
        fprintf( stderr, "Memory allocation error in siox_ont_register_metric()!\n" );
        siox_ont_free_metric( metric );
        siox_ont_free_mid( mid );
        return( NULL );
    }
    strcpy( metric->name, name );
    metric->description = malloc( strlen( description ) + 1 );
    if ( !(metric->description) )
    {
        fprintf( stderr, "Memory allocation error in siox_ont_register_metric()!\n" );
        siox_ont_free_metric( metric );
        siox_ont_free_mid( mid );
        return( NULL );
    }
    strcpy( metric->description, description );
    metric->unit = unit;
    metric->storage = storage;
    metric->scope = scope;

    /* Insert metric into field */
    if( !insert_metric_into_list( metric ) )
    {
        fprintf( stderr, "ERROR: Cannot register metric >%s< due to storage limitations.\n",
                         metric->name );
        siox_ont_free_metric( metric );
        return( NULL );
    }

    return( mid );
}


/**
 * Tries to read the metrics data from a file with the name "<current_ontology>.m.ont".
 *
 * @returns     @c false if any problems arose, @c true otherwise.
 */
static bool
read_metric_file( void )
{
    char            filename[80];
    FILE *          fh;
    char            sBuffer[LINE_BUFFER_SIZE];
    siox_metric     metric;
    int             line;
    int             id, unit, storage, scope;
    int             length;
    char            *p, *q;

    /* Construct file name */
    assert( current_ontology != NULL );
    sprintf( filename, "%s.m.ont", current_ontology );

    /* Initialise metrics field */
    assert( metric_head == NULL );

    current_mid = 0;

    /* Open file */
    if( ( fh = fopen( filename, "rt" ) ) != NULL )
    {
        line = 0;
        /* Read line from file */
        while( fgets( sBuffer, LINE_BUFFER_SIZE, fh ) )
        {
            /* Allow for comment lines */
            if( sBuffer[0] == '#' )
                continue;
            switch( line++ % 4 )
            {
                case 0: /* Parse numeric values */
                    /* Set up new metric object */
                    metric = siox_ont_new_metric();
                    if ( !metric )
                        return( false );
                    if( 4 != sscanf( sBuffer, "%d, %d, %d, %d\n", &id, &unit, &storage, &scope) )
                    {
                        fprintf( stderr, "ERROR: Format error in file %s.\n", filename );
                        fclose( fh );
                        siox_ont_free_metric( metric );
                        current_ontology = NULL;
                        return( false );
                    }
                    metric->id = id;
                    metric->unit = (enum siox_ont_unit_type) unit;
                    metric->storage = (enum siox_ont_storage_type) storage;
                    metric->scope = (enum siox_ont_scope_type) scope;
                    break;
                case 1: /* Parse for name */
                    length = strlen( sBuffer );
                    if( length < MINIMUM_NAME_LENGTH )
                    {
                        fprintf( stderr, "ERROR: Metric name too short (%d < %d) in file %s.\n",
                                 length, MINIMUM_NAME_LENGTH, filename );
                        fclose( fh );
                        siox_ont_free_metric( metric );
                        current_ontology = NULL;
                        return( false );
                    }
                    metric->name = malloc( length );
                    if ( !(metric->name) )
                    {
                        fprintf( stderr, "ERROR: Memory allocation error in siox_ont_open_ontology()!\n" );
                        fclose( fh );
                        siox_ont_free_metric( metric );
                        current_ontology = NULL;
                        return( false );
                    }
                    p = metric->name + length - 2;
                    q = sBuffer + length - 2;
                    while( p >= metric->name )
                         *(p--) = *(q--);
                    metric->name[ length - 1 ] = '\0'; /* Cut off the \n */
                    break;
                case 2: /* Parse for description */
                    length = strlen( sBuffer );
                    if( length < 1 )
                        length = 1;
                    metric->description = malloc( length );
                    if ( !(metric->description) )
                    {
                        fprintf( stderr, "ERROR: Memory allocation error in siox_ont_open_ontology()!\n" );
                        fclose( fh );
                        siox_ont_free_metric( metric );
                        current_ontology = NULL;
                        return( false );
                    }
                    p = metric->description + length - 2;
                    q = sBuffer + length - 2;
                    while( p >= metric->description )
                         *(p--) = *(q--);
                    metric->description[ length - 1 ] = '\0'; /* Cut off the \n */
                    break;
                case 3: /* Parse for empty line */
                    if( !insert_metric_into_list( metric ) )
                    {
                        fprintf( stderr, "ERROR: Cannot store metric >%s< due to storage limitations.\n",
                                         metric->name );
                        fclose( fh );
                        siox_ont_free_metric( metric );
                        current_ontology = NULL;
                        return( false );
                    }
                    /* Insert metric into field */
                    current_mid = metric->id + 1;
                    break;
                default:    /* ERROR! */
                    /* This should never happen! */
                    fprintf( stderr, "ERROR: Format error in file %s.\n", filename );
                    fclose( fh );
                    current_ontology = NULL;
                    return( false );
            }
        }

        /* Close file */
        if( fclose( fh ) == EOF )
        {
            fprintf( stderr, "ERROR: Couldn't close file %s properly.\n", filename );
            current_ontology = NULL;
            return( false );
        }
    }

    return( true );
}


/**
 * Tries to write the metrics data to a file with the name "<current_ontology>.m.ont".
 *
 * @returns     @c true on success, @c false otherwise.
 */
static bool
write_metric_file( void )
{
    FILE *                          fh;
    char                            filename[80];
    char                            sBuffer[LINE_BUFFER_SIZE];
    siox_metric                     metric;
    int                             iError = 0;
    struct siox_ont_metric_node_t  *current;


    sprintf( filename, "%s.m.ont", current_ontology );

    /* Open file */
    if( ( fh = fopen( filename, "wt" ) ) != NULL )
    {
        /** @todo Write file header comment */

        /* Write metrics to file */
        for( current = metric_head; current; current = current->next )
        {
            metric = current->metric;
            assert( metric != NULL );
            /* fprintf( stderr, "Writing to file:\n%s\n", siox_ont_metric_to_string( metric ) ); */
            sprintf( sBuffer, "%d, %d, %d, %d\n",
                              metric->id,
                              metric->unit,
                              metric->storage,
                              metric->scope );
            if( fputs( sBuffer, fh ) == EOF )
                iError++;
            sprintf( sBuffer, "%s\n", metric->name );
            if( fputs( sBuffer, fh ) == EOF )
                iError++;
            sprintf( sBuffer, "%s\n", metric->description );
            if( fputs( sBuffer, fh ) == EOF )
                iError++;
            if( fputs( "\n", fh ) == EOF )
                iError++;
        }
        if( iError )
        {
            fprintf( stderr, "ERROR: Could not write metrics to file %s.\n", filename );
            fclose( fh );
            return( false );
        }

        /* Close file */
        if( fclose( fh ) == EOF )
        {
            fprintf( stderr, "ERROR: Could not close file %s properly.\n", filename );
            return( false );
        }
    }
    else
    {
        fprintf( stderr, "ERROR: Could not open file %s for writing.\n", filename );
        return( false );
    }

    return( true );
}


/**
 * Inserts a metric into the list of metrics according to ascending order of id.
 *
 * @param[in]   metric  The metric to be inserted.
 *
 * @returns             @c true if successfully inserted, otherwise @c false.
 */
static bool
insert_metric_into_list( siox_metric metric )
{
    struct siox_ont_metric_node_t  *last, *this, *new;

    assert( metric != NULL );

    new = malloc( sizeof( struct siox_ont_metric_node_t ) );
    if( !new )
        return( false );
    new->metric = metric;

    /* Insert as only element */
    if( !metric_head )
    {
        new->next = NULL;
        metric_head = new;
        return( true );
    }

    assert( metric_head->metric != NULL );

    /* Insert before first element */
    if( metric->id < metric_head->metric->id )
    {
        new->next = metric_head;
        metric_head = new;
        return( true );
    }

    /* Insert between two elements */
    last = metric_head;
    this = metric_head->next;
    while( this != NULL )
    {
        assert( this->metric != NULL );
        if ( metric->id <= this->metric->id )
        {
            new->next = this;
            last->next = new;
            return( true );
        }
        last = this;
        this = this->next;
    }

    /* Insert behind last element */
    new->next = NULL;
    last->next = new;
    return( true );
}


int
siox_ont_count_datatypes()
{
    struct siox_ont_datatype_node_t  *current;
    int                     result;

    /* Require an open ontology */
    if( !current_ontology )
    {
        fprintf( stderr, "ERROR: No ontology!\n" );
        return( -1 );
    }

    result = 0;
    for( current = datatype_head; current; current = current->next )
        result++;

    return( result );
}


siox_datatype
siox_ont_find_datatype_by_dtid( siox_dtid dtid )
{
    int                     id;
    struct siox_ont_datatype_node_t  *current;

    /* Require an open ontology */
    if( !current_ontology )
    {
        fprintf( stderr, "ERROR: No ontology!\n" );
        return( NULL );
    }

    if( dtid )
        id = dtid->id;
    else
    {
        fprintf( stderr, "ERROR: Empty dtid object!\n" );
        return( NULL );
    }

    if( id < 0  ||  current_dtid <= id )
    {
        fprintf( stderr, "ERROR: dtid %d out of range (0 - %d)!\n", id, current_dtid );
        return( NULL );
    }

    for( current = datatype_head; current; current = current->next )
    {
        assert( current->datatype != NULL );
        if( current->datatype->id == id )
            return( current->datatype );
    }

    return( NULL );
}


siox_dtid
siox_ont_find_dtid_by_name( const char * name)
{
    struct siox_ont_datatype_node_t  *current;

    current = datatype_head;

    while( current )
    {
        assert( current->datatype != NULL );
        if( strcmp( name, current->datatype->name ) == 0 )
            return( siox_ont_dtid_from_id( current->datatype->id ) );
        current = current->next;
    }

    return( NULL );
}


/**
 * Inserts a datatype into the list of datatypes according to ascending order of id.
 *
 * @param[in]   datatype  The datatype to be inserted.
 *
 * @returns             @c true if successfully inserted, otherwise @c false.
 */
static bool
insert_datatype_into_list( siox_datatype datatype )
{
    struct siox_ont_datatype_node_t  *last, *this, *new;

    assert( datatype != NULL );

    new = malloc( sizeof( struct siox_ont_datatype_node_t ) );
    if( !new )
        return( false );
    new->datatype = datatype;

    /* Insert as only element */
    if( !datatype_head )
    {
        new->next = NULL;
        datatype_head = new;
        return( true );
    }

    assert( datatype_head->datatype != NULL );

    /* Insert before first element */
    if( datatype->id < datatype_head->datatype->id )
    {
        new->next = datatype_head;
        datatype_head = new;
        return( true );
    }

    /* Insert between two elements */
    last = datatype_head;
    this = datatype_head->next;
    while( this != NULL )
    {
        assert( this->datatype != NULL );
        if ( datatype->id <= this->datatype->id )
        {
            new->next = this;
            last->next = new;
            return( true );
        }
        last = this;
        this = this->next;
    }

    /* Insert behind last element */
    new->next = NULL;
    last->next = new;
    return( true );
}


siox_dtid
siox_ont_register_datatype( const char *                name,
                          enum siox_ont_storage_type    storage )
{
    siox_dtid    dtid;
    siox_datatype datatype;

    /* Require an open ontology */
    if( !current_ontology )
    {
        fprintf( stderr, "ERROR: No ontology!\n" );
        return( NULL );
    }

    dtid = siox_ont_find_dtid_by_name( name );
    if( dtid )
    {
        fprintf( stderr, "ERROR: The unique name >%s< exists already!\n", name );
        siox_ont_free_dtid( dtid );
        return( NULL );
    }

    dtid = siox_ont_dtid_from_id( current_dtid );
    if( !dtid )
    {
        fprintf( stderr, "ERROR: Cannot register datatype >%s< due to storage limitations.\n", name );
        return( NULL );
    }
    current_dtid++;

    /* Set up new datatype object */
    datatype = siox_ont_new_datatype();
    if ( !datatype )
    {
        fprintf( stderr, "ERROR: Cannot register datatype >%s< due to storage limitations.\n", name );
        siox_ont_free_dtid( dtid );
        return( NULL );
    }
    datatype->id = dtid->id;
    datatype->name = malloc( strlen( name ) + 1 );
    if ( !(datatype->name) )
    {
        fprintf( stderr, "Memory allocation error in siox_ont_register_datatype()!\n" );
        siox_ont_free_datatype( datatype );
        siox_ont_free_dtid( dtid );
        return( NULL );
    }
    strcpy( datatype->name, name );
    datatype->storage = storage;

    /* Insert datatype into field */
    if( !insert_datatype_into_list( datatype ) )
    {
        fprintf( stderr, "ERROR: Cannot register datatype >%s< due to storage limitations.\n",
                         datatype->name );
        siox_ont_free_datatype( datatype );
        return( NULL );
    }

    return( dtid );
}



const char *
siox_ont_data_to_string( siox_dtid dtid, const void * value )
{
    siox_datatype   dtType;
    char *          sResult;

    /* Check whether DTID is valid. */
    dtType = siox_ont_find_datatype_by_dtid( dtid );
    if (dtType == NULL)
    {
        fprintf( stderr, "ERROR: Invalid DTID >%s<.\n",
                 siox_ont_dtid_to_string( dtid ) );
        return( NULL );
    }

    /* Convert according to correct scheme. */
    switch( dtType->storage )
    {
        case SIOX_STORAGE_32_BIT_INTEGER:
            fprintf(stderr, "data_to_string: SIOX_STORAGE_32_BIT_INTEGER\n");
            sResult = to_string( "%d", *((const int *) value) );
            fprintf(stderr, "Result: %s.\n", sResult);
            break;
        case SIOX_STORAGE_64_BIT_INTEGER:
            fprintf(stderr, "data_to_string: SIOX_STORAGE_64_BIT_INTEGER\n");
            sResult = to_string( "%ld", *((const long int *) value) );
            fprintf(stderr, "Result: %s.\n", sResult);
            break;

        case SIOX_STORAGE_FLOAT:
            fprintf(stderr, "data_to_string: SIOX_STORAGE_FLOAT\n");
            sResult = to_string( "%f", *((const float *) value) );
            fprintf(stderr, "Result: %s.\n", sResult);
            break;
        case SIOX_STORAGE_DOUBLE:
            fprintf(stderr, "data_to_string: SIOX_STORAGE_DOUBLE\n");
            sResult = to_string( "%f", *((const double *) value) );
            fprintf(stderr, "Result: %s.\n", sResult);
            break;

        case SIOX_STORAGE_STRING:
            fprintf(stderr, "data_to_string: SIOX_STORAGE_STRING\n");
            sResult = to_string( "%s", *((const char* *) value) );
            /*sResult = *((char**) value);*/
            fprintf(stderr, "Result: %s.\n", sResult);
            break;

        case SIOX_STORAGE_UNASSIGNED:
        default:
            fprintf(stderr, "data_to_string: SIOX_STORAGE_UNASSIGNED\n");
            sResult = to_string( "(Unknown data type)" );
            fprintf(stderr, "Result: %s.\n", sResult);
            break;
    }

    return( sResult );
}


const char *
siox_ont_metric_data_to_string( siox_mid mid, const void * value )
{
    siox_metric     metric;
    char *          sResult;

    /* Check whether MID is valid. */
    metric = siox_ont_find_metric_by_mid( mid );
    if (metric == NULL)
    {
        fprintf( stderr, "ERROR: Invalid MID >%s<.\n",
                 siox_ont_mid_to_string( mid ) );
        return( NULL );
    }

    /* Convert according to correct scheme. */
    switch( metric->storage )
    {
        case SIOX_STORAGE_32_BIT_INTEGER:
            fprintf(stderr, "metric_data_to_string: SIOX_STORAGE_32_BIT_INTEGER\n");
            sResult = to_string( "%d", *((const int *) value) );
            fprintf(stderr, "Result: %s.\n", sResult);
            break;
        case SIOX_STORAGE_64_BIT_INTEGER:
            fprintf(stderr, "metric_data_to_string: SIOX_STORAGE_64_BIT_INTEGER\n");
            sResult = to_string( "%ld", *((const long int *) value) );
            fprintf(stderr, "Result: %s.\n", sResult);
            break;

        case SIOX_STORAGE_FLOAT:
            fprintf(stderr, "metric_data_to_string: SIOX_STORAGE_FLOAT\n");
            sResult = to_string( "%f", *((const float *) value) );
            fprintf(stderr, "Result: %s.\n", sResult);
            break;
        case SIOX_STORAGE_DOUBLE:
            fprintf(stderr, "metric_data_to_string: SIOX_STORAGE_DOUBLE\n");
            sResult = to_string( "%f", *((const double *) value) );
            fprintf(stderr, "Result: %s.\n", sResult);
            break;

        case SIOX_STORAGE_STRING:
            fprintf(stderr, "metric_data_to_string: SIOX_STORAGE_STRING\n");
            sResult = to_string( "%s", *((const char* *) value) );
            // sResult = (char *) value;
            fprintf(stderr, "Result: %s.\n", sResult);
            break;

        case SIOX_STORAGE_UNASSIGNED:
        default:
            fprintf(stderr, "metric_data_to_string: SIOX_STORAGE_UNASSIGNED\n");
            sResult = to_string( "(Unknown data type)" );
            fprintf(stderr, "Result: %s.\n", sResult);
            break;
    }

    return( sResult );
}


/**
 * Function to turn argument list into string. The memory needed will be allocated as exactly as possible.
 *
 * @param[in]   format  The format string, as would be used in printf().
 * @param[in]   ...     The list of arguments to be inserted into the string.
 *
 * @returns     A string containing the arguments as specified by the format.
 */
static char *
to_string(const char * format, ...)
{
   int      nBufferSize = 20; /* A small value to begin with. */
   int      nLength = 0;
   char *   p;
   char *   pNew;
   va_list  arguments;


   /* Claim initial space of size nBufferSize. */
   if( ( p = malloc( nBufferSize ) ) == NULL )
   {
       fprintf( stderr, "Memory allocation error in ontology.c:to_string()!\n" );
       return( NULL );
   }
   /*fprintf(stderr, "tostring(): Allocated %d Bytes at %p.\n", nBufferSize, p);*/

   for(;;)
   {
        /* Try to convert in the space we have. */
        va_start( arguments, format );
        nLength = vsnprintf( p, nBufferSize, format, arguments );
        va_end( arguments );
        /*fprintf(stderr, "tostring(): nLength = %d, nBufferSize = %d.\n", nLength, nBufferSize);*/

        /* Are we done? */
        if(( nLength > -1 ) && ( nLength < nBufferSize ))
            return p;
        /*
        else
            fprintf(stderr, "tostring(): NOT done; nLength = %d, nBufferSize = %d.\n", nLength, nBufferSize);


        fprintf(stderr,"tostring(): nLength = %d >= %d = nBufferSize\n", nLength, nBufferSize);
        */

        /* Get more space and try until successful. */
        if( nLength > -1 )    /* glibc 2.1 */
            nBufferSize = nLength + 1; /* The exact size needed */
        else           /* glibc 2.0 */
            nBufferSize *= 2;  /* Double the last try's size */

        /* Relocate variable to new (larger) memory area. */
        if( ( pNew = realloc ( p, nBufferSize ) ) == NULL )
        {
            free( p );
            fprintf( stderr, "Memory allocation error in ontology.c:to_string()!\n" );
            return( NULL );
        }
        else
            p = pNew;
   }
}
