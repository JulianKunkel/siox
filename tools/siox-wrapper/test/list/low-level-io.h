#include <unistd.h>

#include <sys/uio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <stdio.h>

//@test ''%s'' pathname
int open(const char *pathname, int flags, ...);

//@test 
int close(int fd);

//@test ''%ld,%ld'' count,(long)ret
ssize_t write(int fd, const void *buf, size_t count);
