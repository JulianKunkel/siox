/**
 * @file	ontology.h
 *			Implementation des SIOX-Ontology-Interfaces
 * 
 * @authors	Michaela Zimmer
 * @date	2011
 * 			GNU Public License
 */


#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "ontology.h"


#define METRICS_ARRAY_SIZE			 100
#define LINE_BUFFER_SIZE			2000


/*
 * Type Definitions
 * ================
 */
 
struct siox_mid_t
{
	int		id;	/**< The actual MID. */
};

struct siox_metric_t
{
	int							id;			/**< The metric's <em>MID</em>. */
	char * 						name;		/**< The metric's unique name. */
	char *						description;/**< The metric's description. */
	enum siox_ont_unit_type		unit;		/**< The metric's unit of measurement. */
	enum siox_ont_storage_type	storage;	/**< The minimal data type needed to store the metric's data. */
	enum siox_ont_scope_type	scope;		/**< The temporal scope of the metric's data. */
};


/*
 * Lib-local Variables
 * ===================
 */

/**
 * The current ontology file name.
 */
static const char *	current_ontology = NULL;

/**
 * The next free MID to assign to a new metric. Also, the nuber of metrics in the current ontology.
 */
static int	current_mid = 0;	

/**
 * A field of pointers to the actual metrics data.
 * Simple first implementation, to be replaced by something better soon.
 *
 * @todo Replace field of pointers with bbtree or anything that's better.
 */
static siox_metric	metrics[METRICS_ARRAY_SIZE];


/*
 * Lib-local Function Declarations
 * ===============================
 */
 
static siox_mid get_new_mid( int id );


/*
 * Function Definitions
 * ====================
 */

int
siox_ont_open_ontology( const char * file )
{
	FILE *			fh;
	char			sBuffer[LINE_BUFFER_SIZE];
	siox_metric *	pMetric;
	siox_metric		metric;
	int				line;
	int				id, unit, storage, scope;
	int				length;
	char 			*p, *q;


	/* Memorise file name */
	current_ontology = file;
	
	/* Clean metrics field */	
	for( pMetric = metrics; pMetric < metrics + METRICS_ARRAY_SIZE; pMetric++ )
		(*pMetric) = NULL;
	current_mid = 0;

	/* Open file */
	if( ( fh = fopen( file, "rt" ) ) != NULL )
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
				case 0:	/* Parse numeric values */
					/* Set up new metric object */
					metric = malloc( sizeof( struct siox_metric_t ) );
					if ( !metric )
					{
						fprintf( stderr, "Memory allocation error in siox_ont_open_ontology()!" );
						return( -1 );
					}
					if( 4 != sscanf( sBuffer, "%d, %d, %d, %d\n", &id, &unit, &storage, &scope) )
					{
						fprintf( stderr, "ERROR: Format error in file %s.\n", file );
						fclose( fh );
						return( -1 );
					}
					metric->id = id;
					metric->unit = (enum siox_ont_unit_type) unit;
					metric->storage = (enum siox_ont_storage_type) storage;
					metric->scope = (enum siox_ont_scope_type) scope;
					break;
				case 1: /* Parse for name */
					length = strlen( sBuffer );
					if( length < 5 )
					{
						fprintf( stderr, "ERROR: Metric name too short (%d) in file %s.", length, file );
						return( -1 );
					}
					metric->name = malloc( length );
					if ( !(metric->name) )
					{
						fprintf( stderr, "Memory allocation error in siox_ont_open_ontology()!" );
						return( -1 );
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
						fprintf( stderr, "Memory allocation error in siox_ont_open_ontology()!" );
						return( -1 );
					}
					p = metric->description + length - 2;
					q = sBuffer + length - 2;
					while( p >= metric->description )
						 *(p--) = *(q--);
					metric->description[ length - 1 ] = '\0'; /* Cut off the \n */
					break;
				case 3:	/* Parse for empty line */
					/* Insert metric into field */
					metrics[ metric->id ] = metric;
					current_mid++;
					break;
				default:	/* ERROR! */
					fprintf( stderr, "ERROR: Format error in file %s.\n", file );
					fclose( fh );
					return( -1 );
			}
		}
		
		/* Close file */
		if( fclose( fh ) == EOF )
		{
			fprintf( stderr, "ERROR: Couldn't close file %s properly.\n", file );
			return( -1 );
		}
	}

	/* Did we read any metrics from the file? */	
	if( current_mid > 0 )
		return( 0 );
	else
		return( 1 );
}


int
siox_ont_write_ontology()
{
	FILE *			fh;
	char			sBuffer[LINE_BUFFER_SIZE];
	siox_metric		*pMetric, metric;
	int				iError;

	/* Require an open ontology */
	if( !current_ontology )
	{
		fprintf( stderr, "ERROR: No ontology to write!\n" );
		return( -1 );
	}

	/* Open file */
	if( ( fh = fopen( current_ontology, "wt" ) ) != NULL )
	{
		/* Write metrics to file */
		for( iError = 0, pMetric = metrics;
			 ( !iError ) && ( pMetric < metrics + current_mid );
			 pMetric++ )
		{
			metric = (*pMetric);
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
			fprintf( stderr, "ERROR: Could not write metrics to file %s.\n", current_ontology );
			return( -1 );
		}
		
		/* Close file */
		if( fclose( fh ) == EOF )
		{
			fprintf( stderr, "ERROR: Could not close file %s properly.\n", current_ontology );
			return( -1 );
		}
	}
	else
	{
		fprintf( stderr, "ERROR: Could not open file %s for writing.\n", current_ontology );
		return( -1 );
	}
	
	return( 0 );
}


