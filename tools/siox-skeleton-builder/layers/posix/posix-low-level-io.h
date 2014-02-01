/**
 * See: http://www.jedsoft.org/slang/doc/html/slangfun-15.html
 * Kurzbeschreibungen hier hineinkopieren!
 */

#include <unistd.h>
#include <sys/uio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>

#include <limits.h> // for canonicalize_file_name and realpath

#define HAVE_AIO

#ifdef HAVE_AIO
#include <aio.h>
#endif

// TODO: To ensure thread safety, calls which query the current position should be protected? We may also accept inaccuracy when multiple threads concurrently access a file...
// TODO: Generall aproach to handle errors: How do we convert them to the platform dependent error message.

/* Set the interface name for the library*/
//@component "POSIX" ""

/* Register the data types for the descriptors */
//@register_attribute bytesToRead "POSIX" "quantity/BytesToRead" SIOX_STORAGE_64_BIT_UINTEGER
//@register_attribute bytesToWrite "POSIX" "quantity/BytesToWrite" SIOX_STORAGE_64_BIT_UINTEGER
//@register_attribute filePosition "POSIX" "file/position" SIOX_STORAGE_64_BIT_UINTEGER
//@register_attribute fileExtent "POSIX" "file/extent" SIOX_STORAGE_64_BIT_UINTEGER


//@register_attribute fileMemoryRegions "POSIX" "quantity/memoryRegions" SIOX_STORAGE_32_BIT_INTEGER

//@register_attribute fileOpenFlags "POSIX" "hints/openFlags" SIOX_STORAGE_32_BIT_UINTEGER

//@register_attribute fileName "POSIX" "descriptor/filename" SIOX_STORAGE_STRING
//@register_attribute fileSystem "Global" "descriptor/filesystem" SIOX_STORAGE_32_BIT_UINTEGER
//@register_attribute fileHandle "POSIX" "descriptor/filehandle" SIOX_STORAGE_32_BIT_UINTEGER
//@register_attribute bytesWritten "POSIX" "quantity/BytesWritten" SIOX_STORAGE_64_BIT_UINTEGER
//@register_attribute bytesRead "POSIX" "quantity/BytesRead" SIOX_STORAGE_64_BIT_UINTEGER

//@register_attribute fileAdviseExtent "POSIX" "hint/advise-extent" SIOX_STORAGE_64_BIT_UINTEGER
//@register_attribute fileAdvise "POSIX" "hints/advise" SIOX_STORAGE_32_BIT_INTEGER
//@register_attribute fileBufferSize "POSIX" "hints/bufferSize" SIOX_STORAGE_64_BIT_INTEGER
//@register_attribute fileBufferMode "POSIX" "hints/bufferMode" SIOX_STORAGE_32_BIT_INTEGER


/* Prepare a (hash) map to link descriptors (in this case, of type int) to their activities.
   This is necessary for the horizontal linking of activities.
   The map and all other functions using it default to the same built-in name,
   so it can usually be omitted. */
//@horizontal_map_create_int
//@horizontal_map_create_size


//TODO we have to determine the filesystem based on the mountpoint, this is a SIOX helper function.
//@splice_once ''#define SET_FILENAME(ARG) char fileNamebuffer[PATH_MAX]; char * resPath = realpath(ARG, fileNamebuffer); if ( resPath == NULL ) fileNamebuffer[0] = 0;  siox_activity_set_attribute( sioxActivity, fileName, fileNamebuffer );''

//@include "posix-low-level-io-helper.h"

/*------------------------------------------------------------------------------
End of global part
------------------------------------------------------------------------------*/

//@splice_before mode_t mode = va_arg(valist,mode_t);
//@guard
//@errorErrno ''ret<0''
//@activity
//@splice_before uint32_t translatedFlags = translatePOSIXFlagsToSIOX(flags);
//@activity_attribute fileOpenFlags translatedFlags
//@splice_before SET_FILENAME(pathname)
//@activity_attribute_late fileHandle ret
//@horizontal_map_put_int ret
//@rewriteCall open ''pathname,flags,mode'' ''const char *pathname, int flags, mode_t mode''
int open( const char * pathname, int flags, ... );

//@guard
//@errorErrno ''ret<0''
//@activity
//@horizontal_map_put_int ret
//@splice_before SET_FILENAME(pathname)
//@activity_attribute_late fileHandle ret
int creat( const char * pathname, mode_t mode );

