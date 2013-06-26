/**
 */

#ifndef SIOX_BASIC_TYPES_H
#define SIOX_BASIC_TYPES_H
/**
 * A component, as represented in SIOX.
 */
typedef struct siox_component siox_component;

/**
 * An activity, as represented in SIOX.
 */
typedef struct siox_activity siox_activity;

/**
 * A remote call, as represented in SIOX.
 */
typedef struct siox_remote_call siox_remote_call;

/**
 * A time stamp, as represented in SIOX.
 */
typedef uint64_t siox_timestamp;

typedef struct siox_datatype_t siox_datatype;

typedef struct siox_metric_t siox_metric;

typedef struct siox_attribute_t siox_attribute;

////////////////////////////////////////////////


/**
 * Mögliche Werte für das Attribut "Minimal Storage Type".
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
