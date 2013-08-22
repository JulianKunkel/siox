/**
 * @file libotf2siox.h
 * @author Alvaro Aguilera
 */

#ifndef _LIBOTF2SIOX_H
#define _LIBOTF2SIOX_H

#include <assert.h>
#include <errno.h>
#include <otf.h>

#include "siox-ll.h"


/**
 * @name Custom error codes
 *
 * @{
 */
#define ENOMANAGER  100
#define ENOHANDLER  101
#define ENOEVENTS   102
/** @} */


/**
 * @name I/O Function Family
 *
 * @{
 */
#define OPEN_F  1
#define WRIT_F  2
#define READ_F  3
#define CLOS_F  4
#define SEEK_F  5
#define LOCK_F  6
#define STAT_F  7
#define DELE_F  8
#define CNTL_F  9
#define OTHR_F 10
/** @} */


/**
 * @name Type of I/O operation
 *
 * @{
 */
#define DATA_T 1
#define META_T 2
#define OTHR_T 3
/** @} */


/**
 * @name Software layers
 *
 * @{
 */
#define POSIX_L 1
#define LIBC_L  2
#define MPI_L   3
/** @} */


/**
 * @name I/O Function Dictionary
 *
 * This is a dictionary containing the list of recognized I/O functions together
 * with their family, type and layer.
 *
 * @{
 */