//@splice_before mode_t mode = va_arg(valist,mode_t);
//@guard
//@errorErrno ''ret<0''
//@activity open
//@splice_before SET_FILENAME(pathname)
//@splice_before uint32_t translatedFlags = translatePOSIXFlagsToSIOX(flags);
//@activity_attribute fileOpenFlags translatedFlags
//@horizontal_map_put_int ret
//@activity_attribute_late fileHandle ret
//@rewriteCall open ''pathname,flags,mode'' ''const char *pathname, int flags, mode_t mode''
int open64( const char * pathname, int flags, ... );

//@guard
//@errorErrno ''ret<0''
//@activity creat
//@horizontal_map_put_int ret
//@splice_before SET_FILENAME(pathname)
//@activity_attribute_late fileHandle ret
int creat64( const char * pathname, mode_t mode );
//@guard
//@errorErrno ''ret<0''
//@activity
//@activity_link_int fd
//@horizontal_map_remove_int fd
//@activity_attribute fileHandle fd
int close( int fd );

//@guard
//@errorErrno ''ret<0''
//@activity
//@activity_attribute fileHandle fd
//@activity_lookup_ID_int fd ActivityID=ParentID
//@horizontal_map_put_int_ID ret ActivityID=ParentID
int dup( int fd );

//This code is actually not completely correct, dup2 may close newfd:
//@guard
//@errorErrno ''ret<0''
//@activity
//@activity_attribute fileHandle oldfd
//@activity_lookup_ID_int oldfd ActivityID=ParentID
//@horizontal_map_put_int_ID newfd ActivityID=ParentID
int dup2( int oldfd, int newfd );


//This code is actually not completely correct, dup3 may close newfd:
//@guard
//@errorErrno ''ret<0''
//@activity
//@activity_attribute fileHandle oldfd
//@activity_lookup_ID_int oldfd ActivityID=ParentID
//@horizontal_map_put_int_ID newfd ActivityID=ParentID
int dup3( int oldfd, int newfd, int flags );


#include <sys/sendfile.h>
//@guard
//@errorErrno ''ret<0''
//@activity
//@activity_attribute bytesToWrite count
//@activity_attribute fileHandle out_fd
//@activity_attribute fileHandle in_fd
//@activity_attribute_late bytesWritten ret
//@activity_link_int out_fd
//@activity_link_int in_fd
ssize_t sendfile(int out_fd, int in_fd, off_t *offset, size_t count);


//@guard
//@errorErrno ''ret<0''
//@activity
//@activity_attribute bytesToWrite count
//@activity_attribute fileHandle fd
//@activity_attribute_late bytesWritten ret
//@activity_link_int fd
ssize_t write( int fd, const void * buf, size_t count );

//@guard
//@errorErrno ''ret<0''
//@activity
//@activity_attribute_late bytesRead ret
//@activity_attribute fileHandle fd
//@activity_link_int fd
ssize_t read( int fd, void * buf, size_t count );

//@guard
//@errorErrno ''ret<0''
//@activity
//@activity_attribute fileHandle fd
//@activity_attribute fileMemoryRegions iovcnt
//@activity_attribute_late bytesWritten ret
//@activity_link_int fd
ssize_t writev( int fd, const struct iovec * iov, int iovcnt );

//@guard
//@errorErrno ''ret<0''
//@activity
//@activity_attribute_late bytesRead ret
//@activity_attribute fileMemoryRegions iovcnt
//@activity_attribute fileHandle fd
//@activity_link_int fd
ssize_t readv( int fd, const struct iovec * iov, int iovcnt );

//@guard
//@errorErrno ''ret==(size_t)-1''
//@activity
//@activity_attribute bytesToWrite count
//@activity_attribute fileHandle fd
//@activity_attribute filePosition offset
//@activity_attribute_late bytesWritten ret
//@activity_link_int fd
ssize_t pwrite( int fd, const void * buf, size_t count, off_t offset );

//@guard
//@errorErrno ''ret==(size_t)-1''
//@activity
//@activity_attribute bytesToRead count
//@activity_attribute_late bytesRead ret
//@activity_attribute fileHandle fd
//@activity_attribute filePosition offset
//@activity_link_int fd
ssize_t pread( int fd, void * buf, size_t count, off_t offset );

