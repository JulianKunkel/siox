/**
 * See: http://www.jedsoft.org/slang/doc/html/slangfun-14.html
 */

#include <stdio.h>

FILE *fopen(const char *path, const char *mode);
int fclose(FILE *fp);       

FILE *fdopen(int fd, const char *mode);
FILE *freopen(const char *path, const char *mode, FILE *stream);


size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);