#define IO_FUNCTIONS_COUNT 107
#define IO_FUNCTIONS { \
		{ "close",        CLOS_F, META_T, POSIX_L }, \
		{ "creat",        OPEN_F, META_T, POSIX_L }, \
		{ "creat64",      OPEN_F, META_T, POSIX_L }, \
		{ "dup",          OPEN_F, META_T, POSIX_L }, \
		{ "dup2",         OPEN_F, META_T, POSIX_L }, \
		{ "fclose",       CLOS_F, META_T, LIBC_L  }, \
		{ "fcntl",        CNTL_F, META_T, POSIX_L }, \
		{ "fdatasync",    WRIT_F, DATA_T, POSIX_L }, \
		{ "fdopen",       OPEN_F, META_T, LIBC_L  }, \
		{ "fflush",       WRIT_F, DATA_T, LIBC_L  }, \
		{ "fgetc",        READ_F, DATA_T, LIBC_L  }, \
		{ "fgets",        READ_F, DATA_T, LIBC_L  }, \
		{ "flockfile",    LOCK_F, META_T, LIBC_L  }, \
		{ "flush",        WRIT_F, DATA_T, POSIX_L }, \
		{ "fopen",        OPEN_F, META_T, LIBC_L  }, \
		{ "fopen64",      OPEN_F, META_T, LIBC_L  }, \
		{ "fputs",        WRIT_F, DATA_T, LIBC_L  }, \
		{ "fprintf",      WRIT_F, DATA_T, LIBC_L  }, \
		{ "fread",        READ_F, DATA_T, LIBC_L  }, \
		{ "fscanf",       READ_F, DATA_T, LIBC_L  }, \
		{ "fseek",        SEEK_F, DATA_T, LIBC_L  }, \
		{ "fseeko",       SEEK_F, DATA_T, LIBC_L  }, \
		{ "fseeko64",     SEEK_F, DATA_T, LIBC_L  }, \
		{ "fsetpos",      CNTL_F, META_T, LIBC_L  }, \
		{ "fsetpos64",    CNTL_F, META_T, LIBC_L  }, \
		{ "fstat",        STAT_F, META_T, LIBC_L  }, \
		{ "fsync",        WRIT_F, DATA_T, LIBC_L  }, \
		{ "ftrylockfile", LOCK_F, META_T, LIBC_L  }, \
		{ "funlockfile",  LOCK_F, META_T, LIBC_L  }, \
		{ "fwrite",       WRIT_F, DATA_T, LIBC_L  }, \
		{ "getc",         READ_F, DATA_T, POSIX_L }, \
		{ "gets",         READ_F, DATA_T, POSIX_L }, \
		{ "lockf",        LOCK_F, META_T, POSIX_L }, \
		{ "lseek",        SEEK_F, DATA_T, POSIX_L }, \
		{ "lseek64",      SEEK_F, DATA_T, POSIX_L }, \
		{ "lstat",        STAT_F, META_T, POSIX_L }, \
		{ "MPI_File_call_errhandler",     OTHR_F, OTHR_T, MPI_L }, \
		{ "MPI_File_close",               CLOS_F, META_T, MPI_L }, \
		{ "MPI_File_create_errhandler",   OTHR_F, OTHR_T, MPI_L }, \
		{ "MPI_FILE_delete",              DELE_F, META_T, MPI_L }, \
		{ "MPI_File_get_amode",           STAT_F, META_T, MPI_L }, \
		{ "MPI_File_get_atomicity",       STAT_F, META_T, MPI_L }, \
		{ "MPI_File_get_byte_offset",     STAT_F, META_T, MPI_L }, \
		{ "MPI_File_get_errhandler",      OTHR_F, OTHR_T, MPI_L }, \
		{ "MPI_File_get_group",           STAT_F, META_T, MPI_L }, \
		{ "MPI_File_get_info",            STAT_F, META_T, MPI_L }, \
		{ "MPI_File_get_position",        STAT_F, META_T, MPI_L }, \
		{ "MPI_File_get_position_shared", STAT_F, META_T, MPI_L }, \
		{ "MPI_File_get_size",            STAT_F, META_T, MPI_L }, \
		{ "MPI_File_get_type_extent",     STAT_F, META_T, MPI_L }, \
		{ "MPI_File_get_view",            STAT_F, META_T, MPI_L }, \
		{ "MPI_File_iread",               READ_F, DATA_T, MPI_L }, \
		{ "MPI_File_iread_at",            READ_F, DATA_T, MPI_L }, \
		{ "MPI_File_iread_shared",        READ_F, DATA_T, MPI_L }, \
		{ "MPI_File_iwrite ",             WRIT_F, DATA_T, MPI_L }, \
		{ "MPI_File_iwrite_at",           WRIT_F, DATA_T, MPI_L }, \
		{ "MPI_File_iwrite_shared",       WRIT_F, DATA_T, MPI_L }, \
		{ "MPI_File_open",                OPEN_F, META_T, MPI_L }, \
		{ "MPI_File_preallocate",         CNTL_F, META_T, MPI_L }, \
		{ "MPI_File_read",                READ_F, DATA_T, MPI_L }, \
		{ "MPI_File_read_all",            READ_F, DATA_T, MPI_L }, \
		{ "MPI_File_read_all_begin",      READ_F, DATA_T, MPI_L }, \
		{ "MPI_File_read_all_end",        READ_F, DATA_T, MPI_L }, \
		{ "MPI_File_read_at",             READ_F, DATA_T, MPI_L }, \
		{ "MPI_File_read_at_all",         READ_F, DATA_T, MPI_L }, \
		{ "MPI_File_read_at_all_begin",   READ_F, DATA_T, MPI_L }, \
		{ "MPI_File_read_at_all_end",     READ_F, DATA_T, MPI_L }, \
		{ "MPI_File_read_ordered",        READ_F, DATA_T, MPI_L }, \
		{ "MPI_File_read_ordered_begin",  READ_F, DATA_T, MPI_L }, \
		{ "MPI_File_read_ordered_end",    READ_F, DATA_T, MPI_L }, \
		{ "MPI_File_read_shared",         READ_F, DATA_T, MPI_L }, \
		{ "MPI_File_seek",                SEEK_F, DATA_T, MPI_L }, \
		{ "MPI_File_seek_shared",         SEEK_F, DATA_T, MPI_L }, \
		{ "MPI_File_set_atomicity",       CNTL_F, META_T, MPI_L }, \
		{ "MPI_File_set_errhandler",      OTHR_F, OTHR_T, MPI_L }, \
		{ "MPI_File_set_info",            CNTL_F, META_T, MPI_L }, \
		{ "MPI_File_set_size",            CNTL_F, META_T, MPI_L }, \
		{ "MPI_File_set_view",            CNTL_F, META_T, MPI_L }, \
		{ "MPI_File_sync",                WRIT_F, DATA_T, MPI_L }, \
		{ "MPI_File_write",               WRIT_F, DATA_T, MPI_L }, \
		{ "MPI_File_write_all",           WRIT_F, DATA_T, MPI_L }, \
		{ "MPI_File_write_all_begin",     WRIT_F, DATA_T, MPI_L }, \
		{ "MPI_File_write_all_end",       WRIT_F, DATA_T, MPI_L }, \
		{ "MPI_File_write_at",            WRIT_F, DATA_T, MPI_L }, \
		{ "MPI_File_write_at_all",        WRIT_F, DATA_T, MPI_L }, \
		{ "MPI_File_write_at_all_begin",  WRIT_F, DATA_T, MPI_L }, \
		{ "MPI_File_write_at_all_end",    WRIT_F, DATA_T, MPI_L }, \
		{ "MPI_File_write_ordered",       WRIT_F, DATA_T, MPI_L }, \
		{ "MPI_File_write_ordered_begin", WRIT_F, DATA_T, MPI_L }, \
		{ "MPI_File_write_ordered_end",   WRIT_F, DATA_T, MPI_L }, \
		{ "MPI_File_write_shared",        WRIT_F, DATA_T, MPI_L }, \
		{ "open",         OPEN_F, META_T, POSIX_L }, \
		{ "open64",       OPEN_F, META_T, POSIX_L }, \
		{ "pread",        READ_F, DATA_T, POSIX_L }, \
		{ "pread64",      READ_F, DATA_T, POSIX_L }, \
		{ "putc",         WRIT_F, DATA_T, POSIX_L }, \
		{ "puts",         WRIT_F, DATA_T, POSIX_L }, \
		{ "pwrite",       WRIT_F, DATA_T, POSIX_L }, \
		{ "pwrite64",     WRIT_F, DATA_T, POSIX_L }, \
		{ "read",         READ_F, DATA_T, POSIX_L }, \
		{ "readv",        READ_F, DATA_T, POSIX_L }, \
		{ "rewind",       SEEK_F, DATA_T, POSIX_L }, \
		{ "stat",         STAT_F, META_T, POSIX_L }, \
		{ "sync",         WRIT_F, DATA_T, POSIX_L }, \
		{ "unlink",       DELE_F, META_T, POSIX_L }, \
		{ "write",        WRIT_F, DATA_T, POSIX_L }, \
		{ "writev",       WRIT_F, DATA_T, POSIX_L }  \
	}