//@guard
//@errorErrno ''ret==(size_t)-1''
//@activity
//@activity_attribute bytesToWrite count
//@activity_attribute fileHandle fd
//@activity_attribute filePosition offset
//@activity_attribute_late bytesWritten ret
//@activity_link_int fd
ssize_t pwrite64( int fd, const void * buf, size_t count, off_t offset );

//@guard
//@errorErrno ''ret==(size_t)-1''
//@activity
//@activity_attribute bytesToRead count
//@activity_attribute_late bytesRead ret
//@activity_attribute fileHandle fd
//@activity_attribute filePosition offset
//@activity_link_int fd
ssize_t pread64( int fd, void * buf, size_t count, off_t offset );

//@guard
//@errorErrno ''ret==(size_t)-1''
//@activity
//@activity_attribute fileMemoryRegions iovcnt
//@activity_attribute fileHandle fd
//@activity_attribute filePosition offset
//@activity_attribute_late bytesWritten ret
//@activity_link_int fd
ssize_t pwritev( int fd, const struct iovec * iov, int iovcnt, off_t offset );

//@guard
//@errorErrno ''ret==(size_t)-1''
//@activity
//@activity_attribute_late bytesRead ret
//@activity_attribute fileMemoryRegions iovcnt
//@activity_attribute fileHandle fd
//@activity_attribute filePosition offset
//@activity_link_int fd
ssize_t preadv( int fd, const struct iovec * iov, int iovcnt, off_t offset );

//@guard
//@errorErrno ''ret==(size_t)-1''
//@activity
//@activity_attribute fileMemoryRegions iovcnt
//@activity_attribute fileHandle fd
//@activity_attribute filePosition offset
//@activity_attribute_late bytesWritten ret
//@activity_link_int fd
ssize_t pwritev64( int fd, const struct iovec * iov, int iovcnt, off_t offset );

//@guard
//@errorErrno ''ret==(size_t)-1''
//@activity
//@activity_attribute_late bytesRead ret
//@activity_attribute fileMemoryRegions iovcnt
//@activity_attribute fileHandle fd
//@activity_attribute filePosition offset
//@activity_link_int fd
ssize_t preadv64( int fd, const struct iovec * iov, int iovcnt, off_t offset );


//@guard
//@activity
void sync( void );

//@guard
//@errorErrno ''ret<0''
//@activity
//@activity_attribute fileHandle fd
//@activity_link_int fd
int fsync( int fd );

//@guard
//@errorErrno ''ret<0''
//@activity
//@activity_attribute fileHandle fd
//@activity_link_int fd
int fdatasync( int fd );

//@guard
//@errorErrno ''ret == (off_t) -1''
//@activity
//@activity_attribute fileHandle fd
//@activity_link_int fd
//@activity_attribute filePosition ret
off_t lseek(int fd, off_t offset, int whence);



/* Hints to the system, this might be used for optimizations by SIOX in the future */
/* See also: http://insights.oetiker.ch/linux/fadvise.html */

//@guard
//@errorErrno ''ret<0''
//@activity
//@activity_attribute fileHandle fd
//@activity_attribute filePosition offset
//@activity_attribute fileAdviseExtent len
//@activity_attribute fileAdvise advise
//@activity_link_int fd
int posix_fadvise( int fd, off_t offset, off_t len, int advise );



//  Deletes the file whose name is specified in filename.
/*
 If the file is successfully deleted, a zero value is returned.
On failure, a nonzero value is returned.
On most library implementations, the variable is also set to a system-specific error code on failure.
 */
//@guard
//@errorErrno ''ret<0''
//@activity
//@splice_before SET_FILENAME(filename)
int remove( const char * filename );

/*
 If the file is successfully renamed, a zero value is returned.
On failure, a nonzero value is returned.
On most library implementations, the variable is also set to a system-specific error code on failure.
 */
//@guard
//@errorErrno ''ret<0''
//@activity
//@splice_before SET_FILENAME(oldname)
int rename( const char * oldname, const char * newname );

// Linux specific, Xstat is redirected to Xstat64
// stat() symbols do not exist, a macro rewrites them, problem stat64 types.
// Be aware this might lead to problems.
//@splice_once ''int stat(const char *path, struct stat *buf){ return __xstat64(1, path, buf); }''
//@guard
//@errorErrno ''ret<0''
//@activity Name=stat
//@splice_before SET_FILENAME(path)
int __xstat64( int __ver, const char * path, struct stat64 * buf );

