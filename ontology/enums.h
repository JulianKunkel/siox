/**
 * @file    enums.h
 *          Headerdatei für die Enumerationstypen
 *          im SIOX-Ontology-Interface
 *
 * @authors Michaela Zimmer, Marc Wiedemann
 * @date    2012
 *
 */
#ifndef siox_ONT_ENUM_H
#define siox_ONT_ENUM_H

namespace Scalable_IO
{

/** States which may occur during input file parsing */
enum siox_ont_state
{
	STANDARD=0,
	NAME,
	DESCRIPTION,
	MANDATORY,
	NUMBER
};

/** Error handling */

enum siox_ont_errors
{
	PARSE_OK,
	E_INPUT=1,
	E_OUTPUT=2,
	E_STYLE=4,
	E_BINARY=8
}

/** Output */
enum siox_ont_OutputType
{
	TXT,
	PNG,
	ANSI,
	XTERM256,
};

/**
 * Mögliche Werte für das Attribut "Unit".
 */
enum siox_ont_unit_type{
    SIOX_UNIT_UNASSIGNED,
    SIOX_UNIT_SECONDS,
    SIOX_UNIT_BYTES,
    SIOX_UNIT_IOPS,
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
    SIOX_SCOPE_SAMPLE,
    SIOX_SCOPE_AVERAGE,
    SIOX_SCOPE_DIFFERENCE,
    SIOX_SCOPE_MAXIMUM,
    SIOX_SCOPE_MINIMUM,
    SIOX_SCOPE_MEDIAN,
    SIOX_SCOPE_SUM
};

/**
 * Mögliche Typen für die Entität "Metrik". Hier erstmal kombinierte units
 */

typedef union {
        char throughput[30];

	struct {
		SIOX_UNIT_BYTES;
                SIOX_UNIT_SECONDS;
	}
};


#endif
