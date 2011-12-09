/**
 * @file	ontology.h
 *			Implementation des SIOX-Ontology-Interfaces
 * 
 * @authors	Michaela Zimmer
 * @date	2011
 * 			GNU Public License
 */

/* Switch off assertions unless compiled with -DDEBUG */
#define	NDEBUG
#ifdef	DEBUG
#undef	NDEBUG
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include "ontology.h"

#define MINIMUM_NAME_LENGTH	   5
#define LINE_BUFFER_SIZE	2000


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

/**
 * A list element for a simple list of metrics.
 */
struct siox_ont_node_t
{
	siox_metric				metric;	/**< The metric's actual data. */
	struct siox_ont_node_t	*next;	/**< The next list element. */
};


/*
 * Lib-local Variables
 * ===================
 */

/**
 * The current ontology file name.
 * Alsoused to determine whether we have an open ontology (!= @c NULL) or not (== @c NULL ).
 */
static const char *	current_ontology = NULL;

/**
 * The next free MID to assign to a new metric. Also, the nuber of metrics in the current ontology.
 */
static int	current_mid = 0;	

/**
 * A pointer to the head of the list of actual metrics data.
 * Simple implementation for dynamic, eventually to be replaced by something better.
 *
 * @todo Replace the list with something more sophisticated, such as a btree.
 */
static struct siox_ont_node_t *	head;


/*
 * Lib-local Function Declarations
 * ===============================
 */
 
static siox_mid		new_mid_from_id( int id );
static siox_metric	new_metric( void);
static bool			insert_into_list( siox_metric metric );


/*
 * Function Definitions
 * ====================
 */

bool
siox_ont_open_ontology( const char * file )
{
	FILE *					fh;
	char					sBuffer[LINE_BUFFER_SIZE];
	siox_metric				metric;
	int						line;
	int						id, unit, storage, scope;
	int						length;
	char 					*p, *q;
	
	assert( 1 == 2 );
	
	/* Require no open ontology */
	if( current_ontology )
		siox_ont_close_ontology();

	/* Memorise file name */
	assert( current_ontology == NULL );
	current_ontology = file;
	
	/* Initialise metrics field */
	assert( head == NULL );
	
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
					metric = new_metric();
					if ( !metric )
						return( false );
					if( 4 != sscanf( sBuffer, "%d, %d, %d, %d\n", &id, &unit, &storage, &scope) )
					{
						fprintf( stderr, "ERROR: Format error in file %s.\n", file );
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
								 length, MINIMUM_NAME_LENGTH, file );
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
				case 3:	/* Parse for empty line */
					if( !insert_into_list( metric ) )
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
				default:	/* ERROR! */
					/* This should never happen! */
					fprintf( stderr, "ERROR: Format error in file %s.\n", file );
					fclose( fh );
					current_ontology = NULL;
					return( false );
			}
		}
		
		/* Close file */
		if( fclose( fh ) == EOF )
		{
			fprintf( stderr, "ERROR: Couldn't close file %s properly.\n", file );
			current_ontology = NULL;
			return( false );
		}
	}

	return( true );
}


bool
siox_ont_write_ontology()
{
	FILE *								fh;
	char								sBuffer[LINE_BUFFER_SIZE];
	siox_metric							metric;
	int									iError;
	struct siox_ont_node_t	*current;

	/* Require an open ontology */
	if( !current_ontology )
	{
		fprintf( stderr, "ERROR: No ontology to write!\n" );
		return( false );
	}

	/* Open file */
	if( ( fh = fopen( current_ontology, "wt" ) ) != NULL )
	{
		/** @todo Write file header comment */
		
		/* Write metrics to file */
		for( current = head; current; current = current->next )
		{
			iError = 0;
			metric = current->metric;
			assert( metric != NULL );
			fprintf( stderr, "Writing to file:\n%s\n", siox_ont_metric_to_string( metric ) );
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
			fclose( fh );
			return( false );
		}
		
		/* Close file */
		if( fclose( fh ) == EOF )
		{
			fprintf( stderr, "ERROR: Could not close file %s properly.\n", current_ontology );
			return( false );
		}
	}
	else
	{
		fprintf( stderr, "ERROR: Could not open file %s for writing.\n", current_ontology );
		return( false );
	}
	
	return( true );
}


