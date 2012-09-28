/*
 * See: http://www.jedsoft.org/slang/doc/html/slangfun-14.html
 * (This is not C, but S-lang:
 *  http://en.wikipedia.org/wiki/S-Lang_(programming_language)
 *  As some function signatures vary from the actual C definition,
 *  always double check with the proper "man 3 <function>" <:) )
 */

#include <stdio.h>

//interfaceName "POSIX STDIO"

//register_datatype "filePath" SIOX_STORAGE_STRING filePath_t
//register_datatype "stdio FDP" SIOX_STORAGE_64_BIT_INTEGER filep_t
//register_datatype "posix FD" SIOX_STORAGE_32_BIT_INTEGER pfd_t

//register_metric "Bytes read" "Bytes read" SIOX_UNIT_BYTES SIOX_STORAGE_64_BIT_INTEGER SIOX_SCOPE_SAMPLE mid_br
//register_metric "Bytes written" "Bytes written" SIOX_UNIT_BYTES SIOX_STORAGE_64_BIT_INTEGER SIOX_SCOPE_SAMPLE mid_bw

/* TODO: Check for correct integer sizes */

/* TODO: DIE DATATYPE-TYPNAMEN MÜSSEN ALS C-TYPEN DEFINIERT WERDEN!!! */


/*
 * Open and Close, FDs and FDPs
 */

//activity "fopen"
//receive_descriptor filePath_t path
//map_descriptor filePath_t path filep_t ret
FILE *fopen(const char *path, const char *mode);

//activity "fclose"
//receive_descriptor filep_t fp
//release_descriptor filep_t fp
int fclose(FILE *fp);
/* close a stream */

//activity "freopen"
//receive_descriptor filep_t stream
/*Zwei Deskriptoren empfangen oder ein Tupel?*/
//release_descriptor filep_t stream
//map_descriptor filePath_t path filep_t ret
/*An Stelle von ret könnte auch stream verwendet werden.*/
FILE *freopen(const char *path, const char *mode, FILE *stream);
/* stream open functions */

//activity "fdopen"
//receive_descriptor pfd_t fd
//map_descriptor pfd_t fd filep_t ret
FILE *fdopen(int fd, const char *mode);
/* stream open functions */

//activity "fileno"
//receive_descriptor filep_t stream
//map_descriptor filep_t stream pfd_t ret
int fileno(FILE *stream);
/* return the integer descriptor of the argument stream */



/*
 * Read and Write
 */
/* TODO: Include file position handling? */


/* Binary */

//activity "fread"
//receive_descriptor filep_t stream
//activity_report local_aid pfd_t stream mid_br (size*nmemb) ""
/* TODO: splice: ftell(). */
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
/* binary stream input/output */

//activity "fwrite"
//receive_descriptor filep_t stream
//activity_report local_aid pfd_t stream mid_bw (size*nmemb) ""
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
/* binary stream input/output */


/* By Char or Word */

//activity "getc"
//receive_descriptor filep_t stream
//activity_report local_aid pfd_t stream mid_br (ret==EOF?0:1) ""
int getc(FILE *stream);
/* get next character or word from input stream */
/* getc() is equivalent to fgetc() except that it may be implemented as a
/* macro which evaluates  stream  more than once. */

//activity "fgetc"
//receive_descriptor filep_t stream
//activity_report local_aid pfd_t stream mid_br (ret==EOF?0:1) ""
int fgetc(FILE *stream);
/* get next character or word from input stream */
/* fgetc()  reads the next character from stream and returns it as an unsigned
/* char cast to an int, or EOF on end of file or error. */

//activity "getchar"
//receive_descriptor filep_t stdin
//activity_report local_aid pfd_t stdin mid_br (ret==EOF?0:1) ""
int getchar(void);
/* get next character or word from input stream */
/* getchar() is equivalent to getc(stdin). */

//activity "getw"
//receive_descriptor filep_t stream
//activity_report local_aid pfd_t stream mid_br (ferror()!=0?0:1) ""
/* TODO: ...:1) oder ...:4)  für "word"? */
int getw(FILE *stream);
/* get next character or word from input stream */
/* getw()  reads a word (that is, an int) from stream.  It's provided for
/* compatibility with SVr4.  We recommend you use fread(3) instead. */

//activity "ungetc"
//receive_descriptor filep_t stream
int ungetc(int c, FILE *stream);
/* un-get character from input stream */
/* ungetc() pushes c back to stream, cast to unsigned char, where it is
/* available for subsequent read  operations.  Pushed-back characters will be
/* returned in reverse order; only one pushback is guaranteed. */

//activity "putc"
//receive_descriptor filep_t stream
//activity_report local_aid pfd_t stream mid_bw (ret==EOF?0:1) ""
int putc(int c, FILE *stream);
/* output a character or word to a stream */
/* putc()  is  equivalent to fputc() except that it may be implemented as a
/* macro which evaluates stream more than once. */

//activity "fputc"
//receive_descriptor filep_t stream
//activity_report local_aid pfd_t stream mid_bw (ret==EOF?0:1) ""
int fputc(int c, FILE *stream);
/* output a character or word to a stream */
/* fputc() writes the character c, cast to an unsigned char, to stream. */

