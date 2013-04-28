/**
 * @file    enums.h
 *          Headerdatei für die Enumerationstypen
 *          im SIOX-Ontology-Interface
 *
 * @authors Michaela Zimmer
 * @date    2012
 *          GNU Public License
 */
#ifndef siox_ONT_ENUM_H
#define siox_ONT_ENUM_H


/**
 * Mögliche Werte für das Attribut "Unit".
 */
enum siox_ont_unit_type{
    SIOX_UNIT_UNASSIGNED,
    SIOX_UNIT_SECONDS,
    SIOX_UNIT_NANOSECONDS,
    SIOX_UNIT_BYTES,
    SIOX_UNIT_IOOPS,
    SIOX_UNIT_FLOPS,
    SIOX_UNIT_UNITS
};


/**
 * Mögliche Werte für das Attribut "Minimal Storage Type".
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
 * Mögliche Werte für das Attribut "Data Scope".
 */
enum siox_ont_scope_type{
    SIOX_SCOPE_UNASSIGNED,
    SIOX_SCOPE_ACTUAL,
    SIOX_SCOPE_SAMPLE,
    SIOX_SCOPE_AVERAGE,
    SIOX_SCOPE_DIFFERENCE,
    SIOX_SCOPE_MAXIMUM,
    SIOX_SCOPE_MINIMUM,
    SIOX_SCOPE_MEDIAN,
    SIOX_SCOPE_SUM,
    SIOX_SCOPE_QUOTIENT
};


#endif