//@splice_once ''int lstat(const char *path, struct stat *buf){ return __lxstat64(1, path, buf); }''
//@guard
//@errorErrno ''ret<0''
//@activity Name=lstat
//@splice_before SET_FILENAME(path)
int __lxstat64( int __ver, const char * path, struct stat64 * buf );

//@splice_once ''int fstat(int fd, struct stat *buf){ return __fxstat64(1, fd, buf); }''
//@guard
//@errorErrno ''ret<0''
//@activity Name=fstat
//@activity_attribute fileHandle fd
//@activity_link_int fd
int __fxstat64( int __ver, int fd, struct stat64 * buf );

//guard
//error ''ret<0''
//activity Name=stat
//splice_before SET_FILENAME(path)
//guardEnd
//int __xstat( int __ver, const char * path, struct stat * buf );

//guard
//error ''ret<0''
//activity Name=lstat
//splice_before SET_FILENAME(path)
//guardEnd
//int __lxstat( int __ver, const char * path, struct stat * buf );

//@guard
//@errorErrno ''ret<0''
//@activity Name=fstat
//@activity_attribute fileHandle fd
//@activity_link_int fd
int __fxstat( int __ver, int fd, struct stat * buf );


/* See: http://www.gnu.org/software/libc/manual/html_mono/libc.html#Memory_002dmapped-I_002fO */
/* Probably we should record this type of usage with SIOX, but we cannot track the I/O on user-space */


//@guard
//@errorErrno ''ret<0''
//@activity
//@activity_attribute fileHandle fd
//@activity_attribute fileExtent length
//@activity_attribute filePosition offset
//@activity_link_int fd
void * mmap( void * address, size_t length, int protect, int flags, int fd, off_t offset );

//@guard
//@errorErrno ''ret<0''
//@activity
//@activity_attribute fileHandle fd
//@activity_attribute fileExtent length
//@activity_attribute filePosition offset
//@activity_link_int fd
void * mmap64( void * address, size_t length, int protect, int flags, int fd, off_t offset );

//void * mremap (void *address, size_t length, size_t new_length, int flag);
//int munmap (void *addr, size_t length);
//int madvise (void *addr, size_t length, int advice);

/*
Opens the file whose name is specified in the parameter filename and associates it with a stream that can be identified in future operations by the FILE pointer returned.

If the file is successfully opened, the function returns a pointer to a FILE object that can be used to identify the stream on future operations.
Otherwise, a null pointer is returned.
On most library implementations, the variable is also set to a system-specific error code on failure.
 */
//@guard
//@errorErrno ''ret<0''
//@activity
//@splice_before SET_FILENAME(filename)
//@splice_before uint32_t translatedFlags = translateFILEFlagsToSIOX(mode);
//@activity_attribute fileOpenFlags translatedFlags
//@splice_after int fd = (ret != 0) ? fileno(ret) : 0;
//@activity_attribute_late fileHandle fd
//@horizontal_map_put_size ret
FILE * fopen( const char * filename, const char * mode );


/*
Opens the file whose name is specified in the parameter filename and associates it with a stream that can be identified in future operations by the FILE pointer returned.

If the file is successfully opened, the function returns a pointer to a FILE object that can be used to identify the stream on future operations.
Otherwise, a null pointer is returned.
On most library implementations, thesave variable is also set to a system-specific ercode on failure.
 */
//@guard
//@errorErrno ''ret<0''
//@activity
//@splice_before SET_FILENAME(filename)
//@splice_before uint32_t translatedFlags = translateFILEFlagsToSIOX(mode);
//@activity_attribute fileOpenFlags translatedFlags
//@horizontal_map_put_size ret
//@splice_after int fd = (ret != 0) ? fileno(ret) : 0;
//@activity_attribute_late fileHandle fd
FILE * fopen64( const char * filename, const char * mode );
/*
The  fdopen()  function  associates a stream with the existing file descriptor, fd.  The mode of the stream (one of the
values "r", "r+", "w", "w+", "a", "a+") must be compatible with the mode of the file  descriptor.   The  file  position
indicator  of  the  new  stream  is  set to that belonging to fd, and the error and end-of-file indicators are cleared.
Modes "w" or "w+" do not cause truncation of the file.  The file descriptor is not dup'ed, and will be closed when  the
stream created by fdopen() is closed.
 */
