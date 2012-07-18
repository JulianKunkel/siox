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


