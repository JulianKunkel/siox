/**
 * @file    datatype.h
 *          Headerdatei für die Datatentypen @em DTID und @em datatype
 *          im SIOX-Ontology-Interface
 *
 * @authors Michaela Zimmer
 * @date    2012
 *          GNU Public License
 */


#ifndef siox_ONT_DT_H
#define siox_ONT_DT_H


#include <stdbool.h>


/**
 * Die <em>Data Type ID</em>.
 *
 * Identifiziert einen Semantischen Datentyp in der Ontologie.
 */
typedef struct siox_dtid_t * siox_dtid;


/**
 * Der Datentyp mit allen Attributen.
 *
 * Enthält die Beschreibung eines Datentyps mit allen zugehörigen Eigenschaften und Attributen.
 */
typedef struct siox_datatype_t * siox_datatype;



/**
 * @name Functions for the @em DTID object @em siox_dtid
 */
/**@{*/


/**
 * Create a new @em DTID with a given id.
 *
 * @param[in]   id  The id the new @em DTID is to have.
 * @returns         A new @em DTID with the id given, unless there is no memory left,
 *                  in which case @c NULL is returned.
 */
siox_dtid siox_ont_dtid_from_id( int id );


/**
 * Compare two @em DTIDs for equality.
 *
 * @param[in]   dtid1    An @em DTID.
 * @param[in]   dtid2    Another @em DTID.
 *
 * @returns             @c true if the @em MIDs are equal or both are @c NULL; otherwise @c false.
 */
bool siox_ont_dtid_is_equal( siox_dtid dtid1, siox_dtid dtid2 );


/**
 * Turn the DTID into a human-readable string.
 *
 * @param[in]   dtid    The @em DTID object.
 *
 * @returns             If the metric exists, a string representing the data; otherwise @c NULL.
 */
const char* siox_ont_dtid_to_string( siox_dtid dtid );


/**
 * Destructor for an @em DTID object.
 *
 * @param [in]  dtid The @em DTID object.
 */
void siox_ont_free_dtid( siox_dtid dtid );


/**@}*/



/**
 * @name Functions for the DataType Object @em siox_datatype
 */
/**@{*/


/**
 * Generic constructor for the datatype object.
 *
 * @returns         A new datatype object with storage field initialised to @c NULL or
 *                  @c SIOX_STORAGE_UNASSIGNED;
 *                  if no memory could be allocated, @c NULL is returned.
 */
siox_datatype siox_ont_new_datatype( void );


/**
 * Retrieve a given datatype's @em DTID.
 *
 * @param[in]   datatype  The datatype.
 *
 * @returns             If the datatype exists, its @em DTID; otherwise @c NULL.
 */
siox_dtid siox_ont_datatype_get_dtid( siox_datatype datatype );


/**
 * Retrieve a given datatype's unique name.
 *
 * @param[in]   datatype  The datatype.
 *
 * @returns             If the datatype exists, a copy of its unique name; otherwise, @c NULL.
 */
const char * siox_ont_datatype_get_name( siox_datatype datatype );


/**
 * Retrieve a given datatype's storage attribute.
 *
 * @param[in]   datatype  The datatype.
 *
 * @returns             If the datatype exists, the datatype's storage attribute;
 *                      otherwise @c SIOX_STORAGE_UNASSIGNED.
 */
enum siox_ont_storage_type siox_ont_datatype_get_storage( siox_datatype datatype );


/**
 * Turn the datatype data into a human-readable string.
 *
 * @param[in]   datatype  The datatype object.
 *
 * @returns             If the datatype exists, a multi-line string representing the data; otherwise @c NULL.
 */
char* siox_ont_datatype_to_string( siox_datatype datatype );


/**
 * Destructor for a datatype object.
 *
 * @param[in]   datatype  The datatype object.
 */
void siox_ont_free_datatype( siox_datatype datatype );


/**@}*/


#endif

