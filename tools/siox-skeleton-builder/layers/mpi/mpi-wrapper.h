/* Set the interface name for the library*/
//@component "MPI" "Generic"
// To record the version append:
// implVersion "" int major, minor; char implVersion[151]; int v_ret = MPI_Get_version(& major, & minor); if (v_ret >= 0) snprintf(implVersion, 150, "%d.%d", major, minor); else sprintf(implVersion, "0");

//@createInitializerForLibrary

//Generic MPI hints
//List of all processes in the communicator.
//@register_attribute commProcessesGroup "MPI" "description/group/rankInGroup" SIOX_STORAGE_32_BIT_UINTEGER
//@register_attribute commProcessesWorld "MPI" "description/group/rankInWorld" SIOX_STORAGE_32_BIT_UINTEGER
//@register_attribute commColor "MPI" "description/comm/split_color" SIOX_STORAGE_32_BIT_UINTEGER
//@register_attribute commKey "MPI" "description/comm/split_key" SIOX_STORAGE_32_BIT_UINTEGER
//@register_attribute commHandler "MPI" "description/comm/handler" SIOX_STORAGE_64_BIT_UINTEGER
//@register_attribute commHandlerOut "MPI" "description/comm/handlerOut" SIOX_STORAGE_64_BIT_UINTEGER

//File MPI hints
//@register_attribute commSize "MPI" "description/commSize" SIOX_STORAGE_32_BIT_UINTEGER
//@register_attribute commRank "MPI" "description/commRank" SIOX_STORAGE_32_BIT_UINTEGER
//@register_attribute threadLevelRequired "MPI" "description/threadLevelRequired" SIOX_STORAGE_32_BIT_UINTEGER
//@register_attribute pidRank0 "MPI" "description/pidRank0" SIOX_STORAGE_64_BIT_UINTEGER

/* Register the data types for the descriptors */
//@register_attribute bytesToRead "MPI" "quantity/BytesToRead" SIOX_STORAGE_64_BIT_UINTEGER
//@register_attribute bytesToWrite "MPI" "quantity/BytesToWrite" SIOX_STORAGE_64_BIT_UINTEGER
//@register_attribute bytesWritten "MPI" "quantity/BytesWritten" SIOX_STORAGE_64_BIT_UINTEGER
//@register_attribute bytesRead "MPI" "quantity/BytesRead" SIOX_STORAGE_64_BIT_UINTEGER
//@register_attribute filePosition "MPI" "file/position" SIOX_STORAGE_64_BIT_UINTEGER
//@register_attribute fileExtent "MPI" "file/extent" SIOX_STORAGE_64_BIT_UINTEGER
//@register_attribute fileHandle "MPI" "descriptor/filehandle" SIOX_STORAGE_64_BIT_UINTEGER

//@register_attribute fileDatarepresentation "MPI" "hints/datarepresentation" SIOX_STORAGE_STRING

//@register_attribute fileOpenFlags "MPI" "hints/openFlags" SIOX_STORAGE_32_BIT_UINTEGER

//@register_attribute hintFileSize "MPI" "hints/fileSize" SIOX_STORAGE_64_BIT_UINTEGER

//@register_attribute attribute_etype "MPI" "description/etype" SIOX_STORAGE_STRING

//@register_attribute attribute_filetype_id "MPI" "description/filetype/id" SIOX_STORAGE_64_BIT_UINTEGER
//@register_attribute attribute_filetype "MPI" "description/filetype" SIOX_STORAGE_STRING
//@register_attribute attribute_filetype_extent "MPI" "description/filetype/extent" SIOX_STORAGE_64_BIT_UINTEGER
//@register_attribute attribute_filetype_size "MPI" "description/filetype/size" SIOX_STORAGE_32_BIT_UINTEGER
//@register_attribute attribute_etype_extent "MPI" "description/etype/extent" SIOX_STORAGE_64_BIT_UINTEGER
//@register_attribute attribute_etype_size "MPI" "description/etype/size" SIOX_STORAGE_32_BIT_UINTEGER
//@register_attribute attribute_etype_id "MPI" "description/etype/id" SIOX_STORAGE_64_BIT_UINTEGER

