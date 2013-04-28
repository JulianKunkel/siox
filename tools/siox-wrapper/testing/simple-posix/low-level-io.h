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
#include <stdlib.h>

/* Set the interface name for the library*/
//@register_node SWID="POSIX"

/* Register the metrics to grab the performance data */
//@metric_register mid_bytesWritten "Bytes written."
//SIOX_UNIT_BYTES SIOX_STORAGE_64_BIT_INTEGER SIOX_SCOPE_SUM

//@metric_register mid_bytesRead "Bytes read."
//SIOX_UNIT_BYTES SIOX_STORAGE_64_BIT_INTEGER SIOX_SCOPE_SUM

/*------------------------------------------------------------------------------
End of global part
------------------------------------------------------------------------------*/

//@activity
int open(const char *pathname, int flags, ...);


//@activity aID_close Name="close"
//@activity_link aID "fd"
int close(int fd);

//@activity aID_write NULL NULL NULL "Write to file."
//@report global_unid mid_bytesWritten count
//@activity_link aID "fd"
ssize_t write(int fd, const void *buf, size_t count);

