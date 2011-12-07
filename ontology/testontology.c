/**
 * @file	testontology.c
 *
 * Test program for the SIOX ontology library.
 *
 * @authors	Michaela Zimmer
 * @date	2011
 *			GNU Public License
 */
 
 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ontology.h"


/*
 * Global Definitions
 */

/** The name of the ontology file to use. */
#define ONTOLOGY "testontology.ont"


/*
 * Global Variables
 */

/** Test metric 1: name */
char						name1[] = "Metrik 1";
/** Test metric 1: description */
char						description1[] = "Testmetrik für das Programm testontology.";
/** Test metric 1: unit type */
enum siox_ont_unit_type		unit1 = SIOX_UNIT_SECONDS;
/** Test metric 1: storage type */
enum siox_ont_storage_type	storage1 = SIOX_STORAGE_FLOAT;
/** Test metric 1: scope type */
enum siox_ont_scope_type	scope1 = SIOX_SCOPE_SUM;


/*
 * Function Prototypes
 */

int test1( void );
int test2( void );
int test3( void );
int test4( void );
int test5( void );




/**
 * Die Hauptroutine mit den verschiedenen Testaufrufen.
 * Etwaige Fehlermeldungen werden in die Datei testontology.log geschrieben.
 *
 * @returns	EXIT_SUCCESS, falls die Tests erfolgreich waren; EXIT_FAILURE sonst.
 */
int main()
{
	int failures = 0;
	
	/* Reroute stderr to log file */
	freopen( "testontology.log", "wt", stderr );
	
	failures += test1();
	failures += test2();
	failures += test3();
	failures += test4();
	failures += test5();
	
	fclose( stderr );
	
	if( failures > 0 )
	{
		printf( "SIOX ontology failed %d tests!\n", failures );
		exit( EXIT_FAILURE );
	}
	else
	{
		printf( "SIOX ontology passed all tests!\n" );
		exit( EXIT_SUCCESS );
	}
}



/**
 * Test 1:
 * <ol>
 * <li> Leere Ontologie öffnen
 * <li> Ontologie schließen
 * </ol>
 *
 * @returns	Bei Erfolg 0, sonst 1.
 */
int test1( void )
{
	printf( "==> Test  1..." );

	/* Datei sicherheitshalber löschen. */
	remove( ONTOLOGY );
	
	if( siox_ont_open_ontology( ONTOLOGY ) < 0)
	{
		printf( "FAILED Stage 1!\n" );
		return( 1 );
	}
		
	if( siox_ont_close_ontology() < 0 )
	{
		printf( "FAILED Stage 2!\n" );
		return( 1 );
	}

	printf( "passed.\n" );
	return( 0 );
}

/**
 * Test 2:
 * Fehler provozieren:
 * <ol>
 * <li> Ontologie schließen ohne zu öffnen
 * </ol>
 *
 * @returns	Bei Erfolg 0, sonst 1.
 */
int test2( void)
{
	printf( "==> Test  2..." );

	if( siox_ont_close_ontology() >-1 )
	{
		printf( "FAILED!\n" );
		return( 1 );
	}
	else
		fprintf( stderr, "...but this is as it should be. :)\n" );
	
	printf( "passed.\n" );
	return( 0 );
}

/**
 * Test 3:
 * Fehler provozieren:
 * <ol>
 * <li> Anzahl der Metriken erfragen, ohne Ontologie zu öffnen
 * </ol>
 *
 * @returns	Bei Erfolg 0, sonst 1.
 */
int test3( void )
{
	printf( "==> Test  3..." );

	if( siox_ont_count_metrics() > -1)
	{
		printf( "FAILED!\n" );
		return( 1 );
	}
	else
		fprintf( stderr, "...but this is as it should be. :)\n" );
		

	printf( "passed.\n" );
	return( 0 );
}

/**
 * Test 4:
 * <ol>
 * <li> Ontologie öffnen
 * <li> Metrik erzeugen
 * <li> Anzahl der Metriken prüfen
 * <li> Ontologie schreiben
 * <li> Ontologie schließen
 * </ol>
 *
 * @returns	Bei Erfolg 0, sonst 1.
 */
