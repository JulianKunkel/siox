#include <pthread.h>
#include <assert.h>
#include <stdio.h>


#define NUM_THREADS     10

void * doIO( void * threadid )
{
	FILE * file = ( FILE * ) threadid;
	fwrite( "TEST", 4, 1, file );
	pthread_exit( NULL );
}

int main()
{
	pthread_t threads[NUM_THREADS];
	FILE * myFile = fopen( "testfile", "w" );
	for( int t = 0; t < NUM_THREADS; t++ ) {
		printf( "Creating %d\n", t );
		assert( pthread_create( &threads[t], NULL, doIO, myFile ) == 0 );
	}
	for( int t = 0; t < NUM_THREADS; t++ ) {
		void * status;
		pthread_join( threads[t], &status );
	}
	fclose( myFile );
	exit( 0 );
}