//@guard
//@errorErrno ''ret<0''
//@activity
//@activity_link_int fd
//@splice_before uint32_t translatedFlags = translateFILEFlagsToSIOX(mode);
//@activity_attribute fileOpenFlags translatedFlags
//@horizontal_map_put_size ret
FILE * fdopen( int fd, const char * mode );

// The function fileno() examines the argument stream and returns its integer descriptor.
// Function does not fail, only if stream is invalid.

//@guard
//@errorErrno ''ret<0''
//@activity
//@activity_link_size stream
//@horizontal_map_put_int ret
//@activity_attribute_late fileHandle ret
int fileno( FILE * stream );

/*
Reuses stream to either open the file specified by filename or to change its access mode.
If a new filename is specified, the /function first attempts to close any file already associated with stream (third parameter) and disassociates it. Then, independently of whether that stream was successfuly closed or not, freopen opens the file specified by filename and associates it with the stream just as fopen would do using the specified mode.

If filename is a null pointer, the function attempts to change the mode of the stream. Although a particular library implementation is allowed to restrict the changes permitted, and under which circumstances.

If the file is successfully reopened, the function returns the pointer passed as parameter stream, which can be used to identify the reopened stream.
Otherwise, a null pointer is returned.
On most library implementations, the variable is also set to a system-specific error code on failure.
 */
//@guard
//@errorErrno ''ret<0''
//@activity
//@splice_before SET_FILENAME(filename)
//@splice_before uint32_t translatedFlags = translateFILEFlagsToSIOX(mode);
//@activity_attribute fileOpenFlags translatedFlags
//@horizontal_map_remove_size stream
//@horizontal_map_put_size ret
//@activity_link_size stream
//@splice_after int fd = (ret != 0) ? fileno(ret) : 0;
//@activity_attribute_late fileHandle fd
FILE * freopen( const char * filename, const char * mode, FILE * stream );

//@guard
//@errorErrno ''ret<0''
//@activity
//@horizontal_map_put_size ret
//@splice_after int fd = (ret != 0) ? fileno(ret) : 0;
//@activity_attribute_late fileHandle fd
FILE * tmpfile( void );

//@guard
//@errorErrno ''ret<0''
//@activity
//@activity_link_size stream
//@horizontal_map_remove_size stream
int fclose( FILE * stream );

//  If an error occurs, EOF is returned and the error indicator is set (see ferror).
//@guard
//@errorErrno ''ret<0''
//@activity
//@activity_link_size stream
int fflush( FILE * stream );

//On success, the character read is returned (promoted to an int value).
//The return type is int to accommodate for the special value EOF, which indicates failure:

//@guard
//@errorErrno ''ret == EOF''
//@activity
//@activity_link_size stream
int fgetc( FILE * stream );

/*
On success, the character read is returned (promoted to an int value).
The return type is int to accommodate for the special value EOF, which indicates failure:
If the position indicator was at the end-of-file, the function returns EOF and sets the eof indicator (feof) of stream.
If some other reading error happens, the function also returns EOF, but sets its error indicator (ferror) instead.
 */
//@guard
//@errorErrno ''ret == EOF''
//@activity
//@activity_link_size stream
int getc( FILE * stream );


/*
 Writes a character to the stream and advances the position indicator.
 On success, the character written is returned.
 If a writing error occurs, EOF is returned and the error indicator (ferror) is set.
 */
//@guard
//@errorErrno ''ret == EOF''
//@activity
//@activity_link_size stream
int fputc( int character, FILE * stream );

//@guard
//@errorErrno ''ret == EOF''
//@activity
//@activity_link_size stream
int putc( int character, FILE * stream );

/*
Reads characters from stream and stores them as a C string into str until (num-1) characters have been read or either a newline or the end-of-file is reached, whichever happens first.

On success, the function returns str.
If the end-of-file is encountered while attempting to read a character, the eof indicator is set (feof). If this happens before any characters could be read, the pointer returned is a null pointer (and the contents of str remain unchanged).
If a read error occurs, the error indicator (ferror) is set and a null pointer is also returned (but the contents pointed by str may have changed).
*/

//@guard
//@errorErrno ''ret == NULL''
//@activity
//@activity_link_size stream
char * fgets( char * str, int num, FILE * stream );


/*
On success, a non-negative value is returned.
On error, the function returns EOF and sets the error indicator (ferror).
 */
