#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	const char * file = "test.tmp";
	FILE * stream;
	const char * buf = "abcdef";

	// write to non existing file
	stream = fopen(file, "w");

	// everything ok? write and close
	if ( !stream ) {
		perror("fopen");
	}

	if ( !fwrite(buf, sizeof(char), 3, stream) ) { perror("fwrite"); }
	if ( !fseek(stream, 6, SEEK_SET) ) { perror("fseek"); }
	if ( !fwrite(buf+3, sizeof(char), 3, stream) ) { perror("fwrite"); }

	//fwrite(buf+3, sizeof(char), 3, stream);
	if (fclose(stream)) {
		perror("fclose");		
	}

	//printf("%s written.\n", file);

	return 0;
}
