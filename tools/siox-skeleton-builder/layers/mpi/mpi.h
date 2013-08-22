//@include <mpi.h>

/* Set the interface name for the library*/
//@component "MPIIO"

/* Register the data types for the descriptors */
//@register_attribute bytesToRead "Bytes to read" SIOX_STORAGE_64_BIT_INTEGER
//@register_attribute bytesToWrite "Bytes to write" SIOX_STORAGE_64_BIT_INTEGER
//@register_descriptor fileName "File Name" SIOX_STORAGE_STRING
//@register_descriptor fileHandle "POSIX File Handle" SIOX_STORAGE_64_BIT_INTEGER

/* Register the metrics to grab the performance data */
//@register_metric bytesWritten "/Throughput/Write" SIOX_UNIT_BYTES SIOX_STORAGE_64_BIT_INTEGER SIOX_SCOPE_SUM
//@register_metric bytesRead "/Troughput/Read" SIOX_UNIT_BYTES SIOX_STORAGE_64_BIT_INTEGER SIOX_SCOPE_SUM

/* Prepare a (hash) map to link descriptors (in this case, of type int) to their activities.
   This is necessary for the horizontal linking of activities.
   The map and all other functions using it default to the same built-in name,
   so it can usually be omitted. */
//@horizontal_map_create_int
//@horizontal_map_create_str

/*------------------------------------------------------------------------------
End of global part
------------------------------------------------------------------------------*/

//@activity
//@error ''ret!=0'' ret
int MPI_Init( int * argc, char ** *argv );

//@activity
//@error ''ret!=0'' ret
int MPI_Finalize( void );

