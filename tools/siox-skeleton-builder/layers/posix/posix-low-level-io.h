/**
 * See: http://www.jedsoft.org/slang/doc/html/slangfun-15.html
 * Kurzbeschreibungen hier hineinkopieren!
 */

#define _GNU_SOURCE 1 

#include <unistd.h>
#include <sys/uio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dlfcn.h>
//#include <fcntl.h>
#include <stdio.h>
#include <errno.h>

// TODO: To ensure thread safety, calls which query the current position should be protected using a mutex.
// TODO: Generall aproach to handle errors: How do we convert them to the platform dependent error message.

/* Set the interface name for the library*/
//@component "POSIX" ""

/* Register the data types for the descriptors */
//@register_attribute bytesToRead "POSIX" "quantity/BytesToRead" SIOX_STORAGE_64_BIT_UINTEGER
//@register_attribute bytesToWrite "POSIX" "quantity/BytesToWrite" SIOX_STORAGE_64_BIT_UINTEGER
//@register_attribute filePosition "POSIX" "file/position" SIOX_STORAGE_64_BIT_UINTEGER

//@register_attribute fileMemoryRegions "POSIX" "quantity/memoryRegions" SIOX_STORAGE_32_BIT_INTEGER

//@register_attribute fileName "POSIX" "filename" SIOX_STORAGE_STRING
//@register_attribute fileSystem "Global" "filesystem" SIOX_STORAGE_32_BIT_UINTEGER
//@register_attribute fileHandle "POSIX" "filehandle" SIOX_STORAGE_32_BIT_UINTEGER
//@register_attribute bytesWritten "POSIX" "quantity/BytesWritten" SIOX_STORAGE_64_BIT_UINTEGER
//@register_attribute bytesRead "POSIX" "quantity/BytesRead" SIOX_STORAGE_64_BIT_UINTEGER

 //@register_attribute fileAdviseExtent "POSIX" "hint/advise-extent" SIOX_STORAGE_64_BIT_UINTEGER
 //@register_attribute fileAdvise "POSIX" "hints/advise" SIOX_STORAGE_32_BIT_INTEGER

/* Prepare a (hash) map to link descriptors (in this case, of type int) to their activities.
   This is necessary for the horizontal linking of activities.
   The map and all other functions using it default to the same built-in name,
   so it can usually be omitted. */
//@horizontal_map_create_int

/*------------------------------------------------------------------------------
End of global part
------------------------------------------------------------------------------*/

//@guard
//@activity
//@activity_attribute_pointer fileName pathname
//@activity_attribute_u32 fileHandle ret 
//@horizontal_map_put_int ret
//@error ''ret<0'' errno
//@guardEnd
int open(const char *pathname, int flags, mode_t mode);

//@guard
//@activity
//@horizontal_map_put_int ret
//@activity_attribute_pointer fileName pathname
//@activity_attribute_u32 fileHandle ret 
//@error ''ret<0'' errno
//@guardEnd
int creat(const char *pathname, mode_t mode);

//@guard
//@activity
//@horizontal_map_remove_int fd
//@activity_attribute_u32 fileHandle fd
/*@error 'ret < 0' ret*/
//@guardEnd
int close(int fd);

//@guard
//@activity
//@activity_attribute_u32 fileHandle fd 
//@activity_lookup_int fd Activity=Parent
//@horizontal_map_put_int ret Activity=Parent
//@error ''ret<0'' errno
//@guardEnd
int dup(int fd);

//This code is actually not completely correct, dup2 may close newfd:
//@guard
//@activity
//@activity_attribute_u32 fileHandle oldfd 
//@activity_lookup_int oldfd Activity=Parent
//@horizontal_map_put_int newfd Activity=Parent
//@error ''ret<0'' errno
//@guardEnd
int dup2(int oldfd, int newfd); 


//This code is actually not completely correct, dup3 may close newfd:
//@guard
//@activity
//@activity_attribute_u32 fileHandle oldfd 
//@activity_lookup_int oldfd Activity=Parent
//@horizontal_map_put_int newfd Activity=Parent
//@error ''ret<0'' errno
//@guardEnd
int dup3(int oldfd, int newfd, int flags); 

//@guard
//@activity
//@activity_attribute bytesToWrite count
//@activity_attribute bytesWritten ret
//@activity_attribute_u32 fileHandle fd
//@splice_before ''uint64_t offset = lseek(fd,0,SEEK_CUR);''
//@activity_attribute filePosition offset
//@activity_link_int fd
//@error ''ret<0'' errno
//@guardEnd
ssize_t write(int fd, const void *buf, size_t count);