int test4( void )
{
	siox_mid	mid;
	
	printf( "==> Test  4..." );
	
	/* Datei sicherheitshalber löschen. */
	remove( ONTOLOGY );
	
	if( siox_ont_open_ontology( ONTOLOGY ) < 0)
	{
		printf( "FAILED Stage 1!\n" );
		return( 1 );
	}

	if( !(mid = siox_ont_register_metric( name1, description1, unit1, storage1, scope1 ) ) )
	{
		printf( "FAILED Stage 2!\n" );
		return( 1 );
	}

	if( siox_ont_count_metrics() != 1 )
	{
		printf( "FAILED Stage 3!\n" );
		return( 1 );
	}

	if( siox_ont_write_ontology() < 0 )
	{
		printf( "FAILED Stage 4!\n" );
		return( 1 );
	}

	if( siox_ont_close_ontology() < 0 )
	{
		printf( "FAILED Stage 5!\n" );
		return( 1 );
	}

	printf( "passed.\n" );
	remove( ONTOLOGY );
	return( 0 );
}

/**
 * Test 5:
 * <ol>
 * <li> Ontologie öffnen
 * <li> Metrik erzeugen
 * <li> Anzahl der Metriken prüfen
 * <li> Ontologie schreiben
 * <li> Ontologie schließen
 * <li> Ontologie öffnen
 * <li> Anzahl der Metriken prüfen
 * <li> Metrik suchen
 * <li> Gefundene Metrik auf Übereinstimmung prüfen
 * <li> Ontologie schließen
 * </ol>
 *
 * @returns	Bei Erfolg 0, sonst 1.
 */
int test5( void )
{
	siox_mid	mid;
	int			nLength;
	char *		sBuffer;
	
	printf( "==> Test  5..." );
	
	/* Datei sicherheitshalber löschen. */
	remove( ONTOLOGY );
	
	if( siox_ont_open_ontology( ONTOLOGY ) != 1)
	{
		printf( "FAILED Stage 1!\n" );
		return( 1 );
	}

	if( !(mid = siox_ont_register_metric( name1, description1, unit1, storage1, scope1 ) ) )
	{
		printf( "FAILED Stage 2!\n" );
		return( 1 );
	}

	if( siox_ont_count_metrics() != 1 )
	{
		printf( "FAILED Stage 3!\n" );
		return( 1 );
	}

	if( siox_ont_write_ontology() < 0 )
	{
		printf( "FAILED Stage 4!\n" );
		return( 1 );
	}

	if( siox_ont_close_ontology() < 0 )
	{
		printf( "FAILED Stage 5!\n" );
		return( 1 );
	}
	
	if( siox_ont_open_ontology( ONTOLOGY ) != 0)
	{
		printf( "FAILED Stage 6!\n" );
		return( 1 );
	}
		
	if( siox_ont_count_metrics() != 1 )
	{
		printf( "FAILED Stage 7!\n" );
		return( 1 );
	}
	
	if( !( mid = siox_ont_find_metric( name1 ) ) )
	{
		printf( "FAILED Stage 8!\n" );
		return( 1 );
	}

	/* Compare other data fields besides name with stored data. */
	nLength = strlen( name1 ) + strlen( description1 ) + 50;
	sBuffer = malloc( sizeof( char[nLength] ) );
	if ( !sBuffer )
	{
		printf( "Memory allocation error in siox_ont_metric_to_string()!" );
		return( 1 );
	}
	sprintf( sBuffer, "Metric:\t%s\n"
					  "%s\n"
					  "Unit:\t%u\n"
					  "Storage:\t%u\n"
					  "Scope:\t%u\n",
					  name1,
					  description1,
					  unit1,
					  storage1,
					  scope1 );
	if( strcmp( sBuffer, siox_ont_metric_to_string( siox_ont_get_metric( mid ) ) ) )
	{
		printf( "FAILED Stage 9!\n" );
		return( 1 );
	}

	if( siox_ont_close_ontology() < 0 )
	{
		printf( "FAILED Stage 10!\n" );
		return( 1 );
	}

	printf( "passed.\n" );
	remove( ONTOLOGY );
	return( 0 );
}