//@register_attribute infoBuffSize "MPI" "hints/cbBuffSize" SIOX_STORAGE_64_BIT_UINTEGER
//@register_attribute infoReadBuffSize "MPI" "hints/noncollReadBuffSize" SIOX_STORAGE_64_BIT_UINTEGER
//@register_attribute infoWriteBuffSize "MPI" "hints/noncollWriteBuffSize" SIOX_STORAGE_64_BIT_UINTEGER
//@register_attribute infoCollReadBuffSize "MPI" "hints/collReadBuffSize" SIOX_STORAGE_64_BIT_UINTEGER
//@register_attribute infoCollWriteBuffSize "MPI" "hints/collWriteBuffSize" SIOX_STORAGE_64_BIT_UINTEGER
//@register_attribute infoROMIOCollReadEnabled "ROMIO" "hints/collRead" SIOX_STORAGE_STRING
//@register_attribute infoROMIOCollWriteEnabled "ROMIO" "hints/collWrite" SIOX_STORAGE_STRING
//@register_attribute infoOMPIOCollReadEnabled "OMPIO" "hints/collRead" SIOX_STORAGE_STRING
//@register_attribute infoOMPIOCollWriteEnabled "OMPIO" "hints/collWrite" SIOX_STORAGE_STRING
//@register_attribute infoConcurrency "MPI" "hints/ioconcurrency" SIOX_STORAGE_32_BIT_UINTEGER
//@register_attribute infoCollContiguous "MPI" "hints/collContiguous" SIOX_STORAGE_32_BIT_UINTEGER

//Contains all hints:
//@register_attribute infoString "MPI" "hints/info" SIOX_STORAGE_STRING


//@register_attribute fileName "MPI" "descriptor/filename" SIOX_STORAGE_STRING
/* Attributes for parallel filesystem, if known   ?  */
/* Attributes exist across layers */
//@register_attribute fileSystem "Global" "descriptor/filesystem" SIOX_STORAGE_32_BIT_UINTEGER

/* Prepare a (hash) map to link descriptors (in this case, of type int) to their activities.
   This is necessary for the horizontal linking of activities.
   The map and all other functions using it default to the same built-in name,
   so it can usually be omitted. */
/* Creates Hashmap with filehandles of activities while sizetype is the key in the hashmap. */
//@horizontal_map_create_size

//@include "mpi-helper.h"

/*------------------------------------------------------------------------------
End of global part
------------------------------------------------------------------------------*/

// In MPI_Init() we will determine global MPI parameters and set them as component attributes.
// To link the processes together, Rank 0 broadcasts its pid to all other processes.
// All processes record the pid of Rank 0 then.

#ifndef  OPTIONAL_CONST
#define  OPTIONAL_CONST 
#endif

//@callLibraryInitialize
//@MPI_activity
//@component_attribute commSize mpi_sz  int mpi_sz; MPI_Comm_size(MPI_COMM_WORLD, & mpi_sz);
//@component_attribute commRank mpi_rank  int mpi_rank; MPI_Comm_rank(MPI_COMM_WORLD, & mpi_rank);
//@component_attribute pidRank0 pid uint64_t pid = (uint64_t) getpid(); MPI_Bcast(& pid, 1, MPI_LONG, 0, MPI_COMM_WORLD);
int MPI_Init( int * argc, char ** *argv );

//@callLibraryInitialize
//@MPI_activity
//@component_attribute commSize mpi_sz  int mpi_sz; MPI_Comm_size(MPI_COMM_WORLD, & mpi_sz);
//@component_attribute commRank mpi_rank  int mpi_rank; MPI_Comm_rank(MPI_COMM_WORLD, & mpi_rank);
//@component_attribute threadLevelRequired translatedFlags int32_t translatedFlags = translateMPIThreadLevelToSIOX(required);
//@component_attribute pidRank0 pid uint64_t pid = (uint64_t) getpid(); MPI_Bcast(& pid, 1, MPI_LONG, 0, MPI_COMM_WORLD);
int MPI_Init_thread( int *argc, char ** *argv, int required, int *provided );


//callLibraryFinalize

//@activity
//@supressFunctionCall
//@splice_before siox_register_termination_complete_signal( (void (*)(void)) PMPI_Finalize);
//@splice_after ret = MPI_SUCCESS;
int MPI_Finalize( void );


//@activity
//@callLibraryFinalize
int MPI_Abort( MPI_Comm comm,  int errorcode );

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

