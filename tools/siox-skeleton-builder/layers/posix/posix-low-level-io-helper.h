#include "siox-datatypes-helper-posix-low-level-io.h"

#include <sys/types.h>
#include <unistd.h>
#include <glib.h>



/* Functions that need helpers:
 *
 * readv
 * writev
 *
 */
static inline unsigned translatePOSIXFlagsToSIOX( unsigned flags )
{
	return ( ( flags & SIOX_LOW_LEVEL_O_RDONLY ) > 0 ? SIOX_LOW_LEVEL_O_RDONLY : 0 )
		| ( ( flags & SIOX_LOW_LEVEL_O_WRONLY ) > 0 ? SIOX_LOW_LEVEL_O_WRONLY : 0 )
		| ( ( flags & SIOX_LOW_LEVEL_O_RDWR ) > 0 ? SIOX_LOW_LEVEL_O_RDWR : 0 )
		| ( ( flags & SIOX_LOW_LEVEL_O_APPEND ) > 0 ? SIOX_LOW_LEVEL_O_APPEND : 0 )
		| ( ( flags & SIOX_LOW_LEVEL_O_ASYNC ) > 0 ? SIOX_LOW_LEVEL_O_ASYNC : 0 )
		| ( ( flags & SIOX_LOW_LEVEL_O_CLOEXEC ) > 0 ? SIOX_LOW_LEVEL_O_CLOEXEC : 0 )
		| ( ( flags & SIOX_LOW_LEVEL_O_CREAT ) > 0 ? SIOX_LOW_LEVEL_O_CREAT : 0 )
		| ( ( flags & SIOX_LOW_LEVEL_O_DIRECTORY ) > 0 ? SIOX_LOW_LEVEL_O_DIRECTORY : 0 )
		| ( ( flags & SIOX_LOW_LEVEL_O_EXCL ) > 0 ? SIOX_LOW_LEVEL_O_EXCL : 0 )
		| ( ( flags & SIOX_LOW_LEVEL_O_NOCTTY ) > 0 ? SIOX_LOW_LEVEL_O_NOCTTY : 0 )
		| ( ( flags & SIOX_LOW_LEVEL_O_NOFOLLOW ) > 0 ? SIOX_LOW_LEVEL_O_NOFOLLOW : 0 )
		| ( ( flags & SIOX_LOW_LEVEL_O_NONBLOCK ) > 0 ? SIOX_LOW_LEVEL_O_NONBLOCK : 0 )
		| ( ( flags & SIOX_LOW_LEVEL_O_TRUNC ) > 0 ? SIOX_LOW_LEVEL_O_TRUNC : 0 );
}


static inline unsigned prepareIovec( const struct iovec * iov, int iovcnt ) {
	return 0;
}
