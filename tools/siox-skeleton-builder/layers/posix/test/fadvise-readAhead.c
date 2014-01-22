#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <stdint.h>
#include <sys/mman.h>

/*
This program reads data and tries to prefetch it using posix_fadvise() one iteration before.
It waits some time (simulating computation) between the iteration to allow the read-ahead to take effect.
 */

// 

/*
For the paper:
gcc -Wall -g ./fadvise-readAhead.c -std=gnu99 -lrt -o fadvise-readAhead  -DFADVISE2 -DFADVISE -DTIME_THINK=100

Running on fully cached data without posix_fadvise(): 
gcc -Wall -g ./fadvise-readAhead.c -std=gnu99 -lrt -o fadvise-readAhead
 	iter: 52428 stride: 20480 blocksize: 1024 runtime: 9.582337s iotime: 0.115850s volume: 51.199219 MiB throughput: 441.943345 MiB/s; pages cached: 204.796875 MiB, pages not cached: 0.000000 MiB

Running on fully cached data with posix_fadvise() 
	gcc -Wall -g ./fadvise-readAhead.c -std=gnu99 -lrt -o fadvise-readAhead  -DFADVISE2 -DFADVISE -DTIME_THINK=100
	iter: 52428 stride: 20480 blocksize: 1024 runtime: 9.527199s iotime: 0.101692s volume: 51.199219 MiB throughput: 503.473363 MiB/s; pages cached: 204.796875 MiB, pages not cached: 0.000000 MiB
=> costs of fadvise() are neglectable

Dropping cache: echo 3 > /proc/sys/vm/drop_caches

Without fadvise()
	iter: 52428 stride: 20480 blocksize: 1024 runtime: 15.320709s iotime: 6.145555s volume: 51.199219 MiB throughput: 8.331098 MiB/s; pages cached: 0.113281 MiB, pages not cached: 204.683594 MiB
	Zeiten: 6.1, 5.1, 6.4, 6.15, 6.3, 6.33

With fadvise()
gcc -Wall -g ./fadvise-readAhead.c -std=gnu99 -lrt -o fadvise-readAhead  -DFADVISE2 -DFADVISE -DTIME_THINK=100
	iter: 52428 stride: 20480 blocksize: 1024 runtime: 10.643501s iotime: 1.548526s volume: 51.199219 MiB throughput: 33.063194 MiB/s; pages cached: 190.914062 MiB, pages not cached: 13.882812 MiB
	Zeiten: 1.65, 1.55, 1.58, 1.61, 1.67, 1.67


Reverse:

gcc -Wall -g ./fadvise-readAhead.c -std=gnu99 -lrt -o fadvise-readAhead  -DFADVISE2 -DFADVISE -DTIME_THINK=100

for I in 1 2 3 ; do  sudo bash drop.sh  ;  ./fadvise-readAhead /tmp/test ; done
	iter: 52428 stride: 20480 blocksize: 1024 runtime: 26.574617s iotime: 17.024949s volume: 51.199219 MiB throughput: 3.007305 MiB/s; pages cached: 186.437500 MiB, pages not cached: 18.359375 MiB
	iter: 52428 stride: 20480 blocksize: 1024 runtime: 27.864839s iotime: 18.602371s volume: 51.199219 MiB throughput: 2.752295 MiB/s; pages cached: 186.230469 MiB, pages not cached: 18.566406 MiB
	iter: 52428 stride: 20480 blocksize: 1024 runtime: 27.115805s iotime: 17.432340s volume: 51.199219 MiB throughput: 2.937025 MiB/s; pages cached: 185.855469 MiB, pages not cached: 18.941406 MiB
gcc -Wall -g ./fadvise-readAhead.c -std=gnu99 -lrt -o fadvise-readAhead  -DTIME_THINK=100 -DREVERSE./fadvise-readAhead.c: In function ‘main’:
./fadvise-readAhead.c:134:2: warning: #warning Reversing IO [-Wcpp]
./fadvise-readAhead.c:136:9: warning: unused variable ‘positionNext’ [-Wunused-variable]
for I in 1 2 3 ; do  sudo bash drop.sh  ;  ./fadvise-readAhead /tmp/test ; done
	iter: 52428 stride: 20480 blocksize: 1024 runtime: 39.590862s iotime: 29.981531s volume: 51.199219 MiB throughput: 1.707692 MiB/s; pages cached: 0.078125 MiB, pages not cached: 204.718750 MiB
	iter: 52428 stride: 20480 blocksize: 1024 runtime: 39.159108s iotime: 29.275999s volume: 51.199219 MiB throughput: 1.748846 MiB/s; pages cached: 0.062500 MiB, pages not cached: 204.734375 MiB
	iter: 52428 stride: 20480 blocksize: 1024 runtime: 39.990411s iotime: 30.436572s volume: 51.199219 MiB throughput: 1.682161 MiB/s; pages cached: 0.046875 MiB, pages not cached: 204.750000 MiB

With SIOX (No Multiplexer forwarding, otherwise + 0.3s)
	2.82s, 2.78, 2.7
Overhead:
	+ 0.54s siox_activity_end()
	+ 0.17s siox_activity_start()
	+ 0.26s siox_activity_set_attribute()
	+ 0.1s siox_link_to_parent()
	+ 0.9s time for siox_activity_end() with fadviseReadAhead Plugin
*/

