/**
 * @mainpage otf2siox
 * 
 * otf2siox imports a trace in Open Trace Format (OTF) into the 
 * SIOX system using the low level interface.
 * 
 * This file ist best viewed with 8 characters tabs.
 * 
 * @author Alvaro Aguilera
 */

#ifndef _LIBOTF2SIOX_H
#define _LIBOTF2SIOX_H

#include <assert.h>
#include <errno.h>
#include <otf.h>

#include "siox-ll.h"

#define ENOMANAGER  100
#define ENOHANDLER  101
#define ENOEVENTS   102
#define ENOIOGROUP  103

/* This list should probably include the layer name
   of the function: MPI, libc, etc. */
#define OPEN_ID  1
#define OPEN_MEMBERS {	"open",		\
			"open64",	\
			"fopen",	\
			"fopen64",	\
			"fdopen",	\
			"creat",	\
			"creat64",	\
			"dup",		\
			"dup2",		\
			"MPI_File_open"	\
			}
#define OPEN_LEN 10

#define WRITE_ID  2
#define WRITE_MEMBERS {	"write",			\
			"fwrite",			\
			"fputs",			\
			"writev",			\
			"pwrite",			\
			"pwrite64",			\
			"putc",				\
			"puts",				\
			"fscanf",			\
			"sync",				\
			"flush",			\
			"fflush",			\
			"fsync",			\
			"fdatasync",			\
			"MPI_File_write",		\
			"MPI_File_write_all",		\
			"MPI_File_write_all_begin",	\
			"MPI_File_write_all_end",	\
			"MPI_File_write_at",		\
			"MPI_File_write_at_all",	\
			"MPI_File_write_at_all_begin",	\
			"MPI_File_write_at_all_end",	\
			"MPI_File_write_ordered",	\
			"MPI_File_write_ordered_begin",	\
			"MPI_File_write_ordered_end",	\
			"MPI_File_write_shared",	\
			"MPI_File_iwrite ",		\
			"MPI_File_iwrite_at",		\
			"MPI_File_iwrite_shared",	\
			"MPI_File_sync"			\
			}
#define WRITE_LEN 30

#define READ_ID  3
#define READ_MEMBERS {	"read",				\
			"fread",			\
			"fgets",			\
			"readv",			\
			"pread",			\
			"pread64",			\
			"fgetc",			\
			"getc",				\
			"gets",				\
			"fprintf",			\
			"MPI_File_read",		\
			"MPI_File_read_all",		\
			"MPI_File_read_all_begin",	\
			"MPI_File_read_all_end",	\
			"MPI_File_read_at",		\
			"MPI_File_read_at_all",		\
			"MPI_File_read_at_all_begin",	\
			"MPI_File_read_at_all_end",	\
			"MPI_File_read_ordered",	\
			"MPI_File_read_ordered_begin",	\
			"MPI_File_read_ordered_end",	\
			"MPI_File_read_shared",		\
			"MPI_File_iread",		\
			"MPI_File_iread_at",		\
			"MPI_File_iread_shared" 	\
			}
#define READ_LEN 25

#define SEEK_ID  4
#define SEEK_MEMBERS {	"lseek",			\
			"lseek64",			\
			"fseek",			\
			"fseeko",			\
			"fseeko64",			\
			"rewind",			\
			"fsetpos",			\
			"fsetpos64",			\
			"MPI_File_seek",		\
			"MPI_File_seek_shared",		\
			"MPI_File_get_byte_offset",	\
			"MPI_File_get_position",	\
			"MPI_File_get_position_shared",	\
			"MPI_File_get_type_extent",	\
			"MPI_File_set_size",		\
			"MPI_File_preallocate",		\
			"MPI_File_set_info",		\
			"MPI_File_get_view",		\
			"MPI_File_set_view",		\
			"MPI_File_set_atomicity",	\
			"MPI_File_get_atomicity"	\
			}
#define SEEK_LEN 21

#define CLOSE_ID  5
#define CLOSE_MEMBERS {	"close",		\
			"fclose",		\
			"unlink",		\
			"MPI_File_clsoe",	\
			"MPI_FILE_delete"	\
			}
#define CLOSE_LEN 5

#define LOCK_ID  6
#define LOCK_MEMBERS {	"flockfile",	\
			"ftrylockfile",	\
			"funlockfile",	\
			"lockf",	\
			"fcntl" 	\
			}
#define LOCK_LEN 5

#define STAT_ID 7
#define STAT_MEMBERS {	"stat",			\
			"fstat",		\
			"lstat",		\
			"MPI_File_get_size",	\
			"MPI_File_get_group",	\
			"MPI_File_get_amode",	\
			"MPI_File_get_info"	\
			}
#define STAT_LEN 7


struct otf_defs {
	char *version;
	char *creator;
	uint64_t resolution; 		// in ticks per second
	uint64_t range_min;
	uint64_t range_max;
	uint32_t *processes;
	uint32_t proc_count;
	struct finfo *functions;
	uint32_t func_count;
	uint32_t io_group;
	char **files;
	uint32_t file_count;
	struct keyvalue *keyvals;
	uint32_t kv_count;
};


struct keyvalue {
	char *name;
	char *desc;
	OTF_Type type;
};


struct finfo {
	char *name;
	uint32_t group;
};


struct siox_data {
	uint32_t ppn;		// process # per physical node
	siox_unid *nodes;
	uint32_t node_count;
};