//@activity_with_hints
//@activity_link_int fh
//@error ''ret!=0'' ret
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
int MPI_File_open( MPI_Comm comm, char * filename, int amode, MPI_Info info, MPI_File * fh );
//@activity
//@horizontal_map_remove_int fh
//@error ''ret!=0'' ret
int MPI_File_close( MPI_File * fh );
//@activity
//@horizontal_map_put_str filename
//@error ''ret!=0'' ret
int MPI_File_delete( char * filename, MPI_Info info );
//@activity
//@activity_link_int fh
//@error ''ret!=0'' ret
int MPI_File_set_size( MPI_File fh, MPI_Offset size );
//@activity
//@activity_link_int fh
//@error ''ret!=0'' ret
int MPI_File_preallocate( MPI_File fh, MPI_Offset size );
//@activity
//@activity_link_int fh
//@error ''ret!=0'' ret
int MPI_File_get_size( MPI_File fh, MPI_Offset * size );
//@activity
//@activity_link_int fh
//@error ''ret!=0'' ret
int MPI_File_get_group( MPI_File fh, MPI_Group * group );
//@activity
//@activity_link_int fh
//@error ''ret!=0'' ret
int MPI_File_get_amode( MPI_File fh, int * amode );
//@activity
//@activity_link_int fh
//@error ''ret!=0'' ret
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
int MPI_File_set_info( MPI_File fh, MPI_Info info );
//@activity
//@activity_link_int fh
//@error ''ret!=0'' ret
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
int MPI_File_get_info( MPI_File fh, MPI_Info * info_used );
//@activity_with_hints
//@activity_link_int fh
//@error ''ret!=0'' ret
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
int MPI_File_set_view( MPI_File fh, MPI_Offset disp, MPI_Datatype etype, MPI_Datatype filetype, char * datarep, MPI_Info info );
//@activity
//@activity_link_int fh
//@error ''ret!=0'' ret
int MPI_File_get_view( MPI_File fh, MPI_Offset * disp, MPI_Datatype * etype, MPI_Datatype * filetype, char * datarep );
//@activity
//@activity_link_int fh
//@error ''ret!=0'' ret
int MPI_File_read_at( MPI_File fh, MPI_Offset offset, void * buf, int count, MPI_Datatype datatype, MPI_Status * status );
//@activity
//@activity_link_int fh
//@error ''ret!=0'' ret
int MPI_File_read_at_all( MPI_File fh, MPI_Offset offset, void * buf, int count, MPI_Datatype datatype, MPI_Status * status );
//@activity
//@activity_link_int fh
//@error ''ret!=0'' ret
int MPI_File_write_at( MPI_File fh, MPI_Offset offset, void * buf, int count, MPI_Datatype datatype, MPI_Status * status );
//@activity
//@activity_link_int fh
//@error ''ret!=0'' ret
int MPI_File_write_at_all( MPI_File fh, MPI_Offset offset, void * buf, int count, MPI_Datatype datatype, MPI_Status * status );
//@activity
//@activity_link_int fh
//@error ''ret!=0'' ret
int MPI_File_iread_at( MPI_File fh, MPI_Offset offset, void * buf, int count, MPI_Datatype datatype, MPI_Request * request );
//@activity
//@activity_link_int fh
//@error ''ret!=0'' ret
int MPI_File_iwrite_at( MPI_File fh, MPI_Offset offset, void * buf, int count, MPI_Datatype datatype, MPI_Request * request );
//@activity
//@activity_link_int fh
//@error ''ret!=0'' ret
int MPI_File_read( MPI_File fh, void * buf, int count, MPI_Datatype datatype, MPI_Status * status );
//@activity
//@activity_link_int fh
//@error ''ret!=0'' ret
int MPI_File_read_all( MPI_File fh, void * buf, int count, MPI_Datatype datatype, MPI_Status * status );
//@activity
//@activity_link_int fh
//@error ''ret!=0'' ret
int MPI_File_write( MPI_File fh, void * buf, int count, MPI_Datatype datatype, MPI_Status * status );
//@activity
//@activity_link_int fh
//@error ''ret!=0'' ret
int MPI_File_write_all( MPI_File fh, void * buf, int count, MPI_Datatype datatype, MPI_Status * status );
//@activity
//@activity_link_int fh
//@error ''ret!=0'' ret
int MPI_File_iread( MPI_File fh, void * buf, int count, MPI_Datatype datatype, MPI_Request * request );
//@activity
//@activity_link_int fh
//@error ''ret!=0'' ret
int MPI_File_iwrite( MPI_File fh, void * buf, int count, MPI_Datatype datatype, MPI_Request * request );
//@activity
//@activity_link_int fh
//@error ''ret!=0'' ret
int MPI_File_seek( MPI_File fh, MPI_Offset offset, int whence );
//@activity
//@activity_link_int fh
//@error ''ret!=0'' ret
int MPI_File_get_position( MPI_File fh, MPI_Offset * offset );
//@activity
//@activity_link_int fh
//@error ''ret!=0'' ret
int MPI_File_get_byte_offset( MPI_File fh, MPI_Offset offset, MPI_Offset * disp );
//@activity
//@activity_link_int fh
//@error ''ret!=0'' ret
int MPI_File_read_shared( MPI_File fh, void * buf, int count, MPI_Datatype datatype, MPI_Status * status );
//@activity
//@activity_link_int fh
//@error ''ret!=0'' ret
int MPI_File_write_shared( MPI_File fh, void * buf, int count, MPI_Datatype datatype, MPI_Status * status );
//@activity
//@activity_link_int fh
//@error ''ret!=0'' ret
int MPI_File_iread_shared( MPI_File fh, void * buf, int count, MPI_Datatype datatype, MPI_Request * request );
//@activity
//@activity_link_int fh
//@error ''ret!=0'' ret
int MPI_File_iwrite_shared( MPI_File fh, void * buf, int count, MPI_Datatype datatype, MPI_Request * request );
//@activity
//@activity_link_int fh
//@error ''ret!=0'' ret
int MPI_File_read_ordered( MPI_File fh, void * buf, int count, MPI_Datatype datatype, MPI_Status * status );
//@activity
//@activity_link_int fh
//@error ''ret!=0'' ret
int MPI_File_write_ordered( MPI_File fh, void * buf, int count, MPI_Datatype datatype, MPI_Status * status );
//@activity
//@activity_link_int fh
//@error ''ret!=0'' ret
int MPI_File_seek_shared( MPI_File fh, MPI_Offset offset, int whence );
//@activity
//@activity_link_int fh
//@error ''ret!=0'' ret
int MPI_File_get_position_shared( MPI_File fh, MPI_Offset * offset );
//@activity
//@activity_link_int fh
//@error ''ret!=0'' ret
int MPI_File_read_at_all_begin( MPI_File fh, MPI_Offset offset, void * buf, int count, MPI_Datatype datatype );
//@activity
//@activity_link_int fh
//@error ''ret!=0'' ret
int MPI_File_read_at_all_end( MPI_File fh, void * buf, MPI_Status * status );
//@activity
//@activity_link_int fh
//@error ''ret!=0'' ret
int MPI_File_write_at_all_begin( MPI_File fh, MPI_Offset offset, void * buf, int count, MPI_Datatype datatype );
//@activity
//@activity_link_int fh
//@error ''ret!=0'' ret
int MPI_File_write_at_all_end( MPI_File fh, void * buf, MPI_Status * status );
//@activity
//@activity_link_int fh
//@error ''ret!=0'' ret
int MPI_File_read_all_begin( MPI_File fh, void * buf, int count, MPI_Datatype datatype );
//@activity
//@activity_link_int fh
//@error ''ret!=0'' ret
int MPI_File_read_all_end( MPI_File fh, void * buf, MPI_Status * status );
//@activity
//@activity_link_int fh
//@error ''ret!=0'' ret
int MPI_File_write_all_begin( MPI_File fh, void * buf, int count, MPI_Datatype datatype );
//@activity
//@activity_link_int fh
//@error ''ret!=0'' ret
int MPI_File_write_all_end( MPI_File fh, void * buf, MPI_Status * status );
//@activity
//@activity_link_int fh
//@error ''ret!=0'' ret
int MPI_File_read_ordered_begin( MPI_File fh, void * buf, int count, MPI_Datatype datatype );
//@activity
//@activity_link_int fh
//@error ''ret!=0'' ret
int MPI_File_read_ordered_end( MPI_File fh, void * buf, MPI_Status * status );
//@activity
//@activity_link_int fh
//@error ''ret!=0'' ret
int MPI_File_write_ordered_begin( MPI_File fh, void * buf, int count, MPI_Datatype datatype );
//@activity
//@activity_link_int fh
//@error ''ret!=0'' ret
int MPI_File_write_ordered_end( MPI_File fh, void * buf, MPI_Status * status );
//@activity
//@activity_link_int fh
//@error ''ret!=0'' ret
int MPI_File_get_type_extent( MPI_File fh, MPI_Datatype datatype, MPI_Aint * extent );
//@activity
//@activity_link_int fh
//@error ''ret!=0'' ret
int MPI_File_set_atomicity( MPI_File fh, int flag );
//@activity
//@activity_link_int fh
//@error ''ret!=0'' ret
int MPI_File_get_atomicity( MPI_File fh, int * flag );
//@activity
//@activity_link_int fh
//@error ''ret!=0'' ret
int MPI_File_sync( MPI_File fh );


