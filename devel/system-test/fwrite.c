#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

int main( int argc, char ** argv )
{
	FILE * pFile;
	char buffer[] = {'a', 'b'};
	pFile = fopen( "testfile.bin", "wb" );
	fwrite( buffer , sizeof( char ), 2, pFile );
	int c = fgetc( pFile );
	fputs( "Test", pFile );
	fprintf( pFile, "%s %d %c\n", "TESTSTR", 47, c );
	fclose( pFile );

	struct stat statRes;
        stat("testfile.bin", & statRes);


	return 0;
}
