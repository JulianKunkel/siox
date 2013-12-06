/**
 * @file siox-types.h
 * @date 2013-08-06
 */

#ifndef SIOX_C_TYPES_H
#define SIOX_C_TYPES_H

#include <stdint.h>

/**
 * A time stamp, as represented in SIOX.
 * This is a count of nano-seconds.
 */
typedef uint64_t siox_timestamp;

/**
 * Result of a function indicating its error status.
 * @todo: Design question - map to abstract values (enum) or use raw values?
 */
typedef uint32_t siox_activity_error;


/**
 * Supported datatypes in SIOX
 * The transaction system needs fixed datatypes - any changes here will cause the
 * transaction system to stop working!
 * In that case, the scripts for the transaction system have to be modified.
 * Postgres needs datatypes twice as large as these for storage due to its internal structure.
 */
enum siox_ont_storage_type {
	SIOX_STORAGE_64_BIT_INTEGER,
	SIOX_STORAGE_64_BIT_UINTEGER,
	SIOX_STORAGE_32_BIT_INTEGER,
	SIOX_STORAGE_32_BIT_UINTEGER,
	SIOX_STORAGE_FLOAT,
	SIOX_STORAGE_DOUBLE,
	SIOX_STORAGE_UNASSIGNED,
	SIOX_STORAGE_STRING
};

#endif
