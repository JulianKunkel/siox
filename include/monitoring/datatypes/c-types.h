#ifndef SIOX_C_TYPES_H
#define SIOX_C_TYPES_H

#include <stdint.h>

/**
 * A time stamp, as represented in SIOX.
 */
typedef uint64_t siox_timestamp;

/**
 * Supported datatypes in SIOX
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
