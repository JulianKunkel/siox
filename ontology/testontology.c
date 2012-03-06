/**
 * @file    testontology.c
 *
 * Test program for the SIOX ontology library.
 *
 * @authors Michaela Zimmer
 * @date    2012
 *          GNU Public License
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ontology.h"


/*
 * Global Definitions
 */

/** The name of the ontology file to use. */
#define ONTOLOGY "test"


/*
 * Global Variables
 */

/* None yet... */

/*
 * Function Prototypes
 */

int failure( const char * message );
int success( const char * message );

int test1( void );
int test2( void );
int test3( void );
int test4( void );
int test5( void );
int test6( void );
int test7( void );




/**
 * Die Hauptroutine mit den verschiedenen Testaufrufen.
 * Etwaige Fehlermeldungen werden in die Datei testontology.log geschrieben.
 *
 * @returns EXIT_SUCCESS, falls die Tests erfolgreich waren; EXIT_FAILURE sonst.
 */
int main()
{
    int failures = 0;

    /* Reroute stderr to log file */
    /* fopen( "testontology.log", "wt", stderr ); */

    failures += test1();
    failures += test2();
    failures += test3();
    failures += test4();
    failures += test5();
    failures += test6();
    failures += test7();

    fclose( stderr );

    if( failures > 0 )
    {
        fprintf( stdout, "SIOX ontology failed %d tests!\n", failures );
        exit( EXIT_FAILURE );
    }
    else
    {
        fprintf( stdout, "SIOX ontology passed all tests!\n" );
        exit( EXIT_SUCCESS );
    }
}



/**
 * Test 1:
 * @test
 *  <ol>
 *      <li> Leere Ontologie öffnen
 *      <li> Ontologie schließen
 *  </ol>
 *
 * @returns Bei Erfolg 0, sonst 1.
 */
int test1( void )
{
    fprintf( stdout, "==> Test  1..." );

    /* Datei sicherheitshalber löschen. */
    siox_ont_remove_ontology( ONTOLOGY );

    if( !siox_ont_open_ontology( ONTOLOGY ) )
        return( failure( "FAILED Stage 1 - opening empty ontology!\n" ) );

    if(! siox_ont_close_ontology() )
        return( failure( "FAILED Stage 2 - closing empty ontology!\n" ) );

    return( success( "passed.\n" ) );
}

/**
 * Test 2:
 * @test
 * Fehler provozieren:
 * <ol>
 * <li> Ontologie schließen ohne zu öffnen
 * </ol>
 *
 * @returns Bei Erfolg 0, sonst 1.
 */
int test2( void)
{
    fprintf( stdout, "==> Test  2..." );

    if( siox_ont_close_ontology() )
        return( failure( "FAILED Stage 1 - closing an unopened ontology!\n" ) );
    else
        fprintf( stderr, "...but this is as it should be. :)\n\n" );

    return( success( "passed.\n" ) );
}

/**
 * Test 3:
 * @test
 * Fehler provozieren:
 * <ol>
 * <li> Anzahl der Metriken erfragen, ohne Ontologie zu öffnen
 * </ol>
 *
 * @returns Bei Erfolg 0, sonst 1.
 */
int test3( void )
{
    fprintf( stdout, "==> Test  3..." );

    if( siox_ont_count_metrics() > -1)
        return( failure( "FAILED Stage 1 - counting metrics in an unopened ontology!\n" ) );
    else
        fprintf( stderr, "...but this is as it should be. :)\n\n" );

    return( success( "passed.\n" ) );
}

/**
 * Test 4:
 * @test
 * <ol>
 * <li> Ontologie öffnen
 * <li> Metrik erzeugen
 * <li> Anzahl der Metriken prüfen
 * <li> Ontologie schreiben
 * <li> Ontologie schließen
 * </ol>
 *
 * @returns Bei Erfolg 0, sonst 1.
 */
int test4( void )
{
    char                        name1[] = "Metrik für Test 4";
    char                        description1[] = "Testmetrik für Test 4.";
    enum siox_ont_unit_type     unit1 = SIOX_UNIT_SECONDS;
    enum siox_ont_storage_type  storage1 = SIOX_STORAGE_FLOAT;
    enum siox_ont_scope_type    scope1 = SIOX_SCOPE_SUM;

    siox_mid                    mid;


    fprintf( stdout, "==> Test  4..." );

    /* Datei sicherheitshalber löschen. */
    siox_ont_remove_ontology( ONTOLOGY );

    if( !siox_ont_open_ontology( ONTOLOGY ) )
        return( failure( "FAILED Stage 1 - opening empty ontology!\n" ) );

    if( !(mid = siox_ont_register_metric( name1, description1, unit1, storage1, scope1 ) ) )
        return( failure( "FAILED Stage 2 - registering metric!\n" ) );

    if( siox_ont_count_metrics() != 1 )
        return( failure( "FAILED Stage 3 - counting metrics!\n" ) );

    if( !siox_ont_write_ontology() )
        return( failure( "FAILED Stage 4 - writing ontology!\n" ) );

    if( !siox_ont_close_ontology() )
        return( failure( "FAILED Stage 5 - closing ontology!\n" ) );

    siox_ont_free_mid( mid );

    siox_ont_remove_ontology( ONTOLOGY );

    return( success( "passed.\n" ) );
}

