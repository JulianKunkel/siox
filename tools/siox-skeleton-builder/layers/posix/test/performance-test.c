/*
  JK. This simple performance test calls fwrite with 0 bytes to fetch which will complete ASAP.
  To measure SIOX overhead run:
	siox-inst posix ./a.out
  To validate the result run without SIOX instrumentation which should create 100 million OPs/s.
	On my laptop: 204190398 ops/s without SIOX instrumentation.
 */

#include <stdio.h>
#include <stdint.h>

#include <time.h>

uint64_t gettime()
{
	struct timespec tp;
	if( clock_gettime( CLOCK_REALTIME, &tp ) != 0 ) {
		return -1;
	} else {
		return ( tp.tv_sec * 1000000000ull ) + tp.tv_nsec;
	}
}

int main( int argc, char ** argv )
{
	printf("SIOX performance test, please run it with and without SIOX instrumentation !\n");
	int i, o;
	FILE * pFile;
	char buffer[] = {'a', 'b'};
	pFile = fopen( "testfile.bin", "wb" );	

	printf("Determining the number of iterations for 1s\n");
	// determine an experiment which takes about 1s.
	uint64_t startT = gettime();
	double deltaT = 0;
	uint64_t iterations = 100;
	while( deltaT < 1000000000ull ){	
		for(i=0; i < iterations; i++){
			fwrite( buffer, 0 , 0, pFile );			
		}

		uint64_t endT = gettime();
		deltaT = endT - startT;
		startT = endT;
		printf("Testing: %lld %f\n", (long long int) iterations, deltaT / 1000000000ull);

		if(deltaT == 0){
			iterations *= 100;
		}else{
			iterations = iterations / (deltaT  / 1010000000ull);
		}
	}

	printf("Using %lld iterations for %fs\n", (long long int) iterations, deltaT / 1000000000ull);

	printf("Time Events/s\n");
	for(o=0; o < 100; o++){
		uint64_t startT = gettime();
		for(i=0; i < iterations; i++){
			fwrite( buffer, 0 , 0, pFile );			
		}
		double deltaT = gettime() - startT;
		printf( "%.2f %.3f\n", deltaT/ 1000000000.0 , iterations / (deltaT / 1000000000ull));
	}
	fclose( pFile );
	return 0;
}
