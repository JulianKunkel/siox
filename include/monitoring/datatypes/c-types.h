/**
 * @file c-types.h
 * @date 2013-07
 */
 
#ifndef SIOX_C_TYPES_H
#define SIOX_C_TYPES_H

#include <stdint.h>

/**
 * A time stamp, as represented in SIOX.
 */
typedef uint64_t siox_timestamp;

/**
 * Result of a function.
 * @todo: Design question - map to abstract values (enum) or use raw values?
 */
typedef uint32_t siox_activity_error;
/**
 * Example for abstract values
 */
/*
typedef enum siox_activity_error {
	SIOX_ACTIVITY_SUCCESS,
	SIOX_ACTIVITY_PARTIAL_SUCCESS,
	SIOX_ACTIVITY_ERROR,
} siox_activity_error;
*/

/**
 * Supported datatypes in SIOX
 * The transaction system needs fixed datatypes - Any changes here will have the effect of the transactionsystem to stop working
 * In that case the scripts for the transaction system have to be modified.
 * Postgres needs twice as large datatypes as these for storage due to its internal structure.
 */
enum siox_ont_storage_type{
    SIOX_STORAGE_UNASSIGNED,
    SIOX_STORAGE_32_BIT_INTEGER,
    SIOX_STORAGE_64_BIT_INTEGER,
    SIOX_STORAGE_32_BIT_UINTEGER,
    SIOX_STORAGE_64_BIT_UINTEGER,
    SIOX_STORAGE_FLOAT,
    SIOX_STORAGE_DOUBLE,
    SIOX_STORAGE_STRING
};

#endif