/** @} */


/**
 * Contains the definitions and file handlers for the OTF file.
 */
struct otf_data {
	OTF_FileManager * manager;
	OTF_HandlerArray * handlers;
	OTF_Reader * reader;
	char * version;
	char * creator;
	uint64_t resolution; /**< of the timestamps in ticks per second. */
	uint64_t range_min;
	uint64_t range_max;
	uint32_t * processes;
	uint32_t proc_count;
	struct finfo * functions;
	uint32_t func_count;
	char ** files;
	uint32_t file_count;
	struct keyvalue * keyvals;
	uint32_t kv_count;
};


/** OTF key-value pair */
struct keyvalue {
	char * name;
	char * desc;
	OTF_Type type;
};


/** Function information */
struct finfo {
	char * name;
	uint8_t family;
	uint8_t type;
	uint8_t layer;
	uint32_t group;
};


/** Contains the number of processes running on each SIOX node as well as an
 * array of SIOX node-Ids for each process
 */
struct siox_data {
	uint32_t ppn; /**< Number of processes per SIOX node. */
	siox_unid * nodes; /**< Array of registered SIOX nodes, indexed per
                   process number */
	uint32_t node_count;
};


/** I/O operation containing all the information about it that was stored in the
 * OTF file, together with the SIOX activity and node ID.
 */
struct ioop {
	uint64_t time;
	uint32_t fileid;
	uint32_t process;
	uint64_t handleid;
	uint32_t operation;
	uint32_t flags;
	uint64_t bytes;
	double duration;
	uint32_t source;
	siox_aid aid;
	siox_unid unid;
	struct finfo * finfo;
};


/** Used for convenience, it contains the OTF data, the SIOX data and an array
 * pointing to one operation stack per process to deal with nested operations
 */
struct userdata {
	struct otf_data * odata;
	struct siox_data * siox;
	struct op_stack ** stack;
};


/** Simple stack to manage the operations */
struct op_stack {
	struct ioop * op;
	struct op_stack * next;
};


/**
 * @name OTF helper functions
 *
 * Functions to ease the management of the OTF files.
 *
 * @{
 */

/**
 * This function is in charge of importing the OTF file into SIOX.
 *
 * @note OTF doesn't store any information about the hardware on which the
 * processes run. OTF2SIOX create virtual hardware nodes and distributes the
 * processes evenly among them. The number of processes per virtual SIOX
 * node can be regulated with the parameter `ppn`.
 *
 * @param filename Path to the OTF file.
 * @param maxfiles Maximum number of event files to open.
 * @param ppn Number of processes per SIOX node.
 */
