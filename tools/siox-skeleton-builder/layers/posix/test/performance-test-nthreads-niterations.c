/*
  MW. Threads that do write()
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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#ifdef GOOGLEPROF
#include <google/profiler.h>
#endif

#include <stdio.h>
#include <stdint.h>

#include <time.h>

#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h> //unlink() etc.



pthread_t tid[1];


#define FILEP1 "~/testf1.bin"
#define FILEP2 "~/testf2.bin"


//Funktion mit Übergabe der Iterationen
void* CreateThreads(void *arg)
{
    unsigned long m = 0;
    int pf1;
    int pf2;
    char buffer[] = {'a','b'};
    pthread_t id = pthread_self();

    if(pthread_equal(id,tid[0]))
    {
        //pf1 = open( "testf1.bin", O_TRUNC | O_CREAT );
        printf("\n First thread processing\n");
	//write(pf1, buffer, 1 );
        //close ( pf1 );
    }
    else
    {
        //pf2 = open( "testf2.bin", O_TRUNC | O_CREAT );
        printf("\n Second thread processing\n");
        //write(pf2, buffer, 1 );
        //close ( pf2 );
    }

    //for(m=0; m<(0xFFFFFFFF);m++);

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



//MAIN

// for how many tests: testcount

// for n threads
// create threads

// optional EXEC pthread barrier


// join
// unlink

int main( int argc, char ** argv )
{

	uint64_t test_count     =  10;
        int thread_count    	=  8;

#ifdef GOOGLEPROF
	ProfilerStart("test.prof");
#endif

	// parse command line
	if ( argc > 1 ){
		test_count = atoi(argv[1]);
	}


	printf("SIOX performance test, please run it with and without SIOX instrumentation !\n");
	// Iterators
	int tc,thc,j,o,s;
	int err_tcreat,err_tjoin;

	//int pFile;
	//char buffer[] = {'a', 'b'};
	//pFile = open( "testfile.bin", O_TRUNC | O_CREAT );	


	printf("Determining the number of iterations for 1s\n");
	// determine an experiment which takes about 1s.
	uint64_t startT = gettime();
	double deltaT = 0;
	
	while( deltaT < 1000000000ull ){
		for(j=0; j < test_count; j++){
			for(thc=0; thc< thread_count; thc++){
			        err_tcreat = pthread_create(&(tid[thc]), NULL, &CreateThreads, NULL);
			        if (err_tcreat != 0){
			            printf("\nCan't create thread :[%s]", strerror(err_tcreat));
			        }else{printf("\n Thread created successfully\n");}
				err_tjoin = pthread_join (tid[thc], NULL);
				
			        if (err_tjoin != 0){
                                    printf("\nJoin error", strerror(err_tjoin));
				}else{printf("\n Thread joined successfully\n");}
     			}
		}

		uint64_t endT = gettime();
		deltaT = endT - startT;
		startT = endT;
		printf("Testing: %lld %f\n", (long long int) test_count, deltaT / 1000000000ull);

		if(deltaT == 0){
			test_count *= 10;
		}else{
			test_count = test_count / (deltaT  / 1010000000ull);
		}
	}

	printf("Using %lld threads for %fs\n", (long long int) thread_count, deltaT / 1000000000ull);

	uint64_t t0 = gettime();

	printf("\nTime Events/s TimePerEvent\n");
	for (tc=0; tc < test_count; tc++ ){

		uint64_t startT = gettime();
		for(s=0; s < thread_count; s++){

                                err_tcreat = pthread_create(&(tid[s]), NULL, &CreateThreads, NULL);
                                if (err_tcreat != 0){
                                    printf("\ncan't create thread :[%s]", strerror(err_tcreat));
                                }else{
                                    	printf("\n Thread created successfully\n");}
				err_tjoin = pthread_join(tid[s], NULL);
                                if (err_tjoin != 0){
                                    printf("\nJoin error", strerror(err_tjoin));
                                }else{printf("\n Thread joined successfully\n");}

                            }

		double deltaT = gettime() - startT;
		printf( "%.2f %.3f %.9f\n", deltaT/ 1000000000.0, test_count / (deltaT / 1000000000ull), deltaT / 1000000000ull / test_count);

                //unlink(FILEP1)
                //unlink/FILEP2)
 
	} //test_count End


	{
	printf("\nTotal and averages\n");
	double deltaT = gettime() - t0;
	printf( "%.2f %.3f %.9f\n", deltaT/ 1000000000.0, test_count * thread_count / (deltaT / 1000000000ull), deltaT / thread_count / 1000000000ull / test_count);
	}


#ifdef GOOGLEPROF
        ProfilerStop("test.prof");
#endif
	return 0;
}