// total file size 1 KiB
#define SIZE_MIB 1*1024ul*1024
#define SIZE_BYTE SIZE_MIB*1024

// 20 KiB stride which is a multiple of page size
#define STRIDE 2048000

// block length
#define BLOCK_LEN 1024

#ifndef TIME_THINK
#define TIME_THINK 100
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

int main( int argc, char const * argv[] )
{
	const char * filename = "tmp_test";
	if (argc > 1){
		filename = argv[1];
	}

	ssize_t bytesWritten;
	int fh = open( filename, O_CREAT | O_RDWR, S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR );
	if( fh < 0 ) {
		fprintf( stderr, "Error while opening file\n" );
		exit( 1 );
	}

	struct stat fileStat;
   fstat(fh, & fileStat);
	if ( fileStat.st_blocks * 512ul < SIZE_BYTE ){
		char * buff = malloc (1024*1024);
		printf( "Warning, file not existing/large enough, I will pre-fill it\n" );
		for (long i=0; i < SIZE_MIB / 1024 + 1; i++){
			bytesWritten = write( fh, buff, 1024*1024);
		}
		free(buff);
	}

	// memory map the file to check if pages are resident in memory
	void * file_mmaped;
	const size_t page_size = getpagesize();
	unsigned char * mincore_vector = malloc(BLOCK_LEN / page_size + 1);
	file_mmaped = mmap(0, SIZE_BYTE, PROT_NONE, MAP_SHARED, fh, 0);

	if (file_mmaped == MAP_FAILED){
		fprintf(stderr, "Error in mmap: %s\n", strerror(errno));
	}

	size_t pages_in_memory = 0;
	size_t pages_not_in_memory = 0;

	assert(SIZE_BYTE % page_size == 0);
	assert(STRIDE % page_size == 0);

	long iter = SIZE_BYTE / STRIDE;
	char * data = malloc(BLOCK_LEN);
	memset(data, 0, BLOCK_LEN);

#ifdef FADVISE
#warning "Using fadvise() to set range to random"
	{
	// DO NOT USE THIS OTHERWISE FADV DOES NOT WORK
	// disable read-ahead
	int pret = posix_fadvise(fh, 0, 0, POSIX_FADV_RANDOM );
	if (pret != 0){
		fprintf(stderr, "Error in fadvise POSIX_FADV_RANDOM: %s\n", strerror(pret));
	}
	}
#endif

	uint64_t start_time = gettime();
	uint64_t totalIOtime = 0;
#ifdef REVERSE
#warning Reversing IO
	for(long i=iter-1; i >= 0; i-- ){
		off_t positionNext = (i-1)*STRIDE;		
#else
	for(long i=0; i < iter; i++ ){
		off_t positionNext = (i+1)*STRIDE;		
#endif		
		off_t position = i*STRIDE;
		// seek to the right file position
		lseek(fh, position, SEEK_SET);

		usleep(TIME_THINK);

		int minRet = mincore( ((char*) file_mmaped) + position, BLOCK_LEN, mincore_vector );
		if ( minRet == 0){
	    	for (long pi = 0; pi <= BLOCK_LEN / page_size; pi++) {
	      	if (mincore_vector[pi] & 1) {
	          //  printf("%lu ", (unsigned long) pi);
	           pages_in_memory++;
	         }else{
	         	pages_not_in_memory++;
	         }
	      }
   	}else{
   		fprintf(stderr, "Error in mincore(): %s\n", strerror(errno));
   	}
   	uint64_t op_start_time = gettime();   	
		bytesWritten = read( fh, data, BLOCK_LEN );
		
#ifdef FADVISE2
#warning "Using fadvise during loop()"		
		// prefetch next I/O
		int pret = posix_fadvise(fh, positionNext, BLOCK_LEN, POSIX_FADV_WILLNEED);
		//int pret = readahead(fh, positionNext, BLOCK_LEN);
		if (pret != 0){
			fprintf(stderr, "Error in fadvise(): %s\n", strerror(pret));
		}
#endif

		totalIOtime += gettime() - op_start_time;
		if( bytesWritten < BLOCK_LEN ) {
			fprintf( stderr, "Error while reading from file\n" );
			exit( 1 );
		}
	}
	uint64_t delta = gettime() - start_time;
	double volume = iter * BLOCK_LEN / 1024.0 / 1024;
	printf("iter: %ld stride: %d blocksize: %d runtime: %fs iotime: %fs volume: %f MiB throughput: %f MiB/s; pages cached: %f MiB, pages not cached: %f MiB\n", iter, STRIDE, BLOCK_LEN, delta / 1000000000.0, totalIOtime / 1000000000.0, volume, volume / totalIOtime * 1000000000.0, pages_in_memory * page_size / 1024.0 / 1024.0, pages_not_in_memory * page_size / 1024.0 / 1024.0 );

	free(data);
   munmap(file_mmaped, SIZE_BYTE);
   free(mincore_vector);

	int ret = close( fh );
	if( ret != 0 ) {
		fprintf( stderr, "Error while closing file\n" );
	}
	return 0;
}
