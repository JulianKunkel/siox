#include <mpi.h>

#include <stdio.h>

int main(int argc, char ** argv){
	printf("Run this app with \"siox-inst posix,mpi ./a.out\" two times!!!\n");
	printf("Then the required dat files should have been created.\n");
	MPI_Init(& argc, & argv );
	MPI_Finalize();

	return 0;
}

