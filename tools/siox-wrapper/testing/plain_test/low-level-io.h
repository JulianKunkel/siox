#include <unistd.h>

#include <sys/uio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <stdio.h>

//@printf "open %s\n",pathname
int open(const char *pathname, int flags, ...);

//@printf "close\n"
int close(int fd);

//@printf "write kram\n"
ssize_t write(int fd, const void *buf, size_t count);