//@MPI_activity
//@splice_before uint32_t translatedFlags = translateMPIOpenFlagsToSIOX(amode);
//@activity_attribute fileOpenFlags translatedFlags
//@activity_attribute_str fileName filename
//@MPI_comm_handle comm
//@splice_after setFileInfo(*fh, info);
//@horizontal_map_put_size *fh
//@splice_after recordFileInfo(sioxActivity, *fh);
//@activity_attribute_late fileHandle *fh
int MPI_File_open( MPI_Comm comm, OPTIONAL_CONST char * filename, int amode, MPI_Info info, MPI_File * fh );

//It is required to save the value of *fh prior to calling MPI_File_close, because some MPI implementations will set it to zero, so that we cannot use *fh afterwards to lookup the parent in the hash table or to link the close activity to its parent.
//@activity
//@splice_before MPI_File savedFh = *fh;
//@activity_link_size savedFh
//@horizontal_map_remove_size savedFh
//@MPI_error
int MPI_File_close( MPI_File * fh );

//@MPI_activity
//@activity_attribute_str fileName filename
int MPI_File_delete( OPTIONAL_CONST char * filename, MPI_Info info );

//@MPI_activity
//@activity_link_size fh
//@activity_attribute hintFileSize size
int MPI_File_set_size( MPI_File fh, MPI_Offset size );

//@MPI_activity
//@activity_link_size fh
//@activity_attribute hintFileSize size
int MPI_File_preallocate( MPI_File fh, MPI_Offset size );

//@MPI_activity
//@activity_link_size fh
int MPI_File_get_size( MPI_File fh, MPI_Offset * size );

//@MPI_activity
//@activity_link_size fh
int MPI_File_get_group( MPI_File fh, MPI_Group * group );

//@MPI_activity
//@activity_link_size fh
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

//@MPI_activity
//@activity_link_size fh
//@splice_after recordFileInfo(sioxActivity, fh);
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
//@activity_link_size fh
//@MPI_error
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
//@activity_link_size fh
//@splice_after if(info != MPI_INFO_NULL) { recordFileInfo(sioxActivity, fh); }
//@splice_before recordDatatype(sioxActivity, attribute_filetype, filetype);
//@splice_before recordDatatype(sioxActivity, attribute_etype, etype);
//@splice_before MPI_Aint tmp1, tmp2; uint32_t type_size; uint64_t type_extent; MPI_Type_get_extent(filetype, & tmp1, & tmp2); type_extent = (uint64_t) tmp2; int t1; MPI_Type_size(filetype, & t1); type_size = (uint32_t) t1;
//@activity_attribute attribute_filetype_size type_size
//@activity_attribute attribute_filetype_extent type_extent

//@splice_before MPI_Type_get_extent(filetype, & tmp1, & tmp2); type_extent = (uint64_t) tmp2; MPI_Type_size(filetype, & t1); type_size = (uint32_t) t1;
//@activity_attribute attribute_etype_size type_size
//@activity_attribute attribute_etype_extent type_extent

//@activity_attribute filePosition disp
//@activity_attribute_str fileDatarepresentation datarep
//@MPI_error

//@splice_before '' uint64_t typeID = getDatatypeHandle(filetype);'' 
//@activity_attribute attribute_filetype_id typeID

//@splice_before '' typeID = getDatatypeHandle(etype);'' 
//@activity_attribute attribute_etype_id typeID
int MPI_File_set_view( MPI_File fh, MPI_Offset disp, MPI_Datatype etype, MPI_Datatype filetype, OPTIONAL_CONST char * datarep, MPI_Info info );

//@activity
//@activity_link_size fh
//@MPI_error
int MPI_File_get_view( MPI_File fh, MPI_Offset * disp, MPI_Datatype * etype, MPI_Datatype * filetype, char * datarep );

//@activity
//@activity_link_size fh
//@activity_attribute filePosition offset
//@splice_before ''int intSize; MPI_Type_size(datatype, & intSize); uint64_t size = (uint64_t)intSize * (uint64_t)count;''
//@activity_attribute bytesToRead size
//@MPI_error
int MPI_File_read_at( MPI_File fh, MPI_Offset offset, void * buf, int count, MPI_Datatype datatype, MPI_Status * status );

