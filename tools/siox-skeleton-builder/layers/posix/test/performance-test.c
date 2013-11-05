/*
  JK. This simple performance test calls fwrite with 0 bytes to fetch which will complete ASAP.
  To measure SIOX overhead run:
	siox-inst posix ./a.out
  To validate the result run without SIOX instrumentation which should create 100 million OPs/s.
	On my laptop: 204190398 ops/s without SIOX instrumentation.

  Compile with Google Profiling (for shared libraries, low overhead, nice):
	/usr/local/siox/bin/siox-inst posix wrap gcc performance-test.c -L /opt/gperftools/2.1/lib/ -lprofiler  -I /opt/gperftools/2.1/include/ -DGOOGLEPROF
	see: http://google-perftools.googlecode.com/svn/trunk/doc/cpuprofile.html
  To increase the sampling frequency use:
	export CPUPROFILE_FREQUENCY=1000
  After running the app call:
	/opt/gperftools/2.1/bin/pprof --text ./a.out file.prof 
  For a callgraph analysis:
	/opt/gperftools/2.1/bin/pprof --web ./a.out file.prof 
 */



#ifdef GOOGLEPROF
#include <google/profiler.h>
#endif

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
	int count = 10;
#ifdef GOOGLEPROF
	ProfilerStart("test.prof");
#endif

	// parse command line
	if ( argc > 1 ){
		count = atoi(argv[1]);
	}

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

	uint64_t t0 = gettime();

	printf("\nTime Events/s TimePerEvent\n");
	for(o=0; o < count; o++){
		uint64_t startT = gettime();
		for(i=0; i < iterations; i++){
			fwrite( buffer, 0 , 0, pFile );
		}
		double deltaT = gettime() - startT;
		printf( "%.2f %.3f %.9f\n", deltaT/ 1000000000.0, iterations / (deltaT / 1000000000ull), deltaT / 1000000000ull / iterations);
	}

	{
	printf("\nTotal and averages\n");
	double deltaT = gettime() - t0;
	printf( "%.2f %.3f %.9f\n", deltaT/ 1000000000.0, iterations * count / (deltaT / 1000000000ull), deltaT / count / 1000000000ull / iterations);
	}


	fclose( pFile );
#ifdef GOOGLEPROF
        ProfilerStop("test.prof");
#endif
	return 0;
}