//@activity
//@horizontal_map_create_str info
//@error ''ret!=0'' ret
//TODO Shall we define an array for info?
/*
    MPI_Info_create creates a new info object. The newly created
    object contains no key/value pairs.
*/
int MPI_Info_create( MPI_Info * info );
//@activity
//@horizontal_map_remove_str info
//@error ''ret!=0'' ret
//TODO Shall we define an array for info?
/*
    MPI_Info_delete deletes a (key,value) pair from info. If key is not
    defined in info, the call raises an error of class MPI_ERR_INFO_NOKEY.
*/
int MPI_Info_delete( MPI_Info info, char * key );
//@activity
//@activity_link_str info
//@error ''ret!=0'' ret
//TODO Shall we define an array for info?
/*
    MPI_Info_dup duplicates an existing info object, creating a new object,
    with the same (key,value) pairs and the same ordering of keys.
*/
int MPI_Info_dup( MPI_Info info, MPI_Info * newinfo );

//@activity
//@horizontal_map_remove_str info
//@error ''ret!=0'' ret
//TODO Shall we define an array for info?
/*
    MPI_Info_free frees info and sets it to MPI_INFO_NULL.
*/
int MPI_Info_free( MPI_Info * info );
//@activity
//@activity_link_str info
//@error ''ret!=0'' ret
//TODO Shall we define an array for info?
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
//@activity_link_str info
//@error ''ret!=0'' ret
//TODO Shall we define an array for info?
/*
    MPI_Info_set adds the (key,value) pair to info and overrides the value
    if a value for the same key was previously set. The key and value
    parameters are null-terminated strings in C. In Fortran, leading and
    trailing spaces in key and value are stripped. If either key or value is
    larger than the allowed maximums, the error MPI_ERR_INFO_KEY or
    MPI_ERR_INFO_VALUE is raised, respectively.
*/
int MPI_Info_set( MPI_Info info, char * key, char * value );
