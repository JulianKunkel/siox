#include <stdio.h>
#include <mpi.h>

int main ( int argc, char **argv)
{
	int rank;
	int loop1;
	int loop2;
	int signal;
	FILE * pFile;
	char filename[50];
	const char content[]="This is an apple.\n";

	MPI_Init( &argc, &argv);
	MPI_Comm_rank( MPI_COMM_WORLD, &rank);
	int b[4]={67,68,69,70};
	memset( filename, 0, 50);
	snprintf( filename, 49, "example%d.txt", rank);
	for( loop1=0; loop1<4; loop1++)
	{
		if( rank==1 )
		{
			MPI_Send(&signal, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
		}
		else if (rank==2)
		{
			MPI_Status stat;
			MPI_Recv(&signal, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &stat);
		}
		pFile = fopen ( filename , "w" );
		for( loop2=0; loop2<4; loop2++)
		{
			fwrite( content, sizeof(content), 1, pFile );
		}
		fwrite( (void*) b, sizeof(int), 4, pFile);
		fclose ( pFile );
	}
	MPI_Finalize();
}