//@activity
//@activity_link_size fh
//@activity_attribute filePosition offset
//@splice_before ''int intSize; MPI_Type_size(datatype, & intSize); uint64_t size = (uint64_t)intSize * (uint64_t)count;''
//@activity_attribute bytesToRead size
//@MPI_error
int MPI_File_read_at_all( MPI_File fh, MPI_Offset offset, void * buf, int count, MPI_Datatype datatype, MPI_Status * status );

//@activity
//@activity_link_size fh
//@activity_attribute filePosition offset
//@splice_before ''int intSize; MPI_Type_size(datatype, & intSize); uint64_t size = (uint64_t)intSize * (uint64_t)count;''
//@activity_attribute bytesToWrite size
//@MPI_error
int MPI_File_write_at( MPI_File fh, MPI_Offset offset, OPTIONAL_CONST void * buf, int count, MPI_Datatype datatype, MPI_Status * status );

//@activity
//@activity_link_size fh
//@activity_attribute filePosition offset
//@splice_before ''int intSize; MPI_Type_size(datatype, & intSize); uint64_t size = (uint64_t)intSize * (uint64_t)count;''
//@activity_attribute bytesToWrite size
//@MPI_error
int MPI_File_write_at_all( MPI_File fh, MPI_Offset offset, OPTIONAL_CONST void * buf, int count, MPI_Datatype datatype, MPI_Status * status );

//@activity
//@activity_link_size fh
//@activity_attribute filePosition offset
//@splice_before ''int intSize; MPI_Type_size(datatype, & intSize); uint64_t size = (uint64_t)intSize * (uint64_t)count;''
//@activity_attribute bytesToRead size
//@MPI_error
int MPI_File_iread_at( MPI_File fh, MPI_Offset offset, void * buf, int count, MPI_Datatype datatype, MPI_Request * request );

//@activity
//@activity_link_size fh
//@activity_attribute filePosition offset
//@splice_before ''int intSize; MPI_Type_size(datatype, & intSize); uint64_t size = (uint64_t)intSize * (uint64_t)count;''
//@activity_attribute bytesToWrite size
//@MPI_error
int MPI_File_iwrite_at( MPI_File fh, MPI_Offset offset, OPTIONAL_CONST void * buf, int count, MPI_Datatype datatype, MPI_Request * request );


//@activity
//@activity_link_size fh
//@splice_before ''MPI_Offset offset; MPI_File_get_position(fh, & offset); MPI_File_get_byte_offset(fh, offset, & offset);''
//@activity_attribute filePosition offset
//@splice_before ''int intSize; MPI_Type_size(datatype, & intSize); uint64_t size = (uint64_t)intSize * (uint64_t)count;''
//@activity_attribute bytesToRead size
//@MPI_error
int MPI_File_read( MPI_File fh, void * buf, int count, MPI_Datatype datatype, MPI_Status * status );

//@activity
//@activity_link_size fh
//@splice_before ''MPI_Offset offset; MPI_File_get_position(fh, & offset); MPI_File_get_byte_offset(fh, offset, & offset);''
//@activity_attribute filePosition offset
//@splice_before ''int intSize; MPI_Type_size(datatype, & intSize); uint64_t size = (uint64_t)intSize * (uint64_t)count;''
//@activity_attribute bytesToRead size
//@MPI_error
int MPI_File_read_all( MPI_File fh, void * buf, int count, MPI_Datatype datatype, MPI_Status * status );

//@activity
//@activity_link_size fh
//@splice_before ''MPI_Offset offset; MPI_File_get_position(fh, & offset); MPI_File_get_byte_offset(fh, offset, & offset);''
//@activity_attribute filePosition offset
//@splice_before ''int intSize; MPI_Type_size(datatype, & intSize); uint64_t size = (uint64_t)intSize * (uint64_t)count;''
//@activity_attribute bytesToWrite size
//@MPI_error
int MPI_File_write( MPI_File fh, OPTIONAL_CONST void * buf, int count, MPI_Datatype datatype, MPI_Status * status );

//@activity
//@activity_link_size fh
//@splice_before ''MPI_Offset offset; MPI_File_get_position(fh, & offset); MPI_File_get_byte_offset(fh, offset, & offset);''
//@activity_attribute filePosition offset
//@splice_before ''int intSize; MPI_Type_size(datatype, & intSize); uint64_t size = (uint64_t)intSize * (uint64_t)count;''
//@activity_attribute bytesToWrite size
//@MPI_error
int MPI_File_write_all( MPI_File fh, OPTIONAL_CONST void * buf, int count, MPI_Datatype datatype, MPI_Status * status );

