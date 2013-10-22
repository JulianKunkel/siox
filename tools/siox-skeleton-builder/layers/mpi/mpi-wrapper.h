#include "mpi-helper.h"

/* Set the interface name for the library*/
//@component "MPI" implVersion "" int major, minor; char implVersion[151]; int v_ret = MPI_Get_version(& major, & minor); if (v_ret >= 0) snprintf(implVersion, 150, "%d.%d", major, minor); else sprintf(implVersion, "0");

//@register_attribute commSize "MPI" "description/commSize" SIOX_STORAGE_32_BIT_UINTEGER
//@register_attribute commRank "MPI" "description/commRank" SIOX_STORAGE_32_BIT_UINTEGER
//@register_attribute threadLevelRequired "MPI" "description/threadLevelRequired" SIOX_STORAGE_32_BIT_UINTEGER
//@register_attribute pidRank0 "MPI" "description/pidRank0" SIOX_STORAGE_64_BIT_UINTEGER

/* Register the data types for the descriptors */
//@register_attribute bytesToRead "MPI" "quantity/BytesToRead" SIOX_STORAGE_64_BIT_UINTEGER
//@register_attribute bytesToWrite "MPI" "quantity/BytesToWrite" SIOX_STORAGE_64_BIT_UINTEGER
//@register_attribute filePosition "MPI" "file/position" SIOX_STORAGE_64_BIT_UINTEGER
//@register_attribute fileExtent "MPI" "file/extent" SIOX_STORAGE_64_BIT_UINTEGER
//@register_attribute fileHandle "POSIX" "descriptor/filehandle" SIOX_STORAGE_64_BIT_UINTEGER

//@register_attribute fileOpenFlags "MPI" "hints/openFlags" SIOX_STORAGE_32_BIT_UINTEGER

//@register_attribute infoReadBuffSize "MPI" "hints/noncollReadBuffSize" SIOX_STORAGE_64_BIT_UINTEGER
//@register_attribute infoWriteBuffSize "MPI" "hints/noncollWriteBuffSize" SIOX_STORAGE_64_BIT_UINTEGER
//@register_attribute infoCollReadBuffSize "MPI" "hints/collReadBuffSize" SIOX_STORAGE_64_BIT_UINTEGER
//@register_attribute infoCollWriteBuffSize "MPI" "hints/collWriteBuffSize" SIOX_STORAGE_64_BIT_UINTEGER
//@register_attribute infoConcurrency "MPI" "hints/ioconcurrency" SIOX_STORAGE_32_BIT_UINTEGER
//@register_attribute infoCollContiguous "MPI" "hints/collContiguous" SIOX_STORAGE_32_BIT_UINTEGER
//@register_attribute infoString "MPI" "hints/info" SIOX_STORAGE_STRING

//@register_attribute fileName "MPI" "descriptor/filename" SIOX_STORAGE_STRING
//@register_attribute fileSystem "Global" "descriptor/filesystem" SIOX_STORAGE_32_BIT_UINTEGER

/* Prepare a (hash) map to link descriptors (in this case, of type int) to their activities.
   This is necessary for the horizontal linking of activities.
   The map and all other functions using it default to the same built-in name,
   so it can usually be omitted. */
//@horizontal_map_create_size

/*------------------------------------------------------------------------------
End of global part
------------------------------------------------------------------------------*/


// In MPI_Init() we will determine global MPI parameters and set them as component attributes.
// To link the processes together, Rank 0 broadcasts its pid to all other processes.
// All processes record the pid of Rank 0 then.

//@activity
//@error ''ret!=MPI_SUCCESS'' ret
//@component_attribute commSize mpi_sz  int mpi_sz; PMPI_Comm_size(MPI_COMM_WORLD, & mpi_sz);
//@component_attribute commRank mpi_rank  int mpi_rank; PMPI_Comm_rank(MPI_COMM_WORLD, & mpi_rank);
//@component_attribute pidRank0 pid uint64_t pid = (uint64_t) getpid(); PMPI_Bcast(& pid, 1, MPI_LONG, 0, MPI_COMM_WORLD);
int MPI_Init( int * argc, char ** *argv );


