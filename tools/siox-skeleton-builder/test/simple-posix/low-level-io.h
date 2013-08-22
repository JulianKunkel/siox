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
//@component "POSIX" "LINUX" "Test-Kiste"

//@horizontal_map_create_str

/* Register the metrics to grab the performance data */
//@register_attribute attr_write "POSIX" "Bytes written"
//@SIOX_STORAGE_64_BIT_UINTEGER

//@register_attribute attr_read "POSIX" "Bytes read"
//@SIOX_STORAGE_64_BIT_UINTEGER

/*------------------------------------------------------------------------------
End of global part
------------------------------------------------------------------------------*/

//@activity ComponentVariable=activ_open ActivityVariable=cur_open
//@horizontal_map_put_str "file open" Activity=cur_open
int open( const char * pathname, int flags, ... );


//@activity ActivityVariable=cur_close ComponentVariable=activ_close
//@activiry_link_str "file open" Activity=cur_close
int close( int fd );

//@activity  ActivityVariable=activity_wirte ComponentVariable=cur_wirte
//@activity_link_str "file open" Activity=cur_wirte
ssize_t write( int fd, const void * buf, size_t count );

