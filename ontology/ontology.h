/**
 * @file    ontology.h
 *          Headerdatei für das SIOX-Ontology-Interface
 *
 * @authors Michaela Zimmer, Julian Kunkel & Marc Wiedemann
 * @date    2011
 *          GNU Public License
 */


#ifndef siox_ONT_H
#define siox_ONT_H


#include <stdbool.h>


/**
 * Die <em>Data Type ID</em>.
 *
 * Identifiziert einen Semantischen Datentyp in der Ontologie.
 */
typedef struct siox_dtid_t * siox_dtid;


/**
 * Die <em>Metric ID</em>.
 *
 * Identifiziert eine Metrik in der Ontologie.
 */
typedef struct siox_mid_t * siox_mid;


/**
 * Die Metrik mit allen Attributen.
 *
 * Enthält die Beschreibung einer Metrik mit allen zugehörigen Eigenschaften und Attributen.
 */
typedef struct siox_metric_t * siox_metric;


/**
 * Mögliche Werte für das Attribut "Unit" der Metrik.
 */
enum siox_ont_unit_type{
    SIOX_UNIT_UNASSIGNED,
    SIOX_UNIT_SECONDS,
    SIOX_UNIT_BYTES,
    SIOX_UNIT_FLOPS,
    SIOX_UNIT_UNITS
};


/**
 * Mögliche Werte für das Attribut "Minimal Storage Type" der Metrik.
 */
enum siox_ont_storage_type{
    SIOX_STORAGE_UNASSIGNED,
    SIOX_STORAGE_32_BIT_INTEGER,
    SIOX_STORAGE_64_BIT_INTEGER,
    SIOX_STORAGE_FLOAT,
    SIOX_STORAGE_DOUBLE,
    SIOX_STORAGE_STRING
};


/**
 * Mögliche Werte für das Attribut "Data Scope" der Metrik.
 */
enum siox_ont_scope_type{
    SIOX_SCOPE_UNASSIGNED,
    SIOX_SCOPE_SAMPLE,
    SIOX_SCOPE_AVERAGE,
    SIOX_SCOPE_DIFFERENCE,
    SIOX_SCOPE_MAXIMUM,
    SIOX_SCOPE_MINIMUM,
    SIOX_SCOPE_MEDIAN,
    SIOX_SCOPE_SUM
};



/**
 * @name Opening, Writing and Closing the Ontology
 */
/**@{*/

/**
 * Liest eine Ontologie aus einer Datei.
 *
 * @param   file    Der Name der Datei, aus der die Ontologie gelesen werden soll.
 *                  Existiert die Datei noch nicht, wird eine neue (leere) Ontologie angelegt.
 *
 * @returns         @c true bei Erfolg, sonst @c false.
 */
bool siox_ont_open_ontology( const char * file );

/**
 * Schreibt den aktuellen Stand der Ontologie in die bei siox_ont_open_ontology() angegebene Datei.
 * Der bisherige Inhalt wird dabei überschrieben!
 *
 * @returns         @c true bei Erfolg, sonst @c false.
 */
bool siox_ont_write_ontology();

/**
 * Schließt die Ontologie wieder.
 *
 * Belegte Ressourcen werden freigegeben.
 * Eventuelle Änderungen an der Ontologie schreibt diese Funktion <em>nicht</em> in die Datei zurück;
 * dazu muß siox_ont_write_ontology() aufgerufen werden!
 *
 * @returns         @c true bei Erfolg, sonst @c false.
 */
bool siox_ont_close_ontology();

/**@}*/



/**
 * @name Functions for the @em DTID object @em siox_dtid
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
siox_dtid siox_ont_register_datatype( const char * name, enum siox_ont_storage_type storage );

/**
 * Turn the DTID into a human-readable string.
 *
 * @param[in]   dtid    The @em DTID object.
 *
 * @returns             If the metric exists, a multi-line string representing the data; otherwise @c NULL.
 */