//@guard
//@errorErrno ''ret == EOF''
//@activity
//@activity_link_size stream
int fputs( const char * str, FILE * stream );



/*
The total number of elements successfully read is returned.
If this number differs from the count parameter, either a reading error occurred or the end-of-file was reached while reading. In both cases, the proper indicator is set, which can be checked with ferror and feof, respectively.
If either size or count is zero, the function returns zero and both the stream state and the content pointed by ptr remain unchanged.
 */
//@guard
//@errorErrno ''ret != count''
//@activity
//@activity_link_size stream
//@splice_after ''uint64_t posDelta = ret*size;''
//@activity_attribute_late bytesRead posDelta
size_t fread( void * ptr, size_t size, size_t count, FILE * stream );

/*
Writes an array of count elements, each one with a size of size bytes, from the block of memory pointed by ptr to the current position in the stream.

The total number of elements successfully written is returned.
If this number differs from the count parameter, a writing error prevented the function from completing. In this case, the error indicator (ferror) will be set for the stream.
If either size or count is zero, the function returns zero and the error indicator remains unchanged.
 */
//@guard
//@errorErrno ''ret != count''
//@activity
//@activity_link_size stream
//@splice_after ''uint64_t posDelta = ret*size;''
//@activity_attribute_late bytesWritten posDelta
size_t fwrite( const void * ptr, size_t size, size_t count, FILE * stream );


//@guard
//@errorErrno ''ret != 0''
//@activity
//@activity_link_size stream
//@splice_after ''uint64_t pos = (uint64_t) ftell(stream);''
//@activity_attribute_late filePosition pos
int fseeko(FILE *stream, off_t offset, int whence);



/*
Specifies the buffer to be used by the stream for I/O operations, which becomes a fully buffered stream. Or, alternatively, if buffer is a null pointer, buffering is disabled for the stream, which becomes an unbuffered stream.

This function should be called once the stream has been associated with an open file, but before any input or output operation is performed with it.
 */
//@guard
//@activity
//@activity_link_size stream
void setbuf( FILE * stream, char * buffer );

/*

If the buffer is correctly assigned to the file, a zero value is returned.
Otherwise, a non-zero value is returned; This may be due to an invalid mode parameter or to some other error allocating or assigning the buffer.
 */

//@guard
//@activity
//@activity_link_size stream
//@activity_attribute fileBufferSize size
//@activity_attribute fileBufferMode mode
int setvbuf( FILE * stream, char * buffer, int mode, size_t size );

/*
On success, the total number of characters written is returned.
If a writing error occurs, the error indicator (ferror) is set and a negative number is returned.
If a multibyte character encoding error occurs while writing wide characters, is set to EILSEQ and a negative number is returned.
 */
//@guard
//@errorErrno ''ret < 0''
//@activity
//@activity_link_size stream
//@activity_attribute_late bytesWritten ret
int vfprintf( FILE * stream, const char * format, va_list arg );

/*
On success, the function returns the number of items of the argument list successfully filled. This count can match the expected number of items or be less (even zero) due to a matching failure, a reading error, or the reach of the end-of-file.
If a reading error happens or the end-of-file is reached while reading, the proper indicator is set (feof or ferror). And, if either happens before any data could be successfully read, EOF is returned.
If an encoding error happens interpreting wide characters, the function sets to EILSEQ.
 */
//@guard
//@errorErrno ''ret < 0''
//@activity
//@activity_link_size stream
int vfscanf( FILE * stream, const char * format, va_list arg );


/*
On success, the function returns the number of items of the argument list successfully filled. This count can match the expected number of items or be less (even zero) due to a matching failure, a reading error, or the reach of the end-of-file.
If a reading error happens or the end-of-file is reached while reading, the proper indicator is set (feof or ferror). And, if either happens before any data could be successfully read, EOF is returned.
If an encoding error happens interpreting wide characters, the function sets to EILSEQ.
 */

//@guard
//@errorErrno ''ret < 0''
//@activity
//@activity_link_size stream
//@rewriteCall vfscanf ''stream,format,valist'' 
int fscanf( FILE * stream, const char * format, ... );

/*
 */
//@guard
//@errorErrno ''ret < 0''
//@activity
//@activity_link_size stream
//@rewriteCall vfprintf ''stream,format,valist'' 
int fprintf( FILE * stream, const char * format, ... );


