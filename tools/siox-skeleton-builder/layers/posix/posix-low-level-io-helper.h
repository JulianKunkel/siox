#include "siox-datatypes-helper-posix.h"

#include <sys/types.h>
#include <unistd.h>
#include <glib.h>
#include <fcntl.h>

/* Functions that need helpers:
 *
 * readv
 * writev
 *
 */
static inline unsigned translatePOSIXFlagsToSIOX( unsigned flags )
{
	return ( ( flags & O_RDONLY ) > 0 ? SIOX_LOW_LEVEL_O_RDONLY : 0 )
		| ( ( flags & O_WRONLY ) > 0 ? SIOX_LOW_LEVEL_O_WRONLY : 0 )
		| ( ( flags & O_RDWR ) > 0 ? SIOX_LOW_LEVEL_O_RDWR : 0 )
		| ( ( flags & O_APPEND ) > 0 ? SIOX_LOW_LEVEL_O_APPEND : 0 )
		| ( ( flags & O_ASYNC ) > 0 ? SIOX_LOW_LEVEL_O_ASYNC : 0 )
		| ( ( flags & O_CLOEXEC ) > 0 ? SIOX_LOW_LEVEL_O_CLOEXEC : 0 )
		| ( ( flags & O_CREAT ) > 0 ? SIOX_LOW_LEVEL_O_CREAT : 0 )
		| ( ( flags & O_DIRECTORY ) > 0 ? SIOX_LOW_LEVEL_O_DIRECTORY : 0 )
		| ( ( flags & O_EXCL ) > 0 ? SIOX_LOW_LEVEL_O_EXCL : 0 )
		| ( ( flags & O_NOCTTY ) > 0 ? SIOX_LOW_LEVEL_O_NOCTTY : 0 )
		| ( ( flags & O_NOFOLLOW ) > 0 ? SIOX_LOW_LEVEL_O_NOFOLLOW : 0 )
		| ( ( flags & O_NONBLOCK ) > 0 ? SIOX_LOW_LEVEL_O_NONBLOCK : 0 )
		| ( ( flags & O_TRUNC ) > 0 ? SIOX_LOW_LEVEL_O_TRUNC : 0 );
}

/*
 This helper function translates the mode flag from stdio to the same flags as for low-level I/O.
 Not storing this short string conserves space and standardizes these attributes.
 */
static inline unsigned translateFILEFlagsToSIOX( const char * str )
{
	unsigned flags = 0;

	for(const char * c = str; *c != '\0' ; c++ ){
		switch(*c){
			case 'a':
				flags |= SIOX_LOW_LEVEL_O_APPEND | SIOX_LOW_LEVEL_O_CREAT;
				break;
			case 'r':
				flags |= SIOX_LOW_LEVEL_O_RDONLY;
				break;
			case 'w':
				flags |= SIOX_LOW_LEVEL_O_WRONLY | SIOX_LOW_LEVEL_O_CREAT | SIOX_LOW_LEVEL_O_TRUNC;
				break;
			case '+':
				flags |= SIOX_LOW_LEVEL_O_RDWR;
				break;
		}
	}

	if( flags & SIOX_LOW_LEVEL_O_RDONLY && flags & SIOX_LOW_LEVEL_O_WRONLY){
		flags = (flags | SIOX_LOW_LEVEL_O_RDWR) & ~SIOX_LOW_LEVEL_O_RDONLY & ~SIOX_LOW_LEVEL_O_WRONLY;
	}

	return flags;
}



static inline unsigned prepareIovec( const struct iovec * iov, int iovcnt ) {
	return 0;
}
