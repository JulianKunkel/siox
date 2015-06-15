#include <stdio.h>

int main( int argc, char ** argv )
{
	FILE * pFile;
	char buffer[] = {'a', 'b'};
	pFile = fopen( "testfile.bin", "wb" );
	fwrite( buffer , sizeof( char ), 2, pFile );
	int c = fgetc( pFile );
	fputs( "Test", pFile );
	fprintf( pFile, "%s %d\n", "TESTSTR", 47 );
	fclose( pFile );
	return 0;
}
