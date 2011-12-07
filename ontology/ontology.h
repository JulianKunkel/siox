/**
 * @file	ontology.h
 *			Headerdatei für das SIOX-Ontology-Interface
 * 
 * @authors	Michaela Zimmer
 * @date	2011
 * 			GNU Public License
 */


#ifndef siox_ONT_H
#define siox_ONT_H


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
	SIOX_UNIT_SECONDS,
	SIOX_UNIT_BYTES,
	SIOX_UNIT_FLOPS,
	SIOX_UNIT_UNITS
};


/**
 * Mögliche Werte für das Attribut "Minimal Storage Type" der Metrik.
 */
enum siox_ont_storage_type{
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
	SIOX_SCOPE_SAMPLE,
	SIOX_SCOPE_AVERAGE,
	SIOX_SCOPE_DIFFERENCE,
	SIOX_SCOPE_MAXIMUM,
	SIOX_SCOPE_MINIMUM,
	SIOX_SCOPE_MEDIAN,
	SIOX_SCOPE_SUM
};



/**
 * @name Öffnen und Schließen der Ontologie
 */
/**@{*/

/**
 * Liest eine Ontologie aus einer Datei.
 *
 * @param	file	Der Name der Datei, aus der die Ontologie gelesen werden soll.
 *					Existiert die Datei noch nicht, wird eine neue (leere) Ontologie angelegt.
 *
 * @returns			0 bei Erfolg, 1 nach Neuanlegen der Ontologie, sonst -1.
 */
int siox_ont_open_ontology( const char * file );

/**
 * Schreibt den aktuellen Stand der Ontologie in die bei siox_ont_open_ontology() angegebene Datei.
 * Der bisherige Inhalt wird dabei überschrieben!
 *
 * @returns			0 bei Erfolg, sonst -1.
 */
int siox_ont_write_ontology();

/**
 * Schließt die Ontologie wieder.
 *
 * Belegte Ressourcen werden freigegeben.
 * Eventuelle Änderungen an der Ontologie schreibt diese Funktion <em>nicht</em> in die Datei zurück;
 * dazu muß siox_ont_write_ontology() aufgerufen werden!
 *
 * @returns			0 bei Erfolg, sonst -1.
 */
int siox_ont_close_ontology();

/**@}*/



/**
 * @name Funktionen zum Auslesen der Ontologie
 */
/**@{*/

/**
 * Find the metric with the exact name given.
 *
 * @param[in]	name	The unique name to search for.
 *
 * @returns				The @em MID of the metric with the name given or @c NULL, if no exact match was found.
 */
siox_mid siox_ont_find_metric( const char * name);

/**
 * Retrieve the metric to the @em MID given.
 *
 * @param[in]	mid		The @em MID of the metric.
 *
 * @returns				A copy of the metric's data.
 */
siox_metric siox_ont_get_metric( siox_mid mid );

/**
 * Retrieve a given metric's @em MID.
 *
 * @param[in]	metric	The metric.
 *
 * @returns				The metric's @em MID.
 */
siox_mid siox_ont_get_metric_id( siox_metric metric );

/**
 * Retrieve a given metric's unique name.
 *
 * @param[in]	metric	The metric.
 *
 * @returns				The metric's unique name.
 */
char * siox_ont_get_metric_name( siox_metric metric );

/**
 * Retrieve a given metric's description.
 *
 * @param[in]	metric	The metric.
 *
 * @returns				The metric's description.
 */
char * siox_ont_get_metric_description( siox_metric metric );

/**
 * Retrieve a given metric's unit attribute.
 *
 * @param[in]	metric	The metric.
 *
 * @returns				The metric's unit attibute.
 */
enum siox_ont_unit_type siox_ont_get_metric_unit( siox_metric metric );

/**
 * Retrieve a given metric's storage attribute.
 *
 * @param[in]	metric	The metric.
 *
 * @returns				The metric's storage attribute.
 */
enum siox_ont_storage_type siox_ont_get_metric_storage( siox_metric metric );

/**
 * Retrieve a given metric's scope attribute.
 *
 * @param[in]	metric	The metric.
 *
 * @returns				The metric's scope attribute.
 */
enum siox_ont_scope_type siox_ont_get_metric_scope( siox_metric metric );

/**
 * Turn the metric data into a human-readable string.
 *
 * @param[in]	metric	The metric data.
 *
 * @returns				A multi-line string representing the data.
 */
char* siox_ont_metric_to_string( siox_metric metric );

/**
 * Count the number of metrics in the ontology.
 *
 * @returns		The number of metrics.
 */
int siox_ont_count_metrics();

/**@}*/



/**
 * @name Funktionen zum Erweitern der Ontologie
 */
/**@{*/

/**
 * Fügt eine neue Metrik in die Ontologie ein.
 *
 * @param	name		Der Name der Metrik. Er muß eindeutig sein.
 * @param	description	Eine textuelle Beschreibung der Metrik.
 * @param	unit		Die Einheit, in welcher die Daten gemessen werden.
 * @param	storage		Der minimale zum Speichern der Daten nötige Datentyp.
 * @param	scope		Der zeitliche Bereich, in welchem die Daten angefallen sind.
 *
 * @returns				Eine <em>Metric ID</em>.
 */
siox_mid siox_ont_register_metric( const char *					name,
								   const char *					description,
								   enum siox_ont_unit_type		unit,
								   enum siox_ont_storage_type	storage,
								   enum siox_ont_scope_type		scope );

 
/**@}*/


#endif