/**
 * Test 5:
 * @test
 * <ol>
 * <li> Ontologie öffnen
 * <li> Metrik erzeugen
 * <li> Anzahl der Metriken prüfen
 * <li> Metrik aus MID holen
 * <li> Metrikelemente auf korrekten Inhalt prüfen
 * <li> Ontologie schließen
 * </ol>
 *
 * @returns Bei Erfolg 0, sonst 1.
 */
int test5( void )
{
    char                        name1[] = "Metrik für Test 5";
    char                        description1[] = "Testmetrik für das Programm testontology, Test 5.";
    enum siox_ont_unit_type     unit1 = SIOX_UNIT_BYTES;
    enum siox_ont_storage_type  storage1 = SIOX_STORAGE_32_BIT_INTEGER;
    enum siox_ont_scope_type    scope1 = SIOX_SCOPE_MAXIMUM;

    siox_mid                    mid;
    siox_metric                 metric;

    fprintf( stdout, "==> Test  5..." );

    /* Datei sicherheitshalber löschen. */
    siox_ont_remove_ontology( ONTOLOGY );

    if( !siox_ont_open_ontology( ONTOLOGY ) )
        return( failure( "FAILED Stage 1 - opening empty ontology!\n" ) );

    if( !(mid = siox_ont_register_metric( name1, description1, unit1, storage1, scope1 ) ) )
        return( failure( "FAILED Stage 2 - registering metric!\n" ) );

    if( siox_ont_count_metrics() != 1 )
        return( failure( "FAILED Stage 3 - counting metrics!\n" ) );

    if( !(metric = siox_ont_find_metric_by_mid( mid )) )
        return( failure( "FAILED Stage 4 - getting metric from mid!\n" ) );

    if( !siox_ont_mid_is_equal( mid, siox_ont_metric_get_mid( metric ) ) )
        return( failure( "FAILED Stage 5 - comparing ids!\n" ) );

    if( strcmp( name1, siox_ont_metric_get_name( metric ) ) != 0 )
        return( failure( "FAILED Stage 6 - comparing names!\n" ) );

    if( strcmp( description1, siox_ont_metric_get_description( metric ) ) != 0 )
        return( failure( "FAILED Stage 7 - comparing descriptions!\n" ) );

    if( unit1 != siox_ont_metric_get_unit( metric ) )
        return( failure( "FAILED Stage 8 - comparing units!\n" ) );

    if( storage1 != siox_ont_metric_get_storage( metric ) )
        return( failure( "FAILED Stage 9 - comparing storages!\n" ) );

    if( scope1 != siox_ont_metric_get_scope( metric ) )
        return( failure( "FAILED Stage 10 - comparing scopes!\n" ) );

    if( !siox_ont_close_ontology() )
        return( failure( "FAILED Stage 11 - closing ontology!\n" ) );

    siox_ont_free_mid( mid );

    siox_ont_remove_ontology( ONTOLOGY );

    return( success( "passed.\n" ) );
}


/**
 * Test 6:
 * @test
 * <ol>
 * <li> Ontologie öffnen
 * <li> Metriken erzeugen
 * <li> Anzahl der Metriken prüfen
 * <li> Ontologie schreiben
 * <li> Ontologie schließen
 * <li> Ontologie öffnen
 * <li> Anzahl der Metriken prüfen
 * <li> Metriken suchen
 * <li> Gefundene Metriken auf identische IDs wie beim Erzeugen prüfen
 * <li> Ontologie schließen
 * </ol>
 *
 * @returns Bei Erfolg 0, sonst 1.
 */
