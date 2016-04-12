/*
  JK. 
  This performance test spawns a command line defined number of threads each calling fwrite() with no payload to minimize the conducted work and thus allow easy measurement of the overhead.
  
  Compile with:
  	gcc -std=gnu99 performance-test-multi-threaded.c -lpthread -o performance-test-multi-threaded -g -lrt -DUSE_STDIO

  To measure SIOX overhead run, e.g.:
	siox-inst posix performance-test-multi-threaded 2 4

  Compile with Google Profiling (for shared libraries, low overhead, nice):
	/usr/local/siox/bin/siox-inst posix wrap gcc -std=gnu99 performance-test-multi-threaded.c -g -lpthread -L /opt/gperftools/2.1/lib/ -lprofiler  -I /opt/gperftools/2.1/include/ -DGOOGLEPROF
	see: http://google-perftools.googlecode.com/svn/trunk/doc/cpuprofile.html
  To increase the sampling frequency use:
	export CPUPROFILE_FREQUENCY=1000
  After running the app call:
	/opt/gperftools/2.1/bin/pprof --text performance-test-multi-threaded file.prof 
  For a callgraph analysis:
	/opt/gperftools/2.1/bin/pprof --web performance-test-multi-threaded file.prof 
  To annotate the code with samples in OProfile fashion: (for example on the Log method)
	/opt/gperftools/2.1/bin/pprof --list=Log performance-test-multi-threaded file.prof 
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef GOOGLEPROF
#include <google/profiler.h>
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#include <time.h>

#ifdef USE_STDIO
	#warning USING STDIO fopen(), fwrite(), fclose()
#else
	#warning USING open(), write(), close()
#endif

#ifdef RECORD_INVIDUAL
	#warning RECORD INDIVIDUAL CALLS to "out.csv"
#endif

uint64_t gettime()
{
	struct timespec tp;
	if( clock_gettime( CLOCK_REALTIME, &tp ) != 0 ) {
		return -1;
	} else {
		return ( tp.tv_sec * 1000000000ull ) + tp.tv_nsec;
	}
}

// number of seconds the test runs
static int time_to_run_s = 10;
static int thread_count = 4;

static volatile uint64_t global_iterations = 100; 

pthread_barrier_t barrier;

void * threadFunc(void * data){
	int i, o;
	int thread_number = (int) data;

#ifdef USE_STDIO
	FILE * pFile;
#else
	int fd;
#endif
	char buffer[] = {'a', 'b'};
	char filename[200];
	sprintf(filename, "testfile.%d", thread_number);

#ifdef USE_STDIO
	pFile = fopen( filename, "wb" );	
#else
	fd = creat(filename, S_IRWXU);
#endif

	if( thread_number == 0 ){
		printf("Determining the number of global_iterations for 1s\n");
	}
	// determine an experiment which takes about 1s.
	pthread_barrier_wait( & barrier );

	uint64_t startT = gettime();
	double deltaT = 0;		
	while( deltaT < 1000000000ull ){		
		const uint64_t current_iterations = global_iterations;

		for(i=0; i < current_iterations; i++){
#ifdef USE_STDIO
			fwrite(buffer, 0, 0, pFile );
#else
			write(fd, buffer, 0);
#endif
		}

		pthread_barrier_wait( & barrier );

		uint64_t endT = gettime();
		deltaT = endT - startT;
		startT = endT;
		if( thread_number == 0 ){
			printf("Testing: %lld %f\n", (long long int) current_iterations, deltaT / 1000000000ull);

			if(deltaT == 0){
				global_iterations *= 100;
			}else{
				global_iterations = current_iterations / (deltaT  / 1010000000ull);
			}
		}
		pthread_barrier_wait( & barrier );
	}

	if( thread_number == 0 ){
		printf("Using %lld iterations per thread for %fs\n", (long long int) global_iterations, deltaT / 1000000000ull);
		printf("\nTime\tEvents/s\tTimePerEvent\n");
	}

	pthread_barrier_wait( & barrier );	

	uint64_t t0 = gettime();

#ifdef RECORD_INVIDUAL
	double * times = (double*) malloc(sizeof(double) * (uint64_t) time_to_run_s * global_iterations);
	memset(times, 0, ((uint64_t) sizeof(double)) * time_to_run_s * global_iterations);
	uint64_t tpos = 0;
#endif 

	for(o=0; o < time_to_run_s; o++){
		const uint64_t current_iterations = global_iterations;

		uint64_t startT = gettime();
		for(i=0; i < current_iterations; i++){
#ifdef RECORD_INVIDUAL
			uint64_t startI = gettime();
#endif 
#ifdef USE_STDIO
			fwrite(buffer, 0, 0, pFile );
#else
			write(fd, buffer, 0);
#endif
#ifdef RECORD_INVIDUAL
			times[tpos] = gettime() - startI;
			tpos++;
#endif 			
		}

		pthread_barrier_wait( & barrier );	

		double deltaT = gettime() - startT;

		if ( thread_number == 0){
			printf( "%.2f\t%.3f\t%.9f\n", deltaT/ 1000000000.0, current_iterations * thread_count / (deltaT / 1000000000ull), deltaT / 1000000000ull / current_iterations / thread_count);
		}
	}

	// cleanup phase
#ifdef USE_STDIO
	fclose( pFile );
#else
	close(fd);
#endif
	unlink(filename);

#ifdef RECORD_INVIDUAL
	FILE * csvFile;
	sprintf(filename, "out-%d.csv", thread_number);
	csvFile = fopen( filename, "w" );
	assert(csvFile != NULL);
	printf("Writing to file %s, %lld entries\n", filename, (long long unsigned) tpos);
	for (uint64_t i=0; i < tpos; i++){
		fprintf(csvFile, "%.9f\n", times[i] / 1000000000.0);
	}
	fclose(csvFile);
	free(times);
#endif

	if ( thread_number == 0){
		printf("\nTotal time\taverage \n");
		double deltaT = gettime() - t0;
		printf( "  %.2f\t%.3f %.9f\n", deltaT/ 1000000000.0, global_iterations  * thread_count * time_to_run_s / (deltaT / 1000000000ull), deltaT / time_to_run_s / 1000000000ull / global_iterations /  thread_count);
	}

}

int main( int argc, char ** argv )
{
#ifdef GOOGLEPROF
	ProfilerStart("test.prof");
#endif

	// parse command line
	if ( argc > 1 ){
		time_to_run_s = atoi(argv[1]);
	}
	if ( argc > 2 ){
		thread_count = atoi(argv[2]);
	}

	printf("SIOX threaded performance test, please run it with and without SIOX instrumentation !\n");
	printf("Parameters: %d threads, %d seconds\n", thread_count, time_to_run_s);

	pthread_barrier_init( & barrier, NULL, thread_count );

	pthread_t thread[thread_count];
	for(int i=0; i < thread_count; i++){
		pthread_create(& thread[i], NULL, threadFunc, (void * restrict) i);
	}

	for(int i=0; i < thread_count; i++){
		void * data;
		pthread_join(thread[i], & data);
	}

	pthread_barrier_destroy( & barrier );

#ifdef GOOGLEPROF
        ProfilerStop("test.prof");
#endif
	return 0;
}
