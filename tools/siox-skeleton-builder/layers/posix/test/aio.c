#include <aio.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

int main()
{
	int fd, ret;
	struct aiocb my_iocb;

	fd = open( "siox.conf", O_RDONLY );
	if( fd < 0 ) perror( "open" );

	memset( & my_iocb, 0,  sizeof( struct aiocb ) );

	my_iocb.aio_buf = malloc( 1024 );
	if( !my_iocb.aio_buf ) perror( "malloc" );

	my_iocb.aio_nbytes = 1000;
	my_iocb.aio_fildes = fd;
	my_iocb.aio_offset = 0;

	ret = aio_read( & my_iocb );

	if( ret < 0 ) perror( "aio_read" );

	while( aio_error( & my_iocb ) == EINPROGRESS ) {
		sleep( 1 );
	}

	( ( char * ) my_iocb.aio_buf )[1000] = 0;

	if( ( ret = aio_return( &my_iocb ) ) > 0 ) {
		printf( "Read: %s\n", ( char * ) my_iocb.aio_buf );
	} else {
		perror( "read error" );
	}
}
