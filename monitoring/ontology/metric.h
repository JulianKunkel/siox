/**
 * @file    metric.h
 *          Headerdatei für die Datentypen @em MID und @em metric
 *          im SIOX-Ontology-Interface
 *
 * @authors Michaela Zimmer
 * @date    2012
 *          GNU Public License
 */


#ifndef siox_ONT_M_H
#define siox_ONT_M_H


#include <stdbool.h>


/**
 * Die <em>Metric ID</em>.
 *
 * Identifiziert eine Metrik in der Ontologie.
 */
typedef struct siox_mid_t * siox_mid;


/**
 * Die Metrik mit allen Attributen.
 *
 * Enthält die Beschreibung einer Metrik mit allen zugehörigen Eigenschaften
 * und Attributen.
 */
typedef struct siox_metric_t * siox_metric;



/**
 * @name Functions for the @em MID object @em siox_mid
 */
/**@{*/


/**
 * Create a new @em MID with the id given.
 *
 * @param[in]   id  The id the new @em MID is to have.
 * @returns         A new @em MID with the id given, unless there is no memory left,
 *                  in which case @c NULL is returned.
 */
siox_mid siox_ont_mid_from_id( int id );


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
 * Turn the @em MID into a human-readable string.
 *
 * @param[in]   mid     The @em MID object.
 *
 * @returns             If the @em MID exists, a string representing the data;
 *                      otherwise, @c NULL.
 */
const char* siox_ont_mid_to_string( siox_mid mid );


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
 * Generic constructor for the metric object.
 *
 * @returns         A new metric object with fields initialised to @c NULL or
 *                  @c SIOX_[UNIT|STORAGE|SCOPE]_UNASSIGNED;
 *                  if no memory could be allocated, @c NULL is returned.
 */
siox_metric siox_ont_new_metric( void );

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


#endif

