/**
 * See: http://www.jedsoft.org/slang/doc/html/slangfun-15.html
 */

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int open(const char *pathname, int flags);
int creat(const char *pathname, mode_t mode);

int close(int fd); 

ssize_t write(int fd, const void *buf, size_t count);
ssize_t read(int fd, const void *buf, size_t count);

ssize_t pread(int fd, void *buf, size_t count, off_t offset);
ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset);

ssize_t readv(int fd, const struct iovec *iov, int iovcnt);
ssize_t writev(int fd, const struct iovec *iov, int iovcnt);

/* AIO lassen wir hier mal weg */

/* See: http://www.gnu.org/software/libc/manual/html_mono/libc.html#Synchronizing-I_002fO */
int sync (void);
int fsync (int fildes);
int fdatasync (int fildes);

/* See: http://www.gnu.org/software/libc/manual/html_mono/libc.html#Memory_002dmapped-I_002fO */
/* Probably we should record this type of usage with SIOX, but how... */
void * mmap (void *address, size_t length, int protect, int flags, int filedes, off_t offset);

void * mremap (void *address, size_t length, size_t new_length, int flag)

int munmap (void *addr, size_t length);

/* Hints to the system, this might be used for optimizations by SIOX in the future */
/* See also: http://insights.oetiker.ch/linux/fadvise.html */
int madvise (void *addr, size_t length, int advice);
int posix_fadvise (int fd, __off_t offset, __off_t len, int advise);