//@guard
//@activity
//@activity_attribute bytesRead ret
//@activity_attribute_u32 fileHandle fd
//@splice_before ''uint64_t offset = lseek(fd,0,SEEK_CUR);''
//@activity_attribute filePosition offset
//@activity_link_int fd
//@error ''ret<0'' errno
//@guardEnd
ssize_t read(int fd, void *buf, size_t count);

//@guard
//@activity
//@activity_attribute bytesWritten ret
//@activity_attribute_u32 fileHandle fd
//@activity_attribute_u32 fileMemoryRegions iovcnt
//@splice_before ''uint64_t offset = lseek(fd,0,SEEK_CUR);''
//@activity_attribute filePosition offset
//@activity_link_int fd
//@error ''ret<0'' errno
//@guardEnd
ssize_t writev(int fd, const struct iovec *iov, int iovcnt);

//@guard
//@activity
//@activity_attribute bytesRead ret
//@activity_attribute_u32 fileMemoryRegions iovcnt
//@activity_attribute_u32 fileHandle fd
//@splice_before ''uint64_t offset = lseek(fd,0,SEEK_CUR);''
//@activity_attribute filePosition offset
//@activity_link_int fd
//@error ''ret<0'' errno
//@guardEnd
ssize_t readv(int fd, const struct iovec *iov, int iovcnt);


//@guard
//@activity
//@activity_attribute bytesToWrite count
//@activity_attribute bytesWritten ret
//@activity_attribute_u32 fileHandle fd
//@activity_attribute filePosition offset
//@activity_link_int fd
//@error ''ret<0'' errno
//@guardEnd
ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset);

//@guard
//@activity
//@activity_attribute bytesToRead count
//@activity_attribute bytesRead ret
//@activity_attribute_u32 fileHandle fd
//@activity_attribute filePosition offset
//@activity_link_int fd
//@error ''ret<0'' errno
//@guardEnd
ssize_t pread(int fd, void *buf, size_t count, off_t offset);



//@guard
//@activity
//@activity_attribute bytesWritten ret
//@activity_attribute_u32 fileMemoryRegions iovcnt
//@activity_attribute_u32 fileHandle fd
//@activity_attribute filePosition offset
//@activity_link_int fd
//@error ''ret<0'' errno
//@guardEnd
ssize_t pwritev(int fd, const struct iovec *iov, int iovcnt, off_t offset);

//@guard
//@activity
//@activity_attribute bytesRead ret
//@activity_attribute_u32 fileMemoryRegions iovcnt
//@activity_attribute_u32 fileHandle fd
//@activity_attribute filePosition offset
//@activity_link_int fd
//@error ''ret<0'' errno
//@guardEnd
ssize_t preadv(int fd, const struct iovec *iov, int iovcnt, off_t offset);



//@guard
//@activity
//@guardEnd
void sync (void);

//@guard
//@activity
//@activity_attribute_u32 fileHandle fd
//@activity_link_int fd
//@error ''ret<0'' errno
//@guardEnd
int fsync (int fd);

//@guard
//@activity
//@activity_attribute_u32 fileHandle fd
//@activity_link_int fd
//@error ''ret<0'' errno
//@guardEnd
int fdatasync (int fd);


/* Hints to the system, this might be used for optimizations by SIOX in the future */
/* See also: http://insights.oetiker.ch/linux/fadvise.html */

//@guard
//@activity
//@activity_attribute_u32 fileHandle fd
//@activity_attribute filePosition offset
//@activity_attribute fileAdviseExtent len
//@activity_attribute fileAdvise advise
//@activity_link_int fd
//@error ''ret<0'' errno
//@guardEnd
int posix_fadvise (int fd, off_t offset, off_t len, int advise);


// Linux specific, Xstat is redirected to Xstat64
// stat() symbols do not exist, a macro rewrites them, problem stat64 types.
// Be aware this might lead to problems.
//@splice_once ''int stat(const char *path, struct stat *buf){ return __xstat64(1, path, buf); }''
//@guard
//@activity
//@activity_attribute_pointer fileName path
//@error ''ret<0'' errno
//@guardEnd
int __xstat64(int __ver, const char *path, struct stat64 *buf);

//@splice_once ''int lstat(const char *path, struct stat *buf){ return __lxstat64(1, path, buf); }''
//@guard
//@activity
//@activity_attribute_pointer fileName path
//@error ''ret<0'' errno
//@guardEnd
int __lxstat64(int __ver, const char *path, struct stat64 *buf);

