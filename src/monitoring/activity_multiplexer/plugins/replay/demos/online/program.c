#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	const char * file = "test.tmp";
	char buf[4] = "zzz\n";
	ssize_t bytes_read = -1;
	int i = 0;

	// write to non existing file
	int fd = open(	file,
					O_RDWR | O_CREAT,
					S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP | S_IROTH
				);

	// everything ok? write and close
	if ( fd != -1 )
	{
		// forgive that there is no error checking
		write(fd, "abcdef", 7);
		lseek(fd, 1, SEEK_SET);
		read(fd, buf, 3);
		close(fd);
	}


	printf("Result: %s\n", buf);
	

	return 0;
}