//@activity
//@error ''ret!=MPI_SUCCESS'' ret
//@component_attribute commSize mpi_sz  int mpi_sz; MPI_Comm_size(MPI_COMM_WORLD, & mpi_sz);
//@component_attribute commRank mpi_rank  int mpi_rank; MPI_Comm_rank(MPI_COMM_WORLD, & mpi_rank);
//@splice_before int32_t translatedFlags = translateMPIThreadLevelToSIOX(required);
//@component_attribute threadLevelRequired translatedFlags
//@component_attribute pidRank0 pid uint64_t pid = (uint64_t) getpid(); PMPI_Bcast(& pid, 1, MPI_LONG, 0, MPI_COMM_WORLD);
int MPI_Init_thread( int *argc, char ** *argv, int required, int *provided );

//@activity
//@error ''ret!=MPI_SUCCESS'' ret
int MPI_Finalize( void );

/*
    MPI_Info list:

    SETTABLE HINTS:
    - MPI_INFO_NULL
    - shared_file_timeout: Amount of time (in seconds) to wait for access to
        the shared file pointer before exiting with MPI_ERR_TIMEDOUT.
    - rwlock_timeout: Amount of time (in seconds) to wait for obtaining a read
        or write lock on a contiguous chunk of a UNIX file before exiting with
        MPI_ERR_TIMEDOUT.
    - noncoll_read_bufsize: Maximum size of the buffer used by MPI I/O to
        satisfy multiple noncontiguous read requests in the noncollective
        data-access routines. (See NOTE, below.)
    - noncoll_write_bufsize: Maximum size of the buffer used by MPI I/O to
        satisfy multiple noncontiguous write requests in the noncollective
        data-access routines. (See NOTE, below.)
    - coll_read_bufsize: Maximum size of the buffer used by MPI I/O to satisfy
        multiple noncontiguous read requests in the collective data-access
        routines. (See NOTE, below.)
    - coll_write_bufsize: Maximum size of the buffer used by MPI I/O to satisfy
        multiple noncontiguous write requests in the collective data-access
        routines. (See NOTE, below.)

    NOTE: A buffer size smaller than the distance (in bytes) in a UNIX file
    between the first byte and the last byte of the access request causes MPI
    I/O to iterate and perform multiple UNIX read() or write() calls. If the
    request includes multiple noncontiguous chunks of data, and the buffer size
    is greater than the size of those chunks, then the UNIX read() or write()
    (made at the MPI I/O level) will access data not requested by this process
    in order to reduce the total number of write() calls made. If this is not
    desirable behavior, you should reduce this buffer size to equal the size
    of the contiguous chunks within the aggregate request.

    - mpiio_concurrency: (boolean) controls whether nonblocking I/O routines
        can bind an extra thread to an LWP.
    - mpiio_coll_contiguous: (boolean) controls whether subsequent collective
        data accesses will request collectively contiguous regions of the file.

    NON-SETTABLE HINTS:
    - filename: Access this hint to get the name of the file.
*/
//@activity
//@activity_attribute_pointer fileName filename
//@splice_before uint32_t translatedFlags = translateMPIOpenFlagsToSIOX(amode);
//@activity_attribute fileOpenFlags translatedFlags
//@activity_attribute fileHandle fh
//@activity_attribute_pointer fileName filename
//@horizontal_map_put_size fh
//@splice_after recordDefaultInfo(sioxActivity, info);
//@error ''ret != MPI_SUCCESS '' ret
int MPI_File_open( MPI_Comm comm, char * filename, int amode, MPI_Info info, MPI_File * fh );

//@activity
//@horizontal_map_remove_size fh
//@error ''ret!=MPI_SUCCESS'' ret
int MPI_File_close( MPI_File * fh );

//@activity
//@activity_attribute_pointer fileName filename
//@error ''ret!=MPI_SUCCESS'' ret
int MPI_File_delete( char * filename, MPI_Info info );

//@activity
//@activity_link_size &fh
//@error ''ret!=MPI_SUCCESS'' ret
int MPI_File_set_size( MPI_File fh, MPI_Offset size );

//@activity
//@activity_link_size &fh
//@error ''ret!=MPI_SUCCESS'' ret
int MPI_File_preallocate( MPI_File fh, MPI_Offset size );

//@activity
//@activity_link_size &fh
//@error ''ret!=MPI_SUCCESS'' ret
int MPI_File_get_size( MPI_File fh, MPI_Offset * size );

//@activity
//@activity_link_size &fh
//@error ''ret!=MPI_SUCCESS'' ret
int MPI_File_get_group( MPI_File fh, MPI_Group * group );

//@activity
//@activity_link_size &fh
//@error ''ret!=MPI_SUCCESS'' ret
int MPI_File_get_amode( MPI_File fh, int * amode );

