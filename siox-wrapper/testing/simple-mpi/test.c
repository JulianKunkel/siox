#include <stdio.h>
#include "mpi.h"

int main(int argc, char* argv[])
{
    int rank, size;
    MPI_File fh;
    MPI_Datatype etype, filetype;
    MPI_Offset disp=0;
    MPI_Status status;
    char buf;

    printf("==========MPI_Init()==========\n");
    MPI_Init(&argc, &argv);
    printf("==========MPI_Comm_rank()==========\n");
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    printf("==========MPI_Comm_size()==========\n");
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    printf("Hello, world, I am %d of %d.\n", 
           rank, size);

    etype = MPI_CHAR;
    printf("==========MPI_File_open()==========\n");
    MPI_File_open(MPI_COMM_WORLD, "mpi_wrapper_test_file", MPI_MODE_CREATE|MPI_MODE_RDWR, MPI_INFO_NULL, &fh);
    buf = 'a';
    printf("==========MPI_File_write()==========\n");
    MPI_File_write(fh, &buf, 1, etype, &status);
    printf("==========MPI_File_close()==========\n");
    MPI_File_close(fh);
    printf("==========MPI_Finalize()==========\n");
    MPI_Finalize();

    return 0;
}
