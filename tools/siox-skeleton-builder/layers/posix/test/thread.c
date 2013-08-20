#include <pthread.h>
#include <assert.h>
#include <stdio.h>


#define NUM_THREADS     10

void * doIO(void *threadid)
{
   FILE * file = fopen("testfile", "w");
   fwrite("TEST", 4, 1, file);
   fclose(file);
   pthread_exit(NULL);
}

int main(){
   pthread_t threads[NUM_THREADS];
   for(int t=0; t<NUM_THREADS; t++){
      printf("Creating %d\n", t);
      assert(pthread_create(&threads[t], NULL, doIO, (void *)t) == 0);
   }
   for(int t=0; t<NUM_THREADS; t++){
	void *status;
	pthread_join(threads[t], &status);
   }  
   exit(0);	
}
