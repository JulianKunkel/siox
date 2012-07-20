/**
 * See: http://www.jedsoft.org/slang/doc/html/slangfun-15.html
 * Kurzbeschreibungen hier hineinkopieren!
 */

#include <unistd.h>
// Für readv
#include <sys/uio.h>
//
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int open(const char *pathname, int flags);
int creat(const char *pathname, mode_t mode);

int close(int fd); 

ssize_t write(int fd, const void *buf, size_t count);
/* Hier splice lseek(seekcur,0) */
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