//TODO Shall we define an array for info?
/*
    MPI_Info list:

    SETTABLE HINTS:
    - MPI_INFO_NULL
    - shared_file_timeout: Amount of time (in seconds) to wait for access to
        the shared file pointer before exiting with MPI_ERR_TIMEDOUT.
    - rwlock_timeout: Amount of time (in seconds) to wait for obtaining a read
        or write lock on a contiguous chunk of a UNIX file before exiting with
        MPI_ERR_TIMEDOUT.
    - noncoll_read_bufsize: Maximum size of the buffer used by MPI I/O to
        satisfy multiple noncontiguous read requests in the noncollective
        data-access routines. (See NOTE, below.)
    - noncoll_write_bufsize: Maximum size of the buffer used by MPI I/O to
        satisfy multiple noncontiguous write requests in the noncollective
        data-access routines. (See NOTE, below.)
    - coll_read_bufsize: Maximum size of the buffer used by MPI I/O to satisfy
        multiple noncontiguous read requests in the collective data-access
        routines. (See NOTE, below.)
    - coll_write_bufsize: Maximum size of the buffer used by MPI I/O to satisfy
        multiple noncontiguous write requests in the collective data-access
        routines. (See NOTE, below.)

    NOTE: A buffer size smaller than the distance (in bytes) in a UNIX file
    between the first byte and the last byte of the access request causes MPI
    I/O to iterate and perform multiple UNIX read() or write() calls. If the
    request includes multiple noncontiguous chunks of data, and the buffer size
    is greater than the size of those chunks, then the UNIX read() or write()
    (made at the MPI I/O level) will access data not requested by this process
    in order to reduce the total number of write() calls made. If this is not
    desirable behavior, you should reduce this buffer size to equal the size
    of the contiguous chunks within the aggregate request.

    - mpiio_concurrency: (boolean) controls whether nonblocking I/O routines
        can bind an extra thread to an LWP.
    - mpiio_coll_contiguous: (boolean) controls whether subsequent collective
        data accesses will request collectively contiguous regions of the file.

    NON-SETTABLE HINTS:
    - filename: Access this hint to get the name of the file.
*/

//@activity
//@activity_link_size &fh
//@splice_after recordDefaultInfo(sioxActivity, info);
//@error ''ret!=MPI_SUCCESS'' ret
int MPI_File_set_info( MPI_File fh, MPI_Info info );

//TODO Shall we define an array for info?
/*
    MPI_Info list:

    SETTABLE HINTS:
    - MPI_INFO_NULL
    - shared_file_timeout: Amount of time (in seconds) to wait for access to
        the shared file pointer before exiting with MPI_ERR_TIMEDOUT.
    - rwlock_timeout: Amount of time (in seconds) to wait for obtaining a read
        or write lock on a contiguous chunk of a UNIX file before exiting with
        MPI_ERR_TIMEDOUT.
    - noncoll_read_bufsize: Maximum size of the buffer used by MPI I/O to
        satisfy multiple noncontiguous read requests in the noncollective
        data-access routines. (See NOTE, below.)
    - noncoll_write_bufsize: Maximum size of the buffer used by MPI I/O to
        satisfy multiple noncontiguous write requests in the noncollective
        data-access routines. (See NOTE, below.)
    - coll_read_bufsize: Maximum size of the buffer used by MPI I/O to satisfy
        multiple noncontiguous read requests in the collective data-access
        routines. (See NOTE, below.)
    - coll_write_bufsize: Maximum size of the buffer used by MPI I/O to satisfy
        multiple noncontiguous write requests in the collective data-access
        routines. (See NOTE, below.)

    NOTE: A buffer size smaller than the distance (in bytes) in a UNIX file
    between the first byte and the last byte of the access request causes MPI
    I/O to iterate and perform multiple UNIX read() or write() calls. If the
    request includes multiple noncontiguous chunks of data, and the buffer size
    is greater than the size of those chunks, then the UNIX read() or write()
    (made at the MPI I/O level) will access data not requested by this process
    in order to reduce the total number of write() calls made. If this is not
    desirable behavior, you should reduce this buffer size to equal the size
    of the contiguous chunks within the aggregate request.

    - mpiio_concurrency: (boolean) controls whether nonblocking I/O routines
        can bind an extra thread to an LWP.
    - mpiio_coll_contiguous: (boolean) controls whether subsequent collective
        data accesses will request collectively contiguous regions of the file.

    NON-SETTABLE HINTS:
    - filename: Access this hint to get the name of the file.
*/