int
siox_ont_close_ontology()
{
	siox_metric	*pMetric;
	
	/* Require an open ontology */
	if( !current_ontology )
	{
		fprintf( stderr, "ERROR: No ontology!\n" );
		return( -1 );
	}

	/* Free metrics storage */
	for( pMetric = metrics; pMetric < metrics + current_mid; pMetric++ )
		free(*pMetric);

	/* Forget file name */
	current_ontology = NULL;
	
	return( 0 );
}


siox_mid
siox_ont_find_metric( const char * name)
{
	siox_metric		*pMetric;
	
	for( pMetric = metrics; pMetric < metrics + current_mid; pMetric++ )
		if( !strcmp( name, (*pMetric)->name ) )
			return get_new_mid( (*pMetric)->id );

	return( NULL );
}



siox_metric
siox_ont_get_metric( siox_mid mid )
{
	int id;
	

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
	
	if( id < 0  ||  METRICS_ARRAY_SIZE < id )
	{
		fprintf( stderr, "ERROR: MID out of range (%d)!\n", id );
		return( NULL );
	}
	
	if( !metrics[id] )
	{
		fprintf( stderr, "ERROR: MID does not exist (%d)!\n", id );
		return( NULL );
	}
	
	return( metrics[id] );
}


siox_mid siox_ont_get_metric_id( siox_metric metric );

char * siox_ont_get_metric_name( siox_metric metric );

char * siox_ont_get_metric_description( siox_metric metric );

enum siox_ont_unit_type siox_ont_get_metric_unit( siox_metric metric );

enum siox_ont_storage_type siox_ont_get_metric_storage( siox_metric metric );

enum siox_ont_scope_type siox_ont_get_metric_scope( siox_metric metric );



char *
siox_ont_metric_to_string( siox_metric metric )
{
	char*	sResult;
	int		nLength;
	

	/* Require an open ontology */
	if( !current_ontology )
	{
		fprintf( stderr, "ERROR: No ontology!\n" );
		return( NULL );
	}

	nLength = strlen( metric->name ) + strlen( metric->description ) + 50;
	sResult = malloc( sizeof( char[nLength] ) );
	
	if ( !sResult )
	{
		fprintf( stderr, "Memory allocation error in siox_ont_metric_to_string()!" );
		return( NULL );
	}
	
	sprintf( sResult, "Metric:\t%s\n"
					  "%s\n"
					  "Unit:\t%u\n"
					  "Storage:\t%u\n"
					  "Scope:\t%u\n",
					  metric->name,
					  metric->description,
					  metric->unit,
					  metric->storage,
					  metric->scope );
				
	return( sResult );				
}


int
siox_ont_count_metrics()
{
	/* Require an open ontology */
	if( !current_ontology )
	{
		fprintf( stderr, "ERROR: No ontology!\n" );
		return( -1 );
	}

	return( current_mid );
}


siox_mid
siox_ont_register_metric( const char *					name,
						  const char *					description,
						  enum siox_ont_unit_type		unit,
						  enum siox_ont_storage_type	storage,
						  enum siox_ont_scope_type		scope )
{
	siox_mid	mid;
	siox_metric	metric;
	
	/* Require an open ontology */
	if( !current_ontology )
	{
		fprintf( stderr, "ERROR: No ontology!\n" );
		return( NULL );
	}

	/** @todo Check for this very metric in ontology first. */
	
	mid = get_new_mid( current_mid++ );
	
	/* Set up new metric object */
	metric = malloc( sizeof( struct siox_metric_t ) );
	if ( !metric )
	{
		fprintf( stderr, "Memory allocation error in siox_ont_register_metric()!" );
		return( NULL );
	}
	metric->id = mid->id;
	metric->name = malloc( strlen( name ) + 1 );
	if ( !(metric->name) )
	{
		fprintf( stderr, "Memory allocation error in siox_ont_register_metric()!" );
		return( NULL );
	}
	strcpy( metric->name, name );
	metric->description = malloc( strlen( description ) + 1 );
	if ( !(metric->description) )
	{
		fprintf( stderr, "Memory allocation error in siox_ont_register_metric()!" );
		return( NULL );
	}
	strcpy( metric->description, description );
	metric->unit = unit;
	metric->storage = storage;
	metric->scope = scope;
	
	/* Insert metric into field */
	metrics[mid->id] = metric;
	
	return( mid );
}


/**
 * Create a new @em MID.
 *
 * @param[in]	id	The id the new @em MID is to have.
 * @returns			A new @em MID with the id given, unless there is no memory left,
 *					in which case @c NULL is returned.
 */
static siox_mid
get_new_mid( int id )
{
	siox_mid mid;
	
	/* Metrics field full? */
	if( id >= METRICS_ARRAY_SIZE )
	{
		/** @todo Dynamically grow array. */
		fprintf( stderr, "ERROR: Metrics memory exhausted (%d >= %d)!", id, METRICS_ARRAY_SIZE );
		return( NULL );
	}

	/* Draw fresh MID */
	mid = malloc( sizeof( struct siox_mid_t ) );
	if ( !mid )
	{
		fprintf( stderr, "Memory allocation error in get_new_mid()!" );
		return( NULL );
	}
	mid->id = id;

	return( mid );
}