//@activity
//@activity_link_size fh
//@splice_before ''MPI_Offset offset; MPI_File_get_position(fh, & offset); MPI_File_get_byte_offset(fh, offset, & offset);''
//@activity_attribute filePosition offset
//@splice_before ''int intSize; MPI_Type_size(datatype, & intSize); uint64_t size = (uint64_t)intSize * (uint64_t)count;''
//@activity_attribute bytesToRead size
//@MPI_error
int MPI_File_iread( MPI_File fh, void * buf, int count, MPI_Datatype datatype, MPI_Request * request );

//@activity
//@activity_link_size fh
//@splice_before ''MPI_Offset offset; MPI_File_get_position(fh, & offset); MPI_File_get_byte_offset(fh, offset, & offset);''
//@activity_attribute filePosition offset
//@splice_before ''int intSize; MPI_Type_size(datatype, & intSize); uint64_t size = (uint64_t)intSize * (uint64_t)count;''
//@activity_attribute bytesToWrite size
//@MPI_error
int MPI_File_iwrite( MPI_File fh, OPTIONAL_CONST void * buf, int count, MPI_Datatype datatype, MPI_Request * request );

//@activity
//@activity_link_size fh
//@MPI_error
int MPI_File_seek( MPI_File fh, MPI_Offset offset, int whence );

//@activity
//@activity_link_size fh
//@activity_attribute filePosition *offset
//@MPI_error
int MPI_File_get_position( MPI_File fh, MPI_Offset * offset );

//@activity
//@activity_link_size fh
//@MPI_error
int MPI_File_get_byte_offset( MPI_File fh, MPI_Offset offset, MPI_Offset * disp );

//@activity
//@activity_link_size fh
//@splice_before ''MPI_Offset offset; MPI_File_get_position_shared(fh, & offset); MPI_File_get_byte_offset(fh, offset, & offset);''
//@activity_attribute filePosition offset
//@splice_before ''int intSize; MPI_Type_size(datatype, & intSize); uint64_t size = (uint64_t)intSize * (uint64_t)count;''
//@activity_attribute bytesToRead size
//@MPI_error
int MPI_File_read_shared( MPI_File fh, void * buf, int count, MPI_Datatype datatype, MPI_Status * status );

//@activity
//@activity_link_size fh
//@splice_before ''MPI_Offset offset; MPI_File_get_position_shared(fh, & offset); MPI_File_get_byte_offset(fh, offset, & offset);''
//@activity_attribute filePosition offset
//@splice_before ''int intSize; MPI_Type_size(datatype, & intSize); uint64_t size = (uint64_t)intSize * (uint64_t)count;''
//@activity_attribute bytesToWrite size
//@MPI_error
int MPI_File_write_shared( MPI_File fh, OPTIONAL_CONST void * buf, int count, MPI_Datatype datatype, MPI_Status * status );

//@activity
//@activity_link_size fh
//@splice_before ''MPI_Offset offset; MPI_File_get_position_shared(fh, & offset); MPI_File_get_byte_offset(fh, offset, & offset);''
//@activity_attribute filePosition offset
//@splice_before ''int intSize; MPI_Type_size(datatype, & intSize); uint64_t size = (uint64_t)intSize * (uint64_t)count;''
//@activity_attribute bytesToRead size
//@MPI_error
int MPI_File_iread_shared( MPI_File fh, void * buf, int count, MPI_Datatype datatype, MPI_Request * request );

//@activity
//@activity_link_size fh
//@splice_before ''MPI_Offset offset; MPI_File_get_position_shared(fh, & offset); MPI_File_get_byte_offset(fh, offset, & offset);''
//@activity_attribute filePosition offset
//@splice_before ''int intSize; MPI_Type_size(datatype, & intSize); uint64_t size = (uint64_t)intSize * (uint64_t)count;''
//@activity_attribute bytesToWrite size
//@MPI_error
int MPI_File_iwrite_shared( MPI_File fh, OPTIONAL_CONST void * buf, int count, MPI_Datatype datatype, MPI_Request * request );

//@activity
//@activity_link_size fh

//@MPI_error
int MPI_File_read_ordered( MPI_File fh, void * buf, int count, MPI_Datatype datatype, MPI_Status * status );