//@activity
//@activity_link_size &fh
//@error ''ret!=MPI_SUCCESS'' ret
int MPI_File_get_info( MPI_File fh, MPI_Info * info_used );

/*
    MPI_Info list:

    SETTABLE HINTS:
    - MPI_INFO_NULL
    - shared_file_timeout: Amount of time (in seconds) to wait for access to
        the shared file pointer before exiting with MPI_ERR_TIMEDOUT.
    - rwlock_timeout: Amount of time (in seconds) to wait for obtaining a read
        or write lock on a contiguous chunk of a UNIX file before exiting with
        MPI_ERR_TIMEDOUT.
    - noncoll_read_bufsize: Maximum size of the buffer used by MPI I/O to
        satisfy multiple noncontiguous read requests in the noncollective
        data-access routines. (See NOTE, below.)
    - noncoll_write_bufsize: Maximum size of the buffer used by MPI I/O to
        satisfy multiple noncontiguous write requests in the noncollective
        data-access routines. (See NOTE, below.)
    - coll_read_bufsize: Maximum size of the buffer used by MPI I/O to satisfy
        multiple noncontiguous read requests in the collective data-access
        routines. (See NOTE, below.)
    - coll_write_bufsize: Maximum size of the buffer used by MPI I/O to satisfy
        multiple noncontiguous write requests in the collective data-access
        routines. (See NOTE, below.)

    NOTE: A buffer size smaller than the distance (in bytes) in a UNIX file
    between the first byte and the last byte of the access request causes MPI
    I/O to iterate and perform multiple UNIX read() or write() calls. If the
    request includes multiple noncontiguous chunks of data, and the buffer size
    is greater than the size of those chunks, then the UNIX read() or write()
    (made at the MPI I/O level) will access data not requested by this process
    in order to reduce the total number of write() calls made. If this is not
    desirable behavior, you should reduce this buffer size to equal the size
    of the contiguous chunks within the aggregate request.

    - mpiio_concurrency: (boolean) controls whether nonblocking I/O routines
        can bind an extra thread to an LWP.
    - mpiio_coll_contiguous: (boolean) controls whether subsequent collective
        data accesses will request collectively contiguous regions of the file.

    NON-SETTABLE HINTS:
    - filename: Access this hint to get the name of the file.
*/

//@activity
//@activity_link_size &fh
//@error ''ret!=MPI_SUCCESS'' ret
int MPI_File_set_view( MPI_File fh, MPI_Offset disp, MPI_Datatype etype, MPI_Datatype filetype, char * datarep, MPI_Info info );

//@activity
//@activity_link_size &fh
//@error ''ret!=MPI_SUCCESS'' ret
int MPI_File_get_view( MPI_File fh, MPI_Offset * disp, MPI_Datatype * etype, MPI_Datatype * filetype, char * datarep );

//@activity
//@activity_link_size &fh
//@error ''ret!=MPI_SUCCESS'' ret
int MPI_File_read_at( MPI_File fh, MPI_Offset offset, void * buf, int count, MPI_Datatype datatype, MPI_Status * status );

//@activity
//@activity_link_size &fh
//@error ''ret!=MPI_SUCCESS'' ret
int MPI_File_read_at_all( MPI_File fh, MPI_Offset offset, void * buf, int count, MPI_Datatype datatype, MPI_Status * status );

//@activity
//@activity_link_size &fh
//@error ''ret!=MPI_SUCCESS'' ret
int MPI_File_write_at( MPI_File fh, MPI_Offset offset, void * buf, int count, MPI_Datatype datatype, MPI_Status * status );

//@activity
//@activity_link_size &fh
//@error ''ret!=MPI_SUCCESS'' ret
int MPI_File_write_at_all( MPI_File fh, MPI_Offset offset, void * buf, int count, MPI_Datatype datatype, MPI_Status * status );

//@activity
//@activity_link_size &fh
//@error ''ret!=MPI_SUCCESS'' ret
int MPI_File_iread_at( MPI_File fh, MPI_Offset offset, void * buf, int count, MPI_Datatype datatype, MPI_Request * request );

//@activity
//@activity_link_size &fh
//@error ''ret!=MPI_SUCCESS'' ret
int MPI_File_iwrite_at( MPI_File fh, MPI_Offset offset, void * buf, int count, MPI_Datatype datatype, MPI_Request * request );

//@activity
//@activity_link_size &fh
//@error ''ret!=MPI_SUCCESS'' ret
int MPI_File_read( MPI_File fh, void * buf, int count, MPI_Datatype datatype, MPI_Status * status );

