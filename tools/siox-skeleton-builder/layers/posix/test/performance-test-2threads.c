/*
  MW. Threads
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
  To annotate the code with samples in OProfile fashion: (for example on the Log method)
	/opt/gperftools/2.1/bin/pprof --list=Log ./a.out file.prof 
 */



#ifdef GOOGLEPROF
#include <google/profiler.h>
#endif

#include <stdio.h>
#include <stdint.h>

#include <time.h>

#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

pthread_t tid[2];

void* CreateThreads(void *arg)
{
    unsigned long i = 0;
    pthread_t id = pthread_self();

    if(pthread_equal(id,tid[0]))
    {
        printf("\n First thread processing\n");
    }
    else
    {
        printf("\n Second thread processing\n");
    }

    for(i=0; i<(0xFFFFFFFF);i++);

    return NULL;
}

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

int main(void)
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
	int err;

	int pFile;
	char buffer[] = {'a', 'b'};
	pFile = open( "testfile.bin", "wb" );	

	printf("Determining the number of iterations for 1s\n");
	// determine an experiment which takes about 1s.
	uint64_t startT = gettime();
	double deltaT = 0;
	uint64_t iterations = 100;
	while( deltaT < 1000000000ull ){
		for(i=0; i < iterations; i++){
			int j = 0;
			    while(j < 2)
			    {
			        err = pthread_create(&(tid[j]), NULL, &CreateThreads, NULL);
			        if (err != 0)
			            printf("\ncan't create thread :[%s]", strerror(err));
			        else
			            printf("\n Thread created successfully\n");
			        j++;
			    }

    		sleep(5);
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
			write(pFile, buffer, 1 );
		}
		double deltaT = gettime() - startT;
		printf( "%.2f %.3f %.9f\n", deltaT/ 1000000000.0, iterations / (deltaT / 1000000000ull), deltaT / 1000000000ull / iterations);
	}

	{
	printf("\nTotal and averages\n");
	double deltaT = gettime() - t0;
	printf( "%.2f %.3f %.9f\n", deltaT/ 1000000000.0, iterations * count / (deltaT / 1000000000ull), deltaT / count / 1000000000ull / iterations);
	}


	close( pFile );
#ifdef GOOGLEPROF
        ProfilerStop("test.prof");
#endif
	return 0;
}