int test6( void )
{
    char                        name1[] = "Metrik 1 für Test 6";
    char                        description1[] = "Testmetrik 1 für das Programm testontology, Test 6.";
    enum siox_ont_unit_type     unit1 = SIOX_UNIT_UNITS;
    enum siox_ont_storage_type  storage1 = SIOX_STORAGE_DOUBLE;
    enum siox_ont_scope_type    scope1 = SIOX_SCOPE_SAMPLE;

    char                        name2[] = "Metrik 2 für Test 6";
    char                        description2[] = "Testmetrik 2 für das Programm testontology, Test 6.";
    enum siox_ont_unit_type     unit2 = SIOX_UNIT_SECONDS;
    enum siox_ont_storage_type  storage2 = SIOX_STORAGE_64_BIT_INTEGER;
    enum siox_ont_scope_type    scope2 = SIOX_SCOPE_DIFFERENCE;

    char                        name3[] = "Metrik 3 für Test 6";
    char                        description3[] = "Testmetrik 3 für das Programm testontology, Test 6.";
    enum siox_ont_unit_type     unit3 = SIOX_UNIT_FLOPS;
    enum siox_ont_storage_type  storage3 = SIOX_STORAGE_STRING;
    enum siox_ont_scope_type    scope3 = SIOX_SCOPE_MEDIAN;

    siox_mid                    mid1, mid2, mid3;

    fprintf( stdout, "==> Test  6..." );

    /* Datei sicherheitshalber löschen. */
    siox_ont_remove_ontology( ONTOLOGY );

    if( !siox_ont_open_ontology( ONTOLOGY ) )
        return( failure( "FAILED Stage 1 - opening ontology!\n" ) );

    mid1 = siox_ont_register_metric( name1, description1, unit1, storage1, scope1 );
    mid2 = siox_ont_register_metric( name2, description2, unit2, storage2, scope2 );
    mid3 = siox_ont_register_metric( name3, description3, unit3, storage3, scope3 );
    if(    !mid1 || !mid2 || !mid3 )
        return( failure( "FAILED Stage 2 - registering metrics!\n" ) );

    if( siox_ont_count_metrics() != 3 )
        return( failure( "FAILED Stage 3 - counting metrics!\n" ) );

    if( !siox_ont_write_ontology() )
        return( failure( "FAILED Stage 4 - writing ontology!\n" ) );

    if( !siox_ont_close_ontology() )
        return( failure( "FAILED Stage 5 - closing ontology!\n" ) );

    if( !siox_ont_open_ontology( ONTOLOGY ) )
        return( failure( "FAILED Stage 6 - reopening ontology!\n" ) );

    if( siox_ont_count_metrics() != 3 )
        return( failure( "FAILED Stage 7 - counting metrics!\n" ) );

    if(    !siox_ont_mid_is_equal( mid1, siox_ont_find_mid_by_name( name1 ) )
        || !siox_ont_mid_is_equal( mid2, siox_ont_find_mid_by_name( name2 ) )
        || !siox_ont_mid_is_equal( mid3, siox_ont_find_mid_by_name( name3 ) ) )
        return( failure( "FAILED Stage 8 - looking up metrics, checking for consistent MIDs!\n" ) );

    if( !siox_ont_close_ontology() )
        return( failure( "FAILED Stage 9 - closing ontology!\n" ) );

    siox_ont_free_mid( mid1 );
    siox_ont_free_mid( mid2 );
    siox_ont_free_mid( mid3 );

    siox_ont_remove_ontology( ONTOLOGY );

    return( success( "passed.\n" ) );
}


/**
 * Test 7:
 * @test
 * Fehler provozieren:
 * <ol>
 * <li> Ontologie öffnen
 * <li> Metrik erzeugen
 * <li> Metrik gleichen Namens erneut erzeugen
 * <li> Ontologie schließen
 * </ol>
 *
 * @returns Bei Erfolg 0, sonst 1.
 */
int test7( void )
{
    char                        name1[] = "Metrik für Test 7";
    char                        description1[] = "Testmetrik für das Programm testontology, Test 7.";
    enum siox_ont_unit_type     unit1 = SIOX_UNIT_FLOPS;
    enum siox_ont_storage_type  storage1 = SIOX_STORAGE_64_BIT_INTEGER;
    enum siox_ont_scope_type    scope1 = SIOX_SCOPE_MINIMUM;

    siox_mid                    mid;

    fprintf( stdout, "==> Test  7..." );

    /* Datei sicherheitshalber löschen. */
    siox_ont_remove_ontology( ONTOLOGY );

    if( !siox_ont_open_ontology( ONTOLOGY ) )
        return( failure( "FAILED Stage 1 - opening empty ontology!\n" ) );

    if( !(mid = siox_ont_register_metric( name1, description1, unit1, storage1, scope1 ) ) )
        return( failure( "FAILED Stage 2 - registering metric!\n" ) );

    if( (mid = siox_ont_register_metric( name1, description1, unit1, storage1, scope1 ) ) )
        return( failure( "FAILED Stage 3 - registering existing metric again!\n" ) );
    else
        fprintf( stderr, "...but this is as it should be. :)\n\n" );

    if( !siox_ont_close_ontology() )
        return( failure( "FAILED Stage 4 - closing ontology!\n" ) );

    siox_ont_free_mid( mid );

    return( success( "passed.\n" ) );
}


/**
 * Prints message and returns error code.
 *
 * Pure convenience function to make tests look cleaner.
 *
 * @param[in]   message The message to print.
 *
 * @returns             Always returns the error code 1.
 */
int failure( const char * message )
{
    fprintf( stdout, "%s", message );
    return( 1 );
}

/**
 * Prints message and returns error code.
 *
 * Pure convenience function to make tests look cleaner.
 *
 * @param[in]   message The message to print.
 *
 * @returns             Always returns the error code 0.
 */
int success( const char * message )
{
    fprintf( stdout, "%s", message );
    return( 0 );
}

