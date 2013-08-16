/**
 * See: http://www.jedsoft.org/slang/doc/html/slangfun-15.html
 * Kurzbeschreibungen hier hineinkopieren!
 */

#include <unistd.h>
#include <sys/uio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dlfcn.h>
//#include <fcntl.h>
#include <stdio.h>
#include <errno.h>

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


/* Asynchrone IO lassen wir hier mal weg */
/* Falls doch, siehe: http://www.ibm.com/developerworks/linux/library/l-async/index.html */
/*
aio_read	Request an asynchronous read operation
aio_error	Check the status of an asynchronous request
aio_return	Get the return status of a completed asynchronous request
aio_write	Request an asynchronous operation
aio_suspend	Suspend the calling process until one or more asynchronous requests have completed (or failed)
aio_cancel	Cancel an asynchronous I/O request
lio_listio	Initiate a list of I/O operations
 */

/* See: http://www.gnu.org/software/libc/manual/html_mono/libc.html#Memory_002dmapped-I_002fO */
/* Probably we should record this type of usage with SIOX, but how... */
/* Zumindest müssen wir uns merken, daß hier ein Mapping stattfand, damit wir ggf. auf unteren Schichten
   tricksen können (PageIn/PageOUT, Memory Management, etc.), um die Aktivitäten tracken zu können. */
void * mmap (void *address, size_t length, int protect, int flags, int filedes, off_t offset);

void * mremap (void *address, size_t length, size_t new_length, int flag);

int munmap (void *addr, size_t length);
int madvise (void *addr, size_t length, int advice);

// TODO File stuff
// int fileno(File * stream);