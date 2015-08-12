#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main( int argc, char const * argv[] )
{

	const char * data = "Somebody had to put all of this confusion here!\n";
	size_t len = strlen( data ) * sizeof( char );

	int fh = open( "do-not-exist", O_RDWR );
		
	pid_t pid = fork();

	if ( pid != 0){
		int fh = open( "tmp_posix_test", O_CREAT | O_RDWR, S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR );
		if( fh < 0 ) {
			fprintf( stderr, "Error while opening 'tmp_posix_test'\n" );
			exit( 1 );
		}
		ssize_t bytesWritten = write( fh, ( const void * ) data , len );
		if( bytesWritten < 0 ) {
			fprintf( stderr, "Error while writing to 'tmp_posix_test'\n" );
			exit( 1 );
		}

		int ret = close( fh );
		if( ret != 0 ) {
			fprintf( stderr, "Error while closing file 'tmp_posix_test'\n" );
		}
	}else{
		int ret = close(4855);
	}


	printf("Hello from %lld\n", (long long int) pid);

	return 0;
}