const char* siox_ont_dtid_to_string( siox_dtid dtid );

/**@}*/



/**
 * @name Functions for the @em MID object @em siox_mid
 */
/**@{*/

/**
 * Find the @em MID for the metric with the exact name given.
 *
 * @param[in]   name    The unique name to search for.
 *
 * @returns             The @em MID of the metric with the name given or @c NULL, if no exact match was found.
 */
siox_mid siox_ont_find_mid_by_name( const char * name);

/**
 * Compare two @em MIDs for equality.
 *
 * @param[in]   mid1    An @em MID.
 * @param[in]   mid2    Another @em MID.
 *
 * @returns             @c true if the @em MIDs are equal or both are @c NULL; otherwise @c false.
 */
bool siox_ont_mid_is_equal( siox_mid mid1, siox_mid mid2 );

/**
 * Destructor for an @em MID object.
 *
 * @param [in]  mid The @em MID object.
 */
void siox_ont_free_mid( siox_mid mid );

/**@}*/



/**
 * @name Functions for the Metric Object @em siox_metric
 */
/**@{*/


/**
 * Retrieve the metric with the @em MID given.
 *
 * @param[in]   mid     The @em MID of the metric.
 *
 * @returns             If the @e MID exists, a copy of the metric's data; otherwise @c NULL.
 */
siox_metric siox_ont_find_metric_by_mid( siox_mid mid );

/**
 * Retrieve a given metric's @em MID.
 *
 * @param[in]   metric  The metric.
 *
 * @returns             If the metric exists, its @em MID; otherwise @c NULL.
 */
siox_mid siox_ont_metric_get_mid( siox_metric metric );

/**
 * Retrieve a given metric's unique name.
 *
 * @param[in]   metric  The metric.
 *
 * @returns             If the metric exists, a copy of its unique name; otherwise, @c NULL.
 */
const char * siox_ont_metric_get_name( siox_metric metric );

/**
 * Retrieve a given metric's description.
 *
 * @param[in]   metric  The metric.
 *
 * @returns             If the metric exists, a copy of its description; otherwise @c NULL.
 */
const char * siox_ont_metric_get_description( siox_metric metric );

/**
 * Retrieve a given metric's unit attribute.
 *
 * @param[in]   metric  The metric.
 *
 * @returns             If the metric exists, the metric's unit attibute;
 *                      otherwise @c SIOX_UNIT_UNASSIGNED.
 */
enum siox_ont_unit_type siox_ont_metric_get_unit( siox_metric metric );

/**
 * Retrieve a given metric's storage attribute.
 *
 * @param[in]   metric  The metric.
 *
 * @returns             If the metric exists, the metric's storage attribute;
 *                      otherwise @c SIOX_STORAGE_UNASSIGNED.
 */
enum siox_ont_storage_type siox_ont_metric_get_storage( siox_metric metric );

/**
 * Retrieve a given metric's scope attribute.
 *
 * @param[in]   metric  The metric.
 *
 * @returns             If the metric exists, the metric's scope attribute;
 *                      otherwise @c SIOX_SCOPE_UNASSIGNED.
 */
enum siox_ont_scope_type siox_ont_metric_get_scope( siox_metric metric );

/**
 * Turn the metric data into a human-readable string.
 *
 * @param[in]   metric  The metric object.
 *
 * @returns             If the metric exists, a multi-line string representing the data; otherwise @c NULL.
 */
char* siox_ont_metric_to_string( siox_metric metric );

/**
 * Destructor for a metric object.
 *
 * @param[in]   metric  The metric object.
 */
void siox_ont_free_metric( siox_metric metric );

/**@}*/



/**
 * @name Sizing Up and Extending the Ontology
 */
/**@{*/

/**
 * Count the number of metrics in the ontology.
 *
 * @returns     The number of metrics.
 */
int siox_ont_count_metrics();

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


/**@}*/


#endif
