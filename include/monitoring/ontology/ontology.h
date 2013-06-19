/**
 * @file    ontology.h
 *          Headerdatei für das SIOX-Ontology-Interface
 *
 * @authors Michaela Zimmer, Julian Kunkel & Marc Wiedemann
 * @date    2012
 *          GNU Public License
 */


#ifndef siox_ONT_H
#define siox_ONT_H


#include <stdbool.h>
#include <monitoring/ontology/enums.h>
#include <monitoring/ontology/metric.h>
// #include "datatype.h"
#include <monitoring/ontology/attribute.h>


/**
 * An ontology, as represented in SIOX.
 */
typedef struct siox_ontology siox_ontology;



/**
 * @name Opening, Writing, Closing and Removing the Ontology
 */
/**@{*/

/**
 * Liest eine Ontologie aus einer Datei.
 *
 * @param   file    Der Basisname der Dateien, aus denen die Ontologie gelesen werden soll.
 *                  Er wird jeweils um passende Endungen ergänzt.
 *                  Existieren die Dateien noch nicht, wird eine neue (leere)
 *                  Ontologie angelegt.
 *
 * @returns         @c true bei Erfolg, sonst @c false.
 */
bool siox_ont_open_ontology( const char * file );

/**
 * Schreibt den aktuellen Stand der Ontologie in die bei siox_ont_open_ontology()
 * angegebene Datei.
 * Der bisherige Inhalt wird dabei überschrieben!
 *
 * @returns         @c true bei Erfolg, sonst @c false.
 */
bool siox_ont_write_ontology();

/**
 * Schließt die Ontologie wieder.
 *
 * Belegte Ressourcen werden freigegeben.
 * Eventuelle Änderungen an der Ontologie schreibt diese Funktion <em>nicht</em>
 * in die Datei zurück; dazu muß siox_ont_write_ontology() aufgerufen werden!
 *
 * @returns         @c true bei Erfolg, sonst @c false.
 */
bool siox_ont_close_ontology();

/**
 * Löscht die Ontologie dauerhaft.
 *
 * Belegte Ressourcen werden freigegeben.
 *
 * @param   file    Der Basisname der Dateien, aus denen die zu löschende Ontologie
 *                  besteht. Er wird jeweils um passende Endungen ergänzt.
 *
 * @returns         @c true bei Erfolg, sonst @c false.
 */
bool siox_ont_remove_ontology( const char * file );

/**@}*/




/**
 * @name Accessing the DataType Part of the Ontology
 */
/**@{*/

/**
 * Find the @em DTID for the data type with the specifications given.
 * If it already exists in the ontology, return its DTID; otherwise, create it and return the fresh DTID.
 *
 * @param[in]   name        The data type's unique name.
 * @param[in]   storage     The minimum storage type required to store data of the data type.
 *
 * @returns                 The @em DTID of the descriptor type.
 */
/*siox_dtid siox_ont_register_datatype( const char * name, enum siox_ont_storage_type storage );
*/

/**
 * Find the @em DTID for the datatype with the exact name given.
 *
 * @param[in]   name    The unique name to search for.
 *
 * @returns             The @em DTID of the datatype with the name given or @c NULL, if no exact match was found.
 */
/*siox_dtid siox_ont_find_dtid_by_name( const char * name);*/


/**
 * Retrieve the datatype with the @em DTID given.
 *
 * @param[in]   dtid     The @em DTID of the datatype.
 *
 * @returns             If the @e DTID exists, a copy of the datatype's data; otherwise @c NULL.
 */
/*siox_datatype siox_ont_find_datatype_by_dtid( siox_dtid dtid );*/


/**
 * Turn data typed by a @em DTID into a human-readable string.
 *
 * @param[in]   dtid    The @em DTID of the datatype describing the value.
 * @param[in]   value   A pointer to the actual value.
 *
 * @returns     A human-readable string representation of the the value.
 *
 * @note		Strings to be passed as values must be defined as @c char*, @em not
 * 				as @c char[], or a segmentation fault may result!!!
 */
/*const char * siox_ont_data_to_string( siox_dtid dtid, const void * value );*/


/**
 * Count the number of datatypes in the ontology.
 *
 * @returns     The number of datatypes.
 */
/*int siox_ont_count_datatypes();*/


/**@}*/



/**
 * @name Accessing the Metrics Part of the Ontology
 */
/**@{*/

/**
 * Fügt eine neue Metrik in die Ontologie ein.
 *
 * @param[in]   name        Der Name der Metrik. Er muß eindeutig sein.
 * @param[in]   description Eine textuelle Beschreibung der Metrik.
 * @param[in]   unit        Die Einheit, in welcher die Daten gemessen werden.
 * @param[in]   storage     Der minimale zum Speichern der Daten nötige Datentyp.
 * @param[in]   scope       Der zeitliche Bereich, in welchem die Daten angefallen sind.
 *
 * @returns             Eine <em>Metric ID</em>.
 */
siox_mid siox_ont_register_metric( const char *                 name,
                                   const char *                 description,
                                   enum siox_ont_unit_type      unit,
                                   enum siox_ont_storage_type   storage,
                                   enum siox_ont_scope_type     scope );


/**
 * Find the @em MID for the metric with the exact name given.
 *
 * @param[in]   name    The unique name to search for.
 *
 * @returns             The @em MID of the metric with the name given or @c NULL, if no exact match was found.
 */
/*siox_mid siox_ont_find_mid_by_name( const char * name);*/

/**
 * Retrieve the metric with the @em MID given.
 *
 * @param[in]   mid     The @em MID of the metric.
 *
 * @returns             If the @e MID exists, a copy of the metric's data; otherwise @c NULL.
 */
/*siox_metric siox_ont_find_metric_by_mid( siox_mid mid );*/

/**
 * Turn data typed by a @em MID into a human-readable string.
 *
 * @param[in]   mid     The @em MID of the metric describing the value.
 * @param[in]   value   A pointer to the actual value.
 *
 * @returns     A human-readable string representation of the the value.
 *
 * @note		Strings to be passed as values must be defined as @c char*, @em not
 * 				as @c char[], or a segmentation fault may result!!!
 */
/*const char * siox_ont_metric_data_to_string( siox_mid mid, const void * value );*/


/**
 * Count the number of metrics in the ontology.
 *
 * @returns     The number of metrics.
 */
int siox_ont_count_metrics();

/**@}*/

#endif