int import_otf( const char * filename, const uint32_t maxfiles,
                const uint32_t ppn );

/**
 * opens the OTF file using the OTF API and stores the file handlers in a
 * `otf_data` structure.
 */
void open_otf( struct otf_data * odata, const char * filename,
               const uint32_t maxfiles );


/**
 * Closes the OTF file specified in the `otf_data` structure using the OTF API.
 */
void close_otf( struct otf_data * odata );

/**@}*/


/**
 * @name OTF Definition Counters
 *
 * This group of functions is used to increment the counters of the different
 * OTF definitions (number of processes, files, functions and key-value pairs).
 *
 * @note For more information about the meaning of the parameters, please refer
 * to the OTF documentation.
 */
/**@{*/

/**
 * Counts the number of processes, functions, files and key-value pairs defined
 * inside the given OTF file.
 *
 * @param odata `otf_data` structure with the open OTF file, etc.
 */
void count_definitions( struct otf_data * odata );

int inc_proc_count( struct otf_data * odata, uint32_t stream, uint32_t process,
                    const char * name, uint32_t parent, OTF_KeyValueList * kvlist );

int inc_file_count( struct otf_data * odata, uint32_t stream, uint32_t token,
                    const char * name, uint32_t funcGroup,
                    OTF_KeyValueList * kvlist );

int inc_function_count( struct otf_data * odata, uint32_t stream, uint32_t func,
                        const char * name, uint32_t funcGroup, uint32_t source,
                        OTF_KeyValueList * kvlist );

int inc_keyvalue_count( struct otf_data * odata, uint32_t stream, uint32_t key,
                        OTF_Type type, const char * name, const char * desc,
                        OTF_KeyValueList * kvlist );
/**@}*/


/**
 * @name OTF Definition Handlers
 *
 * This group of functions stores the relevant definitions comming from the OTF
 * stream into a `otf_data` structure.
 *
 * @note For more information about the meaning of the parameters, please refer
 * to the OTF documentation.
 *
 * @{*/

/**
 * Allocate the buffers to store the OTF definitions.
 *
 * @param odata `otf_data` structure where the buffers will be allocated.
 */
void allocate_def_arrays( struct otf_data * odata );

/**
 * Loads the relevant OTF definitions into a `otf_data` structure.
 *
 * @param odata Pointer to the `otf_data` structure where the definitions will
 * be stored.
 */
void load_definitions( struct otf_data * odata );

int def_version( struct otf_data * odata, uint32_t stream, uint8_t major,
                 uint8_t minor, uint8_t sub, const char * string,
                 OTF_KeyValueList * kvlist );

int def_creator( struct otf_data * odata, uint32_t stream, const char * creator,
                 OTF_KeyValueList * kvlist );

int def_resolution( struct otf_data * odata, uint32_t stream,
                    uint64_t ticksPerSecond, OTF_KeyValueList * kvlist );

int def_range( struct otf_data * odata, uint32_t stream, uint64_t mintime,
               uint64_t maxtime, OTF_KeyValueList * kvlist );

int def_function( struct otf_data * odata, uint32_t stream, uint32_t func,
                  const char * name, uint32_t funcGroup, uint32_t source,
                  OTF_KeyValueList * kvlist );

int def_function_group( struct otf_data * odata, uint32_t stream,
                        uint32_t funcGroup, const char * name,
                        OTF_KeyValueList * kvlist );

int def_file( struct otf_data * odata, uint32_t stream, uint32_t token,
              const char * name, uint32_t funcGroup, OTF_KeyValueList * kvlist );

int def_process( struct otf_data * odata, uint32_t stream, uint32_t process,
                 const char * name, uint32_t parent, OTF_KeyValueList * kvlist );


int def_keyvalue( struct otf_data * odata, uint32_t stream, uint32_t key,
                  OTF_Type type, const char * name, const char * desc,
                  OTF_KeyValueList * kvlist );
/**@}*/


/**
 * @name OTF Event Handler Functions.
 *
 * An I/O call will be described as:
 *
 *  event_enter, event_begin_io, (*), event_end_io, event_end
 *
 * nesting is possible at position (*).
 *
 * @note For more information about the meaning of the parameters, please refer
 * to the OTF documentation.
 *
 * @{
 */