//@activity
//@activity_link_size fh
//@MPI_error
int MPI_File_write_ordered( MPI_File fh, OPTIONAL_CONST void * buf, int count, MPI_Datatype datatype, MPI_Status * status );

//@activity
//@activity_link_size fh
//@MPI_error
int MPI_File_seek_shared( MPI_File fh, MPI_Offset offset, int whence );

//@activity
//@activity_link_size fh
//@MPI_error
int MPI_File_get_position_shared( MPI_File fh, MPI_Offset * offset );

//@activity
//@activity_link_size fh
//@splice_before ''MPI_File_get_byte_offset(fh, offset, & offset);''
//@activity_attribute filePosition offset
//@splice_before ''int intSize; MPI_Type_size(datatype, & intSize); uint64_t size = (uint64_t)intSize * (uint64_t)count;''
//@activity_attribute bytesToRead size
//@MPI_error
int MPI_File_read_at_all_begin( MPI_File fh, MPI_Offset offset, void * buf, int count, MPI_Datatype datatype );

//@activity
//@activity_link_size fh
//@MPI_error
int MPI_File_read_at_all_end( MPI_File fh, void * buf, MPI_Status * status );

//@activity
//@activity_link_size fh
//@splice_before ''MPI_File_get_byte_offset(fh, offset, & offset);''
//@activity_attribute filePosition offset
//@splice_before ''int intSize; MPI_Type_size(datatype, & intSize); uint64_t size = (uint64_t)intSize * (uint64_t)count;''
//@activity_attribute bytesToWrite size
//@MPI_error
int MPI_File_write_at_all_begin( MPI_File fh, MPI_Offset offset, OPTIONAL_CONST void * buf, int count, MPI_Datatype datatype );

//@activity
//@activity_link_size fh
//@MPI_error
int MPI_File_write_at_all_end( MPI_File fh, OPTIONAL_CONST void * buf, MPI_Status * status );

//@activity
//@activity_link_size fh
//@splice_before ''MPI_Offset offset; MPI_File_get_position(fh, & offset); MPI_File_get_byte_offset(fh, offset, & offset);''
//@activity_attribute filePosition offset
//@splice_before ''int intSize; MPI_Type_size(datatype, & intSize); uint64_t size = (uint64_t)intSize * (uint64_t)count;''
//@activity_attribute bytesToRead size
//@MPI_error
int MPI_File_read_all_begin( MPI_File fh, void * buf, int count, MPI_Datatype datatype );

//@activity
//@activity_link_size fh
//@MPI_error
int MPI_File_read_all_end( MPI_File fh, void * buf, MPI_Status * status );

//@activity
//@activity_link_size fh
//@splice_before ''MPI_Offset offset; MPI_File_get_position(fh, & offset); MPI_File_get_byte_offset(fh, offset, & offset);''
//@activity_attribute filePosition offset
//@splice_before ''int intSize; MPI_Type_size(datatype, & intSize); uint64_t size = (uint64_t)intSize * (uint64_t)count;''
//@activity_attribute bytesToWrite size
//@MPI_error
int MPI_File_write_all_begin( MPI_File fh, OPTIONAL_CONST void * buf, int count, MPI_Datatype datatype );

//@activity
//@activity_link_size fh
//@MPI_error
int MPI_File_write_all_end( MPI_File fh, OPTIONAL_CONST void * buf, MPI_Status * status );



//@activity
//@activity_link_size fh
//@splice_before ''MPI_Offset offset; MPI_File_get_position_shared(fh, & offset); MPI_File_get_byte_offset(fh, offset, & offset);''
//@activity_attribute filePosition offset
//@splice_before ''int intSize; MPI_Type_size(datatype, & intSize); uint64_t size = (uint64_t)intSize * (uint64_t)count;''
//@activity_attribute bytesToRead size
//@MPI_error
int MPI_File_read_ordered_begin( MPI_File fh, void * buf, int count, MPI_Datatype datatype );

//@activity
//@activity_link_size fh
//@MPI_error
int MPI_File_read_ordered_end( MPI_File fh, void * buf, MPI_Status * status );