//@activity
//@activity_link_size &fh
//@error ''ret!=MPI_SUCCESS'' ret
int MPI_File_read_all( MPI_File fh, void * buf, int count, MPI_Datatype datatype, MPI_Status * status );

//@activity
//@activity_link_size &fh
//@error ''ret!=MPI_SUCCESS'' ret
int MPI_File_write( MPI_File fh, void * buf, int count, MPI_Datatype datatype, MPI_Status * status );

//@activity
//@activity_link_size &fh
//@error ''ret!=MPI_SUCCESS'' ret
int MPI_File_write_all( MPI_File fh, void * buf, int count, MPI_Datatype datatype, MPI_Status * status );

//@activity
//@activity_link_size &fh
//@error ''ret!=MPI_SUCCESS'' ret
int MPI_File_iread( MPI_File fh, void * buf, int count, MPI_Datatype datatype, MPI_Request * request );

//@activity
//@activity_link_size &fh
//@error ''ret!=MPI_SUCCESS'' ret
int MPI_File_iwrite( MPI_File fh, void * buf, int count, MPI_Datatype datatype, MPI_Request * request );

//@activity
//@activity_link_size &fh
//@error ''ret!=MPI_SUCCESS'' ret
int MPI_File_seek( MPI_File fh, MPI_Offset offset, int whence );

//@activity
//@activity_link_size &fh
//@error ''ret!=MPI_SUCCESS'' ret
int MPI_File_get_position( MPI_File fh, MPI_Offset * offset );

//@activity
//@activity_link_size &fh
//@error ''ret!=MPI_SUCCESS'' ret
int MPI_File_get_byte_offset( MPI_File fh, MPI_Offset offset, MPI_Offset * disp );

//@activity
//@activity_link_size &fh
//@error ''ret!=MPI_SUCCESS'' ret
int MPI_File_read_shared( MPI_File fh, void * buf, int count, MPI_Datatype datatype, MPI_Status * status );

//@activity
//@activity_link_size &fh
//@error ''ret!=MPI_SUCCESS'' ret
int MPI_File_write_shared( MPI_File fh, void * buf, int count, MPI_Datatype datatype, MPI_Status * status );

//@activity
//@activity_link_size &fh
//@error ''ret!=MPI_SUCCESS'' ret
int MPI_File_iread_shared( MPI_File fh, void * buf, int count, MPI_Datatype datatype, MPI_Request * request );

//@activity
//@activity_link_size &fh
//@error ''ret!=MPI_SUCCESS'' ret
int MPI_File_iwrite_shared( MPI_File fh, void * buf, int count, MPI_Datatype datatype, MPI_Request * request );

//@activity
//@activity_link_size &fh
//@error ''ret!=MPI_SUCCESS'' ret
int MPI_File_read_ordered( MPI_File fh, void * buf, int count, MPI_Datatype datatype, MPI_Status * status );

//@activity
//@activity_link_size &fh
//@error ''ret!=MPI_SUCCESS'' ret
int MPI_File_write_ordered( MPI_File fh, void * buf, int count, MPI_Datatype datatype, MPI_Status * status );

//@activity
//@activity_link_size &fh
//@error ''ret!=MPI_SUCCESS'' ret
int MPI_File_seek_shared( MPI_File fh, MPI_Offset offset, int whence );

//@activity
//@activity_link_size &fh
//@error ''ret!=MPI_SUCCESS'' ret
int MPI_File_get_position_shared( MPI_File fh, MPI_Offset * offset );

//@activity
//@activity_link_size &fh
//@error ''ret!=MPI_SUCCESS'' ret
int MPI_File_read_at_all_begin( MPI_File fh, MPI_Offset offset, void * buf, int count, MPI_Datatype datatype );

//@activity
//@activity_link_size &fh
//@error ''ret!=MPI_SUCCESS'' ret
int MPI_File_read_at_all_end( MPI_File fh, void * buf, MPI_Status * status );

//@activity
//@activity_link_size &fh
//@error ''ret!=MPI_SUCCESS'' ret
int MPI_File_write_at_all_begin( MPI_File fh, MPI_Offset offset, void * buf, int count, MPI_Datatype datatype );

//@activity
//@activity_link_size &fh
//@error ''ret!=MPI_SUCCESS'' ret
int MPI_File_write_at_all_end( MPI_File fh, void * buf, MPI_Status * status );

//@activity
//@activity_link_size &fh
//@error ''ret!=MPI_SUCCESS'' ret
int MPI_File_read_all_begin( MPI_File fh, void * buf, int count, MPI_Datatype datatype );