/**
 * Processes the I/O events stored in the OTF file already open.
 *
 * @param odata `otf_data` structure containing the OTF definitions.
 * @param sdata `siox_data` containing the relevant SIOX information.
 */
void process_events( struct otf_data * odata, struct siox_data * sdata );

int event_enter( struct userdata * u, uint64_t time, uint32_t function,
                 uint32_t process, uint32_t source, OTF_KeyValueList * kvlist );

int event_leave( struct userdata * u, uint64_t time, uint32_t function,
                 uint32_t process, uint32_t source, OTF_KeyValueList * kvlist );

int event_begin_io( struct userdata * u, uint64_t time, uint32_t process,
                    uint64_t matchingId, uint32_t scltoken,
                    OTF_KeyValueList * kvlist );

int event_end_io( struct userdata * u, uint64_t time, uint32_t process,
                  uint32_t fileid, uint64_t matchingId, uint64_t handleId,
                  uint32_t operation, uint64_t bytes, uint32_t scltoken,
                  OTF_KeyValueList * kvlist );
/**@}*/


/**
 * @name SIOX Helper Functions
 *
 * Functions used to set up the SIOX environment.
 *
 * @{
 */

/**
 * Registers the SIOX nodes resulting from the number of processes defined
 * inside the OTF file and the number of process per SIOX nodes given as a
 * parameter to the program.
 *
 * @param odata `otf_data` structure.
 * @param sdata `siox_data` structure.
 * @param ppn process per node.
 */
void register_nodes( struct otf_data * odata, struct siox_data * sdata,
                     const uint32_t ppn );

/**
 * Unregisters all SIOX nodes
 *
 * @param sdata `siox_data` structure containing the array of SIOX nodes.
 */
void checkout( struct siox_data * sdata );


/**@}*/


/**
 * @name SIOX Report Functions
 *
 * These functions send the performance data of a given I/O operation to the
 * SIOX system.
 *
 * @todo Most of these functions report only the completion times. If that is
 * to stay that way they could be joined together into just two functions.
 *
 * @{
 */
void report_close( struct userdata * u, struct ioop * op );
void report_lock( struct userdata * u, struct ioop * op );
void report_open( struct userdata * u, struct ioop * op );
void report_read( struct userdata * u, struct ioop * op );
void report_seek( struct userdata * u, struct ioop * op );
void report_stat( struct userdata * u, struct ioop * op );
void report_write( struct userdata * u, struct ioop * op );
void report_del( struct userdata * u, struct ioop * op );
void report_cntl( struct userdata * u, struct ioop * op );
void report_other( struct userdata * u, struct ioop * op );
/**@}*/


/**
 * @name Operation Stack Helper Functions
 *
 * Functions to manage the operation stacks.
 * @{
 */

/**
 * Helper function to initialize the operation stacks.
 *
 * @param s Pointer to an array of operation stacks (one for each process).
 * @param n Number of processes.
 */
void init_op_stacks( struct op_stack ** *s, uint32_t n );


/**
 * Helper function to push an operation in a process stack.
 *
 * @param s Pointer to the operation stack of one process.
 * @param op Pointer to the operation to be pushed.
 */
void push_op( struct op_stack ** s, struct ioop * op );


/**
 * Helper function to pop an operation from a process stack.
 *
 * @param s Pointer to the operation stack of one process.
 * @param op Pointer to the operation to be returned.
 * @return -1 in case or error, otherwise 0.
 */
int pop_op( struct op_stack ** s, struct ioop ** op );

/**@}*/


/**
 * @name Utility Functions
 *
 * Other useful functions.
 *
 * @{
 */

/**
 * If the function `fname` was defined in the macro `IO_FUNCTIONS`, then the
 * relevant information is copied into `f`.
 *
 * @param odata `otf_data` structure.
 * @param fname name of the function.
 * @param f `finfo` structure for the return value.
 * @return 0 if the functions was found, -1 otherwise.
 */
int get_finfo( struct otf_data * odata, const char * fname, struct finfo ** f );


/**
 * Frees all the allocated buffers.
 *
 * @param odata OTF related buffers.
 * @param sdata SIOX related buffers.
 *
 */
void murica( struct otf_data * odata, struct siox_data * sdata );

/** @} */

#endif /* _LIBOTF2SIOX_H */