bool
siox_ont_close_ontology()
{
	struct siox_ont_node_t	*this, *next;
	
	/* Require an open ontology */
	if( !current_ontology )
	{
		fprintf( stderr, "ERROR: No ontology!\n" );
		return( false );
	}

	/* Free metrics storage */
	this = head;
	while( this )
	{
		assert( this->metric != NULL);
		next = this->next;
		siox_ont_free_metric( this->metric );
		this = next;
	}
	head = NULL;
	
	/* Forget file name */
	current_ontology = NULL;
	
	return( true );
}


siox_mid
siox_ont_find_mid_by_name( const char * name)
{
	struct siox_ont_node_t	*current;
	
	current = head;
	
	while( current )
	{
		assert( current->metric != NULL );
		if( strcmp( name, current->metric->name ) == 0 )
			return( new_mid_from_id( current->metric->id ) );
		current = current->next;
	}

	return( NULL );
}


void
siox_ont_free_mid( siox_mid mid )
{
	free( mid );

	return;
}


siox_metric
siox_ont_find_metric_by_mid( siox_mid mid )
{
	int 							id;
	struct siox_ont_node_t	*current;
	
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
	
	for( current = head; current; current = current->next )
	{
		assert( current->metric != NULL );
		if( current->metric->id == id )
			return( current->metric );
	}
	
	return( NULL );
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


siox_mid
siox_ont_metric_get_mid( siox_metric metric )
{
	if ( metric )
		return new_mid_from_id( metric->id );
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


int
siox_ont_count_metrics()
{
	struct siox_ont_node_t	*current;
	int						result;

	/* Require an open ontology */
	if( !current_ontology )
	{
		fprintf( stderr, "ERROR: No ontology!\n" );
		return( -1 );
	}

	result = 0;
	for( current = head; current; current = current->next )
		result++;
		
	return( result );
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

	mid = siox_ont_find_mid_by_name( name );
	if( mid )
	{
		fprintf( stderr, "ERROR: The unique name >%s< exists already!\n", name );
		siox_ont_free_mid( mid );
		return( NULL );
	}
	
	mid = new_mid_from_id( current_mid );
	if( !mid )
	{
		fprintf( stderr, "ERROR: Cannot register metric >%s< due to storage limitations.\n", name );
		return( NULL );
	}
	current_mid++;
	
	/* Set up new metric object */
	metric = new_metric();
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
	if( !insert_into_list( metric ) )
	{
		fprintf( stderr, "ERROR: Cannot register metric >%s< due to storage limitations.\n",
						 metric->name );
		siox_ont_free_metric( metric );
		return( NULL );
	}
	
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
new_mid_from_id( int id )
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


/**
 * Generic constructor for the metric object.
 *
 * @returns			A new metric object with fields initialised to @c NULL or
 *					@c SIOX_[UNIT|STORAGE|SCOPE]_UNASSIGNED;
 *					if no memory could be allocated, @c NULL is returned.
 */
static siox_metric
new_metric( void )
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


/**
 * Inserts a metric into the list of metrics according to ascending order of id.
 *
 * @param[in]	metric	The metric to be inserted.
 *
 * @returns				@c true if successfully inserted, otherwise @c false.
 */
static bool
insert_into_list( siox_metric metric )
{
	struct siox_ont_node_t	*last, *this, *new;
	
	assert( metric != NULL );
	
	new = malloc( sizeof( struct siox_ont_node_t ) );
	if( !new )
		return( false );
	new->metric = metric;
	
	/* Insert as only element */
	if( !head )
	{
		new->next = NULL;
		head = new;
		return( true );
	}

	assert( head->metric != NULL );
	
	/* Insert before first element */
	if( metric->id < head->metric->id )
	{
		new->next = head;
		head = new;
		return( true );
	}
	
	/* Insert between two elements */
	last = head;
	this = head->next;
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

