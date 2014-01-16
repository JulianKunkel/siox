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

// gcc -Wall -g ./fadvise-readAhead.c -std=gnu99 -lrt -o fadvise-readAhead

// total file size 1 KiB
#define SIZE_MIB 1*1024ul*1024
#define SIZE_BYTE SIZE_MIB*1024

// 20 KiB stride which is a multiple of page size
#define STRIDE 20480

// block length
#define BLOCK_LEN 1024

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
	// disable read-ahead
	int pret = posix_fadvise(fh, 0, 0, POSIX_FADV_RANDOM | POSIX_FADV_NOREUSE);
	if (pret != 0){
		fprintf(stderr, "Error in fadvise POSIX_FADV_RANDOM: %s\n", strerror(pret));
	}
	}
#endif

	uint64_t start_time = gettime();
	for(long i=iter-1; i >= 0; i-- ){
		off_t position = i*STRIDE;
		// seek to the right file position
		lseek(fh, position, SEEK_SET);

#ifdef FADVISE2
#warning "Using fadvise during loop()"
		// prefetch next I/O
		int pret = posix_fadvise(fh, (i-1)*STRIDE, BLOCK_LEN, POSIX_FADV_NORMAL | POSIX_FADV_WILLNEED);
		if (pret != 0){
			fprintf(stderr, "Error in fadvise(): %s\n", strerror(pret));
		}
#endif

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

		bytesWritten = read( fh, data, BLOCK_LEN );
		if( bytesWritten < BLOCK_LEN ) {
			fprintf( stderr, "Error while reading from file\n" );
			exit( 1 );
		}
	}
	uint64_t delta = gettime() - start_time;
	double volume = iter * BLOCK_LEN / 1024.0 / 1024;
	printf("iter: %ld stride: %d blocksize: %d in %fs volume: %f MiB throughput: %f MiB/s; pages cached: %f MiB, pages not cached: %f MiB\n", iter, STRIDE, BLOCK_LEN, delta / 1000000000.0, volume, volume / delta * 1000000000.0, pages_in_memory * page_size / 1024.0 / 1024.0, pages_not_in_memory * page_size / 1024.0 / 1024.0 );

	free(data);
   munmap(file_mmaped, SIZE_BYTE);
   free(mincore_vector);

	int ret = close( fh );
	if( ret != 0 ) {
		fprintf( stderr, "Error while closing file\n" );
	}
	return 0;
}
