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

/* Set the interface name for the library*/
//interface_name "POSIX"

/* Register the data types for the descriptors */
//register_datatype "FileName" SIOX_STORAGE_STRING dtid_FileName

//register_datatype "FileHandle" SIOX_STORAGE_64_BIT_INTEGER dtid_FileHandle

/* Register the metrics to grab the performance data */
//register_metric "Bytes written." "Count of bytes written to file."
//SIOX_UNIT_BYTES SIOX_STORAGE_64_BIT_INTEGER SIOX_SCOPE_SUM mid_bytes_written

//register_metric "Bytes read." "Count of bytes read form file"
//SIOX_UNIT_BYTES SIOX_STORAGE_64_BIT_INTEGER SIOX_SCOPE_SUM mid_bytes_read

/* Register the descriptor mapping form a file name to a file handle */
//register_descriptor_map dtid_FileName dtid_FileHandle dmid_open

/* Receive the file name descriptor and map it to the file handle descriptor */
//receive_descriptor dtid_FileName "FileName"
//map_descriptor dmid_open "FileName" "FileHandle"
int open(const char *pathname, int flags, ...);

int creat(const char *pathname, mode_t mode);

/* The close function destroys the file handle descriptor */
//receive_descriptor dtid_FileHandle "FileHandle"
//release_descriptor dtid_FIleHandle "FileHandle"
int close(int fd);

/* The write function starts an activity and reports it */
//receive_descriptor dtid_FileHandle "FileHandle"
//activity dtid_FileHandle "FileHandle" mid_bytes_written SIOX_TYPE_INTEGER &count "Write to disk."
ssize_t write(int fd, const void *buf, size_t count);

/* The read function starts an activity and reports it*/
//receive_descriptor dtid_FileHandle "FileHAndle"
//activity dtid_FileHandle "FileHandle" mid_bytes_read SIOX_TYPE_INTEGER &count "Read from disk."
ssize_t read(int fd, const void *buf, size_t count);

ssize_t pread(int fd, void *buf, size_t count, off_t offset);
ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset);

ssize_t readv(int fd, const struct iovec *iov, int iovcnt);
ssize_t writev(int fd, const struct iovec *iov, int iovcnt);

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

/* See: http://www.gnu.org/software/libc/manual/html_mono/libc.html#Synchronizing-I_002fO */
/* Problem: Woher sollen wir wissen, welche Dateien betroffen sind? => Tot? */
/* Stellenweise optional, ggf. von SIOX zu Optimierung nutzbar? */
int sync (void);
int fsync (int fildes);
int fdatasync (int fildes);

/* See: http://www.gnu.org/software/libc/manual/html_mono/libc.html#Memory_002dmapped-I_002fO */
/* Probably we should record this type of usage with SIOX, but how... */
/* Zumindest müssen wir uns merken, daß hier ein Mapping stattfand, damit wir ggf. auf unteren Schichten
   tricksen können (PageIn/PageOUT, Memory Management, etc.), um die Aktivitäten tracken zu können. */
void * mmap (void *address, size_t length, int protect, int flags, int filedes, off_t offset);

void * mremap (void *address, size_t length, size_t new_length, int flag);

int munmap (void *addr, size_t length);

/* Hints to the system, this might be used for optimizations by SIOX in the future */
/* See also: http://insights.oetiker.ch/linux/fadvise.html */
/* Wie ein Attribut als "tunebarer Parameter" kennzeichnen?! */
/* Wie als "Parameter, dessen Tuningeffekte wir ggf. beobachten wollen"? */
/* Wie den Scope des Tunings beschreiben? */
int madvise (void *addr, size_t length, int advice);
int posix_fadvise (int fd, __off_t offset, __off_t len, int advise);

/* Map-Kette um ein Glied (fd->ret) verlängern? Oder alles, was (->fd) mapt, ebenfalls (->ret) mappen? */
int dup_fd (int fd);
