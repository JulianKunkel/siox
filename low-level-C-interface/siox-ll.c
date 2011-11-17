/**
 * @file siox-ll.c
 * Implementation des SIOX-Low-Level-Interfaces.
 *
 * 
 * \mainpage Projekt SIOX - Das Low-Level Interface
 *
 * \section intro_sec Einführung
 * siox-ll wurde aus den Rahmenbedingungen entwickelt, die die Abstraktion
 * des I/O-Pfadmodell @em IOPm an das System stellt.
 *
 * @authors	Julian Kunkel & Michaela Zimmer
 * @date	2011
 * 			GNU Public License.
 */


#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "siox-ll.h"


	
/** The next unassigned UNID. */
static unsigned long int	current_unid = 0L;

struct siox_unid_t {
	unsigned long int	id;	/**< The actual ID. */
	};
	
	
/** The next unassigned AID. */
static unsigned long int	current_aid = 0L;

struct siox_aid_t {
	unsigned long int	id;	/**< The actual ID. */
	};


/** The next unassigned DMID. */
static unsigned long int	current_dmid = 0L;

struct siox_dmid_t {
	unsigned long int	id;	/**< The actual ID. */
	};




siox_unid
siox_register_node( const char * hwid,
					const char * swid,
					const char * iid)
{
	/* Draw fresh UNID */
	siox_unid unid = malloc( sizeof( struct siox_unid_t ) );
	(*unid).id = current_unid++;
	
	
	printf( "Node registered as UNID %ld with hwid >%s<, swid >%s< and iid >%s<.\n",
		(*unid).id, hwid, swid, iid );
	
	return( unid );
}


void
siox_unregister_node( siox_unid	unid )
{
	printf( "UNID %ld unregistered.\n", (*unid).id );
}


void
siox_register_attribute( siox_unid				unid,
						 const char *			key,
						 enum siox_value_type	value_type,
						 void *					value )
{
	printf( "UNID %ld registered the following additional attributes:\n",
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
siox_register_edge( siox_unid		unid,
					const char *	child_swid )
{
	printf( "UNID %ld registered edge to child node >%s<.\n",	(*unid).id, child_swid );
}


siox_dmid
siox_register_descriptor_map( siox_unid		unid,
							  const char *	source_descriptor_type,
							  const char *	target_descriptor_type)
{
	/* Draw fresh DMID */
	siox_dmid dmid = malloc( sizeof( struct siox_dmid_t ) );
	(*dmid).id = current_dmid++;
	
	
	printf( "UNID %ld registered DMID %ld: %s -> %s.\n",
		(*unid).id, (*dmid).id, source_descriptor_type, target_descriptor_type );
	
	return( dmid );
}


void
siox_create_descriptor( siox_unid		unid,
						const char *	descriptor_type,
						const char *	descriptor )
{
	printf( "UNID %ld created descriptor >%s< of type >%s<.\n",
		(*unid).id, descriptor, descriptor_type );
}


void
siox_send_descriptor( siox_unid		unid,
					  const char *	child_swid,
					  const char *	descriptor_type,
					  const char *	descriptor )
{
	printf( "UNID %ld sent descriptor >%s< of type >%s< to child node %s.\n",
		(*unid).id, descriptor, descriptor_type, child_swid );
}


void
siox_receive_descriptor( siox_unid		unid,
						 const char *	descriptor_type,
						 const char *	descriptor )
{
	printf( "UNID %ld received descriptor >%s< of type >%s<.\n",
		(*unid).id, descriptor, descriptor_type );
}


void
siox_map_descriptor( siox_unid		unid,
					 siox_dmid		dmid,
					 const char *	source_descriptor,
					 const char *	target_descriptor )
{
	printf( "UNID %ld applied DMID %ld, mapping %s to %s.\n",
		(*unid).id, (*dmid).id, source_descriptor, target_descriptor );
}


void
siox_release_descriptor( siox_unid		unid,
						 const char *	descriptor_type,
						 const char *	descriptor )
{
	printf( "UNID %ld released descriptor >%s< of type >%s<.\n",
		(*unid).id, descriptor, descriptor_type );
}


siox_aid
siox_start_activity( siox_unid	unid )
{
	/* Draw timestamp */
	time_t 	timeStamp = time(NULL);
	
	/* Draw fresh AID */
	siox_aid aid = malloc( sizeof( struct siox_aid_t ) );
	(*aid).id = current_aid++;
	
	
	printf( "UNID %ld started AID %ld at %s",
		(*unid).id, (*aid).id, ctime( &timeStamp ));
	
	return( aid );
}


void
siox_stop_activity( siox_aid	aid )
{
	/* Draw timestamp */
	time_t	timeStamp = time(NULL);


	printf( "AID %ld stopped at %s",
		(*aid).id, ctime( &timeStamp ));
}


void
siox_report_activity( siox_aid aid,
					  const char * 			descriptor_type,
					  const char * 			descriptor,
					  const char *  		measure,
					  enum siox_value_type	value_type,
					  void * 				value,
					  const char * 			details )
{
	printf( "AID %ld, identified by the %s of %s, was measured as follows:\n",
		(*aid).id, descriptor_type, descriptor );
	printf( "\t%s:\t", measure );
	switch ( value_type ){
		case SIOX_TYPE_INTEGER:
			printf( "%d", *((int*) value) );
			break;
		case SIOX_TYPE_LONG:
			printf( "%ld", *((long*) value) );
			break;
		case SIOX_TYPE_FLOAT:
			printf( "%f", *((float*) value) );
			break;
		case SIOX_TYPE_STRING:
			printf( "%s", (char*) value );
			break;
	}
	if (details != NULL)
		printf( "\tNote:\t%s\n", details );
}


void
siox_end_activity ( siox_aid	aid )
{
	/* Draw timestamp */
	time_t	timeStamp = time(NULL);


	printf( "AID %ld finally ended at %s",
		(*aid).id, ctime( &timeStamp ));
}


void
siox_report( siox_unid				unid,
			 const char *  			measure,
			 enum siox_value_type	value_type,
			 void * 				value,
			 const char *			details )
{
	printf( "UNID %ld was measured as follows:\n",
		(*unid).id );
	printf( "\t%s:\t", measure );
	switch ( value_type ){
		case SIOX_TYPE_INTEGER:
			printf( "%d", *((int*) value) );
			break;
		case SIOX_TYPE_LONG:
			printf( "%ld", *((long*) value) );
			break;
		case SIOX_TYPE_FLOAT:
			printf( "%f", *((float*) value) );
			break;
		case SIOX_TYPE_STRING:
			printf( "%s", (char*) value );
			break;
	}
	if (details != NULL)
		printf( "\tNote:\t%s\n", details );

}