//@activity
//@activity_link_size fh
//@splice_before ''MPI_Offset offset; MPI_File_get_position_shared(fh, & offset); MPI_File_get_byte_offset(fh, offset, & offset);''
//@activity_attribute filePosition offset
//@splice_before ''int intSize; MPI_Type_size(datatype, & intSize); uint64_t size = (uint64_t)intSize * (uint64_t)count;''
//@activity_attribute bytesToRead size
//@MPI_error
int MPI_File_write_ordered_begin( MPI_File fh, OPTIONAL_CONST void * buf, int count, MPI_Datatype datatype );

//@activity
//@activity_link_size fh
//@MPI_error
int MPI_File_write_ordered_end( MPI_File fh, OPTIONAL_CONST void * buf, MPI_Status * status );

//@activity
//@activity_link_size fh
//@MPI_error
int MPI_File_get_type_extent( MPI_File fh, MPI_Datatype datatype, MPI_Aint * extent );

//@activity
//@activity_link_size fh
//@MPI_error
int MPI_File_set_atomicity( MPI_File fh, int flag );

//@activity
//@activity_link_size fh
//@MPI_error
int MPI_File_get_atomicity( MPI_File fh, int * flag );

//@activity
//@activity_link_size fh
//@MPI_error
int MPI_File_sync( MPI_File fh );


////////////////////////////////////////////////////////////////////////////////////
/* OTHER MPI FUNCTIONS */
////////////////////////////////////////////////////////////////////////////////////



//@activity ActivityVariable=sioxActivity
//@splice_after if (ret == MPI_SUCCESS ) { recordProcessesGroup(sioxActivity, group); }
//@splice_after ''uint64_t commHandlerValue; commHandlerValue = getCommHandle(*newcomm);''
//@activity_attribute_late commHandlerOut commHandlerValue
//@splice_after ''commHandlerValue = getCommHandle(comm);''
//@activity_attribute_late commHandler commHandlerValue
int MPI_Comm_create( MPI_Comm comm, MPI_Group group,  MPI_Comm *newcomm );

//@activity ActivityVariable=sioxActivity
//@splice_after ''uint64_t commHandlerValue; commHandlerValue = getCommHandle(*newcomm);''
//@activity_attribute_late commHandlerOut commHandlerValue
//@splice_after ''commHandlerValue = getCommHandle(comm);''
//@activity_attribute_late commHandler commHandlerValue
int MPI_Comm_dup( MPI_Comm comm, MPI_Comm *newcomm );

//@activity ActivityVariable=sioxActivity
//@activity_attribute commColor color
//@activity_attribute commKey key
//@splice_after if (ret == MPI_SUCCESS ) { recordProcessesComm(sioxActivity, *newcomm); }
//@splice_after ''uint64_t commHandlerValue; commHandlerValue = getCommHandle(*newcomm);''
//@activity_attribute_late commHandlerOut commHandlerValue
//@splice_after ''commHandlerValue = getCommHandle(comm);''
//@activity_attribute_late commHandler commHandlerValue
int MPI_Comm_split( MPI_Comm comm, int color, int key, MPI_Comm *newcomm );



//@register_attribute t_id "MPI" "description/type/id" SIOX_STORAGE_64_BIT_UINTEGER

//@register_attribute t_size "MPI" "quantity/type/size" SIOX_STORAGE_32_BIT_INTEGER
//@register_attribute t_extent "MPI" "quantity/type/extent" SIOX_STORAGE_64_BIT_UINTEGER

#define RECORD_TYPE_INFORMATION(t) { int tmp_size; MPI_Type_size(t, & tmp_size); siox_activity_set_attribute( sioxActivity, t_size, & tmp_size ); MPI_Aint tmp_extent; uint64_t aint;  MPI_Type_extent(t, & tmp_extent); aint = tmp_extent;siox_activity_set_attribute( sioxActivity, t_extent, & aint );  }

//@register_attribute t_count "MPI" "quantity/type/struct/count" SIOX_STORAGE_32_BIT_INTEGER
//@register_attribute t_blocklen "MPI" "quantity/type/struct/blocklen" SIOX_STORAGE_32_BIT_INTEGER
//@register_attribute t_displ "MPI" "quantity/type/struct/displacement" SIOX_STORAGE_64_BIT_UINTEGER