struct ioop {
	uint32_t iocall;
	uint64_t time;
	uint32_t function;
	uint32_t group;
	uint32_t family;
	uint32_t fileid;
	uint32_t process;
	uint64_t handleid;
	uint32_t operation;
	uint32_t flags;
	uint64_t bytes;
	uint64_t duration;
	uint32_t source;
	siox_unid unid;
	char     *fname;
};


struct userdata {
	struct otf_defs *definitions;
	struct siox_data *siox;
	struct ioop *op;
};


/* I'll remove these global variables later. */
OTF_FileManager  *manager;
OTF_HandlerArray *handlers;
OTF_Reader       *reader;

int import_otf(const char *filename, const uint32_t maxfiles, 
	       const uint32_t ppn);

void open_otf(const char *filename, const uint32_t maxfiles);


void close_otf();


void count_definitions(const char *filename, const uint32_t maxfiles, 
		       struct otf_defs *definitions);

int inc_proc_count(struct otf_defs *definitions, uint32_t stream, 
		   uint32_t process, const char *name, uint32_t parent, 
		   OTF_KeyValueList *kvlist); 

int inc_file_count(struct otf_defs *definitions, uint32_t stream, 
		   uint32_t token, const char *name, uint32_t funcGroup, 
		   OTF_KeyValueList *kvlist);

int inc_function_count(struct otf_defs *definitions, uint32_t stream, 
		       uint32_t func, const char *name, uint32_t funcGroup, 
		       uint32_t source, OTF_KeyValueList *kvlist); 

int inc_keyvalue_count(struct otf_defs *definitions, uint32_t stream, 
		       uint32_t key, OTF_Type type, const char *name, 
		       const char *desc, OTF_KeyValueList *kvlist);

void load_definitions(const char *filename, const uint32_t maxfiles, 
		      struct otf_defs *definitions);

void allocate_def_arrays(struct otf_defs *definitions);


/* Handlers for the OTF Definitions */
int def_version(struct otf_defs *definitions, uint32_t stream, uint8_t major,
		uint8_t minor, uint8_t sub, const char *string, 
		OTF_KeyValueList *kvlist);

int def_creator(struct otf_defs *definitions, uint32_t stream, 
		const char *creator, OTF_KeyValueList *kvlist);

int def_resolution(struct otf_defs *definitions, uint32_t stream, 
		   uint64_t ticksPerSecond, OTF_KeyValueList *kvlist);

int def_range(struct otf_defs *definitions, uint32_t stream, uint64_t mintime,
	      uint64_t maxtime, OTF_KeyValueList *kvlist);

int def_function(struct otf_defs *definitions, uint32_t stream, uint32_t func, 
		 const char *name, uint32_t funcGroup, uint32_t source, 
		 OTF_KeyValueList *kvlist); 

int def_function_group(struct otf_defs *definitions, uint32_t stream, 
		       uint32_t funcGroup, const char *name, 
		       OTF_KeyValueList *kvlist);

int def_file(struct otf_defs *definitions, uint32_t stream, uint32_t token, 
	     const char *name, uint32_t funcGroup, OTF_KeyValueList *kvlist);

int def_process(struct otf_defs *definitions, uint32_t stream, uint32_t process,
		const char *name, uint32_t parent, OTF_KeyValueList *kvlist); 


int def_keyvalue(struct otf_defs *definitions, uint32_t stream, uint32_t key,
		 OTF_Type type, const char *name, const char *desc, 
		 OTF_KeyValueList *kvlist);


void register_nodes(struct otf_defs *definitions, struct siox_data *sdata, 
		    const uint32_t ppn);

void process_events(const char *filename, const uint32_t maxfiles, 
		    struct otf_defs *definitions, struct siox_data *sdata);


int event_enter(struct userdata *u, uint64_t time, uint32_t function, 
		uint32_t process, uint32_t source, OTF_KeyValueList *kvlist);

int event_leave(struct userdata *u, uint64_t time, uint32_t function, 
		uint32_t process, uint32_t source, OTF_KeyValueList *kvlist);

int event_io(struct userdata *u, uint64_t time, uint32_t fileid, 
	     uint32_t process, uint64_t handleid, uint32_t operation, 
	     uint64_t bytes, uint64_t duration, uint32_t source, 
	     OTF_KeyValueList *kvlist);

int event_begin_io(struct userdata *u, uint64_t time, uint32_t process, 
		   uint64_t matchingId, uint32_t scltoken, 
		   OTF_KeyValueList *kvlist);

int event_end_io(struct userdata *u, uint64_t time, uint32_t process, 
		 uint32_t fileid, uint64_t matchingId, uint64_t handleId, 
		 uint32_t operation, uint64_t bytes, uint32_t scltoken, 
		 OTF_KeyValueList *kvlist);

void hdler_close(struct userdata *u);
void hdler_lock(struct userdata *u);
void hdler_open(struct userdata *u);
void hdler_read(struct userdata *u);
void hdler_seek(struct userdata *u);
void hdler_stat(struct userdata *u);
void hdler_write(struct userdata *u);

uint32_t io_family(const char *member);
inline uint32_t is_mpi(const char *str);

void checkout(struct siox_data *sdata);
void murica(struct otf_defs *definitions, struct siox_data *sdtata);

#endif /* _LIBOTF2SIOX_H */