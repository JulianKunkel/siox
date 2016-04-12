#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	const char * file = "test.tmp";
	ssize_t bytes_read = -1;
	char buf[4] = "zzz\n";
	int i = 0;

	FILE * stream;

	// write to non existing file
	stream = fopen(file, "w+");

	// everything ok? write and close
	if ( !stream ) {
		perror("fopen");
	}

	if ( !fwrite("abcdef", sizeof(char), 6, stream) ) { perror("fwrite"); }
	if ( !fseek(stream, 1, SEEK_SET) ) { perror("fseek"); }
	if ( !fread(buf, sizeof(char), 3, stream) ) { perror("fwrite"); }

	if (fclose(stream)) {
		perror("fclose");		
	}

	printf("Result: %s\n", buf);
	

	return 0;
}