//@splice_once ''int fstat(int fd, struct stat *buf){ return __fxstat64(1, fd, buf); }''
//@guard
//@activity
//@activity_attribute_u32 fileHandle fd
//@activity_link_int fd
//@error ''ret<0'' errno
//@guardEnd
int __fxstat64(int __ver, int fd, struct stat64 *buf);

//@guard
//@activity
//@activity_attribute_pointer fileName path
//@error ''ret<0'' errno
//@guardEnd
int __xstat(int __ver, const char *path, struct stat *buf);

//@guard
//@activity
//@activity_attribute_pointer fileName path
//@error ''ret<0'' errno
//@guardEnd
int __lxstat(int __ver, const char *path, struct stat *buf);

//@guard
//@activity
//@activity_attribute_u32 fileHandle fd
//@activity_link_int fd
//@error ''ret<0'' errno
//@guardEnd
int __fxstat(int __ver, int fd, struct stat *buf);


/* See: http://www.gnu.org/software/libc/manual/html_mono/libc.html#Memory_002dmapped-I_002fO */
/* Probably we should record this type of usage with SIOX, but we cannot track the I/O on user-space */

void * mmap (void *address, size_t length, int protect, int flags, int filedes, off_t offset);
void * mremap (void *address, size_t length, size_t new_length, int flag);
int munmap (void *addr, size_t length);
int madvise (void *addr, size_t length, int advice);



/* Asynchronous I/O */
/* see http://www.ibm.com/developerworks/linux/library/l-async/index.html */
/*
aio_read	Request an asynchronous read operation
aio_error	Check the status of an asynchronous request
aio_return	Get the return status of a completed asynchronous request
aio_write	Request an asynchronous operation
aio_suspend	Suspend the calling process until one or more asynchronous requests have completed (or failed)
aio_cancel	Cancel an asynchronous I/O request
lio_listio	Initiate a list of I/O operations
 */



// TODO STDIO interface

//On success, the function returns zero.
//In case of error, errno is set to a platform-specific positive value and the function returns a non-zero value.
//The function fills the fpos_t object ...
//int fgetpos ( FILE * stream, fpos_t * pos );
// long int ftell ( FILE * stream );
// perror();


/*
Opens the file whose name is specified in the parameter filename and associates it with a stream that can be identified in future operations by the FILE pointer returned.

If the file is successfully opened, the function returns a pointer to a FILE object that can be used to identify the stream on future operations.
Otherwise, a null pointer is returned.
On most library implementations, the errno variable is also set to a system-specific error code on failure.
 */
FILE * fopen ( const char * filename, const char * mode );

/*
The  fdopen()  function  associates a stream with the existing file descriptor, fd.  The mode of the stream (one of the
values "r", "r+", "w", "w+", "a", "a+") must be compatible with the mode of the file  descriptor.   The  file  position
indicator  of  the  new  stream  is  set to that belonging to fd, and the error and end-of-file indicators are cleared.
Modes "w" or "w+" do not cause truncation of the file.  The file descriptor is not dup'ed, and will be closed when  the
stream created by fdopen() is closed. 
 */
FILE *fdopen(int fd, const char *mode);

/*
Reuses stream to either open the file specified by filename or to change its access mode.
If a new filename is specified, the function first attempts to close any file already associated with stream (third parameter) and disassociates it. Then, independently of whether that stream was successfuly closed or not, freopen opens the file specified by filename and associates it with the stream just as fopen would do using the specified mode.

If filename is a null pointer, the function attempts to change the mode of the stream. Although a particular library implementation is allowed to restrict the changes permitted, and under which circumstances.

If the file is successfully reopened, the function returns the pointer passed as parameter stream, which can be used to identify the reopened stream.
Otherwise, a null pointer is returned.
On most library implementations, the errno variable is also set to a system-specific error code on failure.
 */
FILE * freopen ( const char * filename, const char * mode, FILE * stream );


FILE * tmpfile ( void );

int fclose ( FILE * stream );

//  If an error occurs, EOF is returned and the error indicator is set (see ferror). 
int fflush ( FILE * stream );

//On success, the character read is returned (promoted to an int value).
//The return type is int to accommodate for the special value EOF, which indicates failure:
int fgetc ( FILE * stream );

/* 
On success, the character read is returned (promoted to an int value).
The return type is int to accommodate for the special value EOF, which indicates failure:
If the position indicator was at the end-of-file, the function returns EOF and sets the eof indicator (feof) of stream.
If some other reading error happens, the function also returns EOF, but sets its error indicator (ferror) instead.
 */
int getc ( FILE * stream );