//activity "putchar"
//receive_descriptor filep_t stream
//activity_report local_aid pfd_t stdout mid_bw (ret==EOF?0:1) ""
int putchar(int c);
/* output a character or word to a stream */
/* putchar(c); is equivalent to putc(c,stdout). */

//activity "putw"
//receive_descriptor filep_t stream
//activity_report local_aid pfd_t stream mid_bw (ferror()!=0?0:1) ""
/* TODO: ...:1) oder ...:4)  für "word"? */
int putw(int w, FILE *stream);
/* output a character or word to a stream */


/* By Line */

//activity "gets"
//receive_descriptor filep_t stdin
//activity_report local_aid pfd_t stdin mid_br strlen(s) ""
char *gets(char *s);
/* get a line from a stream */
/* gets()  reads a line from stdin into the buffer pointed to by s until either
/* a terminating newline or EOF, which it replaces with a null byte ('\0').  No
/* check for buffer overrun is performed (see BUGS below).*/
/* DEPRECATED - BUGGY!!! */

//activity "fgets"
//receive_descriptor filep_t stream
//activity_report local_aid pfd_t stream mid_br strlen(s) ""
char *fgets(char *s, int size, FILE *stream);
/* get a line from a stream */
/* fgets() reads in at most one less than size characters from stream and
/* stores them into the buffer pointed to by s.  Reading stops after an EOF or
/* a newline.  If a newline is read, it is stored into the buffer.  A
/* terminating null byte ('\0') is stored after the last character in the
/* buffer. */

//activity "puts"
//receive_descriptor filep_t stream
//activity_report local_aid pfd_t stdout mid_bw (strlen(s)+1) ""
int puts(const char *s);
/* output a line to a stream */
/* puts() writes the string s and a trailing newline to stdout. */
/* TODO: _Stimmt_ das mit dem "trailling newline"?! */


//activity "fputs"
//receive_descriptor filep_t stream
//activity_report local_aid pfd_t stream mid_bw strlen(s) ""
int fputs(const char *s, FILE *stream);
/* output a line to a stream */
/* fputs() writes the string s to stream, without its terminating null byte ('\0'). */


/* Formatted */

//activity "scanf"
/* input format conversion */

//activity "fscanf"
/* input format conversion */

//activity "sscanf"
/* input format conversion */

//activity "printf"
//receive_descriptor filep_t stdout
/* geht das? */
/* TODO: splice: Berechnung der Datenmenge über strlen(fmt) und size der arglist-elemente*/
//activity_report local_aid pfd_t stream mid_br total_size ""
int printf (const char * fmt, ...);
/* formatted output conversion */

//activity "fprintf"
/* formatted output conversion */

//activity "sprintf"
/* formatted output conversion */

//activity "vfprintf"
/* formatted output conversion */

//activity "vfscanf"
/* input format conversion */

//activity "vprintf"
/* formatted output conversion */

//activity "vscanf"
/* input format conversion */

//activity "vsprintf"
/* formatted output conversion */

//activity "vsscanf"
/* input format conversion */



/*
 * Buffers
 */


/* Define Buffers */
/* TODO: register_attribute ?! Aber für eine andere Node?! */

//activity "setbuf"
/* stream buffering operations */

//activity "setbuffer"
/* stream buffering operations */

//activity "setlinebuf"
/* stream buffering operations */

//activity "setvbuf"
/* stream buffering operations */


/* Flush Buffers */

//activity "fflush"
//receive_descriptor filep_t fp
/* TODO: splice: Buffer-Füllstand für Report ermitteln? */
int fflush (FILE fp);
/* flush a stream */

//activity "fpurge"
/* flush a stream */



/*
 * Stream Position
 */

/* Set */

//activity "fseek"
/* reposition a stream */

//activity "fsetpos"
/* reposition a stream */

//activity "rewind"
/* reposition a stream */


/* Read */

//activity "ftell"
/* reposition a stream */

//activity "fgetpos"
/* reposition a stream */




/*
 * Temp Files
 */

//activity "tmpfile"
//create_descriptor filep_t ret
/* TODO: _Wann_ den Descriptor anlegen? In Init? In After? Und wie nach oben weitergeben? */
FILE *tmpfile(void);
/* temporary file routines */
/* The  tmpfile()  function  opens a unique temporary file in binary
/* read/write (w+b) mode.  The file will be automatically deleted when it is
/* closed or the program terminates. */


/*
 * Other Directory Functions
 */

//activity "remove"
//release_descriptor filePath_t pathname
int remove(const char *pathname);
/* remove directory entry */
/* remove() deletes a name from the file system.  It calls unlink(2) for
/* files, and rmdir(2) for directories. If  the  removed name was the last
/* link to a file and no processes have the file open, the file is deleted and
/* the space it was using is made available for reuse. If the name was the
/* last link to a file, but any processes still have the file open, the file
/* will  remain in existence until the last file descriptor referring to it is
/* closed. If the name referred to a symbolic link, the link is removed. If
/* the  name  referred  to  a  socket, FIFO, or device, the name is removed,
/* but processes which have the object open may continue to use it. */