//@activity ActivityVariable=sioxActivity
//@activity_attribute t_count count
//@splice_after '' uint64_t typeID = getConstructedDatatypeHandle(*newtype);'' 
//@activity_attribute_late t_id typeID
//@splice_before '' for (int t=0; t < count ; t++) { siox_activity_set_attribute( sioxActivity, t_blocklen , & array_of_blocklengths[t] ); uint64_t tmp;  tmp = array_of_displacements[t]; siox_activity_set_attribute( sioxActivity, t_displ, & tmp );  tmp = getDatatypeHandle(array_of_types[t]); siox_activity_set_attribute( sioxActivity, t_id , & tmp); } ''
//@splice_after RECORD_TYPE_INFORMATION(*newtype)
int MPI_Type_create_struct( int count, int array_of_blocklengths[], MPI_Aint array_of_displacements[], MPI_Datatype array_of_types[], MPI_Datatype *newtype );



#define MPI_COMMUNICATION

#ifdef MPI_COMMUNICATION

//@component "MPI" "Communication" ComponentVariable=comm

//@register_attribute cBytesSend "MPI" "quantity/bytesToSend" SIOX_STORAGE_64_BIT_UINTEGER
//@register_attribute cBytesReceived "MPI" "quantity/bytesReceived" SIOX_STORAGE_64_BIT_UINTEGER
//@register_attribute cBytesMaxReceived "MPI" "quantity/bytesMaxReceive" SIOX_STORAGE_64_BIT_UINTEGER
//@register_attribute cDest "MPI" "description/p2p/dest" SIOX_STORAGE_32_BIT_INTEGER
//@register_attribute cSource "MPI" "description/p2p/source" SIOX_STORAGE_32_BIT_INTEGER
//@register_attribute cTag "MPI" "description/p2p/tag" SIOX_STORAGE_32_BIT_INTEGER
//@register_attribute cCount "MPI" "quantity/p2p/count" SIOX_STORAGE_32_BIT_INTEGER
//@register_attribute CRoot "MPI" "description/collective/root" SIOX_STORAGE_32_BIT_INTEGER
//@register_attribute CCount "MPI" "quantity/collective/count" SIOX_STORAGE_32_BIT_INTEGER

//@register_attribute CDataSize "MPI" "quantity/collective/dataSize" SIOX_STORAGE_64_BIT_UINTEGER

//@activity ComponentVariable=comm
//@MPI_error
//@splice_after ''uint64_t commHandlerValue; commHandlerValue = getCommHandle(comm);''
//@activity_attribute_late commHandler commHandlerValue
//@activity_attribute cCount count
//@activity_attribute cDest dest
//@activity_attribute cTag tag
//@splice_before ''int intSize; MPI_Type_size(datatype, & intSize); uint64_t size = (uint64_t)intSize * (uint64_t)count;''
//@activity_attribute cBytesSend size
int MPI_Send(void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);

//@activity ComponentVariable=comm
//@MPI_error
//@splice_after ''uint64_t commHandlerValue; commHandlerValue = getCommHandle(comm);''
//@activity_attribute_late commHandler commHandlerValue
//@activity_attribute cCount count
//@activity_attribute cSource source
//@activity_attribute cTag tag

//@splice_before ''int intSize; MPI_Type_size(datatype, & intSize); uint64_t size = (uint64_t)intSize * (uint64_t)count;''
//@activity_attribute cBytesMaxReceived size

//@splice_before ''MPI_Status sStatus; if (status == MPI_STATUS_IGNORE) {status = & sStatus; }''
//@splice_after '' int rCount; MPI_Get_count(status, datatype, & rCount); size = (uint64_t) intSize * (uint64_t) rCount;''
//@activity_attribute_late cBytesReceived size
int MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status);


//@activity ComponentVariable=comm
//@MPI_error
//@splice_after ''uint64_t commHandlerValue; commHandlerValue = getCommHandle(comm);''
//@activity_attribute_late commHandler commHandlerValue
int MPI_Barrier( MPI_Comm comm );


//@activity ComponentVariable=comm
//@MPI_error
//@splice_after ''uint64_t commHandlerValue; commHandlerValue = getCommHandle(comm);''
//@activity_attribute_late commHandler commHandlerValue
//@activity_attribute CRoot root
//@activity_attribute CCount count

//@splice_before '' uint64_t typeID = getDatatypeHandle(datatype);'' 
//@activity_attribute t_id typeID

//@splice_before ''int intSize; MPI_Type_size(datatype, & intSize); uint64_t size = (uint64_t)intSize * (uint64_t)count;''
//@activity_attribute CDataSize size
int MPI_Bcast( void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm );

#endif