//@activity
//@activity_link_size &fh
//@error ''ret!=MPI_SUCCESS'' ret
int MPI_File_read_all_end( MPI_File fh, void * buf, MPI_Status * status );

//@activity
//@activity_link_size &fh
//@error ''ret!=MPI_SUCCESS'' ret
int MPI_File_write_all_begin( MPI_File fh, void * buf, int count, MPI_Datatype datatype );

//@activity
//@activity_link_size &fh
//@error ''ret!=MPI_SUCCESS'' ret
int MPI_File_write_all_end( MPI_File fh, void * buf, MPI_Status * status );

//@activity
//@activity_link_size &fh
//@error ''ret!=MPI_SUCCESS'' ret
int MPI_File_read_ordered_begin( MPI_File fh, void * buf, int count, MPI_Datatype datatype );

//@activity
//@activity_link_size &fh
//@error ''ret!=MPI_SUCCESS'' ret
int MPI_File_read_ordered_end( MPI_File fh, void * buf, MPI_Status * status );

//@activity
//@activity_link_size &fh
//@error ''ret!=MPI_SUCCESS'' ret
int MPI_File_write_ordered_begin( MPI_File fh, void * buf, int count, MPI_Datatype datatype );

//@activity
//@activity_link_size &fh
//@error ''ret!=MPI_SUCCESS'' ret
int MPI_File_write_ordered_end( MPI_File fh, void * buf, MPI_Status * status );

//@activity
//@activity_link_size &fh
//@error ''ret!=MPI_SUCCESS'' ret
int MPI_File_get_type_extent( MPI_File fh, MPI_Datatype datatype, MPI_Aint * extent );

//@activity
//@activity_link_size &fh
//@error ''ret!=MPI_SUCCESS'' ret
int MPI_File_set_atomicity( MPI_File fh, int flag );

//@activity
//@activity_link_size &fh
//@error ''ret!=MPI_SUCCESS'' ret
int MPI_File_get_atomicity( MPI_File fh, int * flag );

//@activity
//@activity_link_size &fh
//@error ''ret!=MPI_SUCCESS'' ret
int MPI_File_sync( MPI_File fh );


//TODO Shall we define an array for info?
/*
    MPI_Info_create creates a new info object. The newly created
    object contains no key/value pairs.
*/
//@activity
//TODO Shall we define an array for info?
//@error ''ret!=MPI_SUCCESS'' ret
int MPI_Info_create( MPI_Info * info );

//@activity
//@error ''ret!=MPI_SUCCESS'' ret
/*
    MPI_Info_delete deletes a (key,value) pair from info. If key is not
    defined in info, the call raises an error of class MPI_ERR_INFO_NOKEY.
*/
int MPI_Info_delete( MPI_Info info, char * key );

//@activity
//@error ''ret!=MPI_SUCCESS'' ret
/*
    MPI_Info_dup duplicates an existing info object, creating a new object,
    with the same (key,value) pairs and the same ordering of keys.
*/
int MPI_Info_dup( MPI_Info info, MPI_Info * newinfo );

//@activity
//@error ''ret!=MPI_SUCCESS'' ret
/*
    MPI_Info_free frees info and sets it to MPI_INFO_NULL.
*/
int MPI_Info_free( MPI_Info * info );

//@activity
//@error ''ret!=MPI_SUCCESS'' ret
/*
    MPI_Info_get retrieves the value associated with key in a previous call
    to MPI_Info_set. If such a key exists, it sets flag to true and returns
    the value in value; otherwise it sets flag to false and leaves value
    unchanged. valuelen is the number of characters available in value. If
    it is less than the actual size of the value, the returned value is
    truncated. In C, valuelen should be one less than the amount of allocated
    space to allow for the null terminator.

    If key is larger than MPI_MAX_INFO_KEY, the call is erroneous.
*/
int MPI_Info_get( MPI_Info info, char * key, int valuelen, char * value, int * flag );

//@activity
//@error ''ret!=MPI_SUCCESS'' ret
/*
    MPI_Info_set adds the (key,value) pair to info and overrides the value
    if a value for the same key was previously set. The key and value
    parameters are null-terminated strings in C. In Fortran, leading and
    trailing spaces in key and value are stripped. If either key or value is
    larger than the allowed maximums, the error MPI_ERR_INFO_KEY or
    MPI_ERR_INFO_VALUE is raised, respectively.
*/
int MPI_Info_set( MPI_Info info, char * key, char * value );
