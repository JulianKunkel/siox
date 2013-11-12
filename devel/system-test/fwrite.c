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
	return 0;
}