/*
Reads characters from stream and stores them as a C string into str until (num-1) characters have been read or either a newline or the end-of-file is reached, whichever happens first.

On success, the function returns str.
If the end-of-file is encountered while attempting to read a character, the eof indicator is set (feof). If this happens before any characters could be read, the pointer returned is a null pointer (and the contents of str remain unchanged).
If a read error occurs, the error indicator (ferror) is set and a null pointer is also returned (but the contents pointed by str may have changed).
 */
char * fgets ( char * str, int num, FILE * stream );

// The function fileno() examines the argument stream and returns its integer descriptor.
// Function does not fail, only if stream is invalid.
int fileno(File * stream);

/*
On success, the total number of characters written is returned.
If a writing error occurs, the error indicator (ferror) is set and a negative number is returned.
*/
int fprintf ( FILE * stream, const char * format, ... );
// call vprintf afterwards

/*
On success, the total number of characters written is returned.
If a writing error occurs, the error indicator (ferror) is set and a negative number is returned.
If a multibyte character encoding error occurs while writing wide characters, errno is set to EILSEQ and a negative number is returned.
 */
int vfprintf ( FILE * stream, const char * format, va_list arg );

/*
On success, the function returns the number of items of the argument list successfully filled. This count can match the expected number of items or be less (even zero) due to a matching failure, a reading error, or the reach of the end-of-file.
If a reading error happens or the end-of-file is reached while reading, the proper indicator is set (feof or ferror). And, if either happens before any data could be successfully read, EOF is returned.
If an encoding error happens interpreting wide characters, the function sets errno to EILSEQ.
 */
int vfscanf ( FILE * stream, const char * format, va_list arg );

/*
 Writes a character to the stream and advances the position indicator.
 On success, the character written is returned.
 If a writing error occurs, EOF is returned and the error indicator (ferror) is set.
 */
int fputc ( int character, FILE * stream );
int putc ( int character, FILE * stream );

/*
On success, a non-negative value is returned.
On error, the function returns EOF and sets the error indicator (ferror).
 */
int fputs ( const char * str, FILE * stream );

/*
The total number of elements successfully read is returned.
If this number differs from the count parameter, either a reading error occurred or the end-of-file was reached while reading. In both cases, the proper indicator is set, which can be checked with ferror and feof, respectively.
If either size or count is zero, the function returns zero and both the stream state and the content pointed by ptr remain unchanged.
 */
size_t fread ( void * ptr, size_t size, size_t count, FILE * stream );

/*
Writes an array of count elements, each one with a size of size bytes, from the block of memory pointed by ptr to the current position in the stream.

The total number of elements successfully written is returned.
If this number differs from the count parameter, a writing error prevented the function from completing. In this case, the error indicator (ferror) will be set for the stream.
If either size or count is zero, the function returns zero and the error indicator remains unchanged.
 */
size_t fwrite ( const void * ptr, size_t size, size_t count, FILE * stream );


/*
On success, the function returns the number of items of the argument list successfully filled. This count can match the expected number of items or be less (even zero) due to a matching failure, a reading error, or the reach of the end-of-file.
If a reading error happens or the end-of-file is reached while reading, the proper indicator is set (feof or ferror). And, if either happens before any data could be successfully read, EOF is returned.
If an encoding error happens interpreting wide characters, the function sets errno to EILSEQ.
 */
int fscanf ( FILE * stream, const char * format, ... );

// Redirection of stdout to a file? No, we do not handle this at the moment. 

//  Deletes the file whose name is specified in filename.
/* 
 If the file is successfully deleted, a zero value is returned.
On failure, a nonzero value is returned.
On most library implementations, the errno variable is also set to a system-specific error code on failure.
 */
int remove ( const char * filename );

/*
 If the file is successfully renamed, a zero value is returned.
On failure, a nonzero value is returned.
On most library implementations, the errno variable is also set to a system-specific error code on failure.
 */
int rename ( const char * oldname, const char * newname );

/*
Specifies the buffer to be used by the stream for I/O operations, which becomes a fully buffered stream. Or, alternatively, if buffer is a null pointer, buffering is disabled for the stream, which becomes an unbuffered stream.

This function should be called once the stream has been associated with an open file, but before any input or output operation is performed with it.
 */
void setbuf ( FILE * stream, char * buffer );

/* 

If the buffer is correctly assigned to the file, a zero value is returned.
Otherwise, a non-zero value is returned; This may be due to an invalid mode parameter or to some other error allocating or assigning the buffer.
 */
int setvbuf ( FILE * stream, char * buffer, int mode, size_t size );


