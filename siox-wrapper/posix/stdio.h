/**
 * See: http://www.jedsoft.org/slang/doc/html/slangfun-14.html
 */

#include <stdio.h>

//interfaceName "POSIX STDIO"

//register_datatype "filePath" SIOX_TYPE_STRING filePath_t
//register_datatype "stdio FDP" SIOX_TYPE_POINTER filep_t
//register_datatype "posix FD" SIOX_TYPE_INT pfd_t

//activity "fopen"
//receive_descriptor filePath_t path
//map_descriptor filePath_t path filep_t ret
FILE *fopen(const char *path, const char *mode);

//activity "fclose"
//receive_descriptor filep_t fp
//release_descriptor filep_t fp
int fclose(FILE *fp);

//activity "fdopen"
//reveive_descriptor pfd_t fd
//map_descriptor pfd_t fd filep_t ret
FILE *fdopen(int fd, const char *mode);

//activity "freopen"
//receive_descriptor filep_t stream
/*Zwei descriptoren empfangen oder ein Tupel?*/
//release_descriptor filep_t stream
//map_descriptor filePath_t path filep_t ret
/*An Stelle von ret könnte auch stream verwendet werden.*/
FILE *freopen(const char *path, const char *mode, FILE *stream);

//activity "fread"
//receive_descriptor filep_t stream
//report_activity size_t (size*nmemb)
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);

//activity "fwrite"
//receive_descriptor filep_t stream
//report_activity size_t (size*nmemb)
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);