// Redirection of stdout to a file? No, we do not handle this at the moment.
#ifdef HAVE_AIO
/* Asynchronous I/O */
/* see http://www.ibm.com/developerworks/linux/library/l-async/index.html */
/*
aio_read    Request an asynchronous read operation
aio_error   Check the status of an asynchronous request
aio_return  Get the return status of a completed asynchronous request
aio_write   Request an asynchronous operation
aio_suspend Suspend the calling process until one or more asynchronous requests have completed (or failed)
aio_cancel  Cancel an asynchronous I/O request
lio_listio  Initiate a list of I/O operations
 */


//@guard
//@errorErrno ''ret < 0''
//@activity
int aio_read( struct aiocb * cb );

//@guard
//@errorErrno ''ret < 0''
//@activity
int aio_write( struct aiocb * cb );

//@guard
//@errorErrno ''ret < 0''
//@activity
int lio_listio( int mode, struct aiocb * const aiocb_list[], int nitems, struct sigevent * sevp );

//@guard
//@errorErrno ''ret < 0''
//@activity
int aio_suspend( const struct aiocb * const aiocb_list[], int nitems, const struct timespec * timeout );

//@guard
//@errorErrno ''ret < 0''
//@activity
int aio_cancel( int fd, struct aiocb * aiocbp );

#endif


#include <sched.h>

//http://stackoverflow.com/questions/14407544/mixing-threads-fork-and-mutexes-what-should-i-watch-out-for

//splice_before ''printf("Warning clone() called, statistics are re-initialized !\n");''
//splice_before siox_finalize_monitoring();
//splice_after siox_initialize_monitoring();
//int clone( int ( *fn )( void * ), void * child_stack, int flags, void * arg, pid_t * ptid, struct user_desc * tls, pid_t * ctid );

//@splice_before ''printf("Warning fork() called, statistics are re-initialized !\n");''
//@splice_before siox_finalize_monitoring();
//@splice_after siox_initialize_monitoring();
pid_t fork( void );


// An FD might be involved in inter-process communication and not only in file I/O.
// Actually, do we really want to monitor socket I/O with SIOX ? 
// We want to distinguish file I/O from socket I/O, though!

// FILE LOCKING STUFF

int lockf(int fd, int cmd, off_t len);

#include <sys/file.h>
int flock(int fd, int operation);

// Special commands

// int fcntl(int fd, int cmd, ...);

// #include <sys/ioctl.h>
// int ioctl(int d, int request, ...);


#define INSTRUMENT_POSIX_COMMUNICATION

#ifdef INSTRUMENT_POSIX_COMMUNICATION

#include <sys/types.h>
#include <sys/socket.h>
//@guard
//@errorErrno ''ret < 0''
//@activity
//@activity_attribute_late fileHandle ret
//@horizontal_map_put_int ret
int socket(int domain, int type, int protocol);

//@guard
//@errorErrno ''ret < 0''
//@activity
//@horizontal_map_put_int pipefd[0]
//@horizontal_map_put_int pipefd[1]
//@activity_attribute_late fileHandle pipefd[0]
//@activity_attribute_late fileHandle pipefd[1]
int pipe(int pipefd[2]);

//@guard
//@errorErrno ''ret < 0''
//@activity
//@horizontal_map_put_int pipefd[0]
//@horizontal_map_put_int pipefd[1]
//@activity_attribute_late fileHandle pipefd[0]
//@activity_attribute_late fileHandle pipefd[1]
int pipe2(int pipefd[2], int flags);

//@guard
//@errorErrno ''ret < 0''
//@activity
//@horizontal_map_put_int sv[0]
//@horizontal_map_put_int sv[1]
//@activity_attribute_late fileHandle sv[0]
//@activity_attribute_late fileHandle sv[1]
int socketpair(int domain, int type, int protocol, int sv[2]);

//@guard
//@errorErrno ''ret < 0''
//@activity
//@horizontal_map_put_int ret
//@activity_attribute_late fileHandle sockfd
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

//@guard
//@errorErrno ''ret < 0''
//@activity
//@horizontal_map_put_int ret
//@activity_attribute_late fileHandle sockfd
int accept4(int sockfd, struct sockaddr *addr, socklen_t *addrlen, int flags);


//ssize_t send(int sockfd, const void *buf, size_t len, int flags);
//ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);
//ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags);

#endif
