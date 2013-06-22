#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char const *argv[])
{

  const char *data = "Somebody had to put all of this confusion here!\n";
  size_t len = strlen(data) * sizeof(char);

  int fh = open("tmp_posix_test", O_CREAT|O_RDWR, S_IRUSR|S_IRGRP|S_IROTH|S_IWUSR);
  if (fh < 0)
  {
    fprintf(stderr, "Error while opening 'tmp_posix_test'\n");
    exit(1);
  }

  ssize_t bytesWritten = write(fh, (const void*) data , len);
  if (bytesWritten < 0)
  {
    fprintf(stderr, "Error while writing to 'tmp_posix_test'\n");
    exit(1);
  }

  bytesWritten = close(fh);
  if (bytesWritten != 0)
  {
    fprintf(stderr, "Error while closing file 'tmp_posix_test'\n");
  }

  return 0;
}