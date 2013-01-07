#define _GNU_SOURCE
#include <dlfcn.h>

#include  <stdlib.h>
#include  <siox-ll.h>
#include  <stdarg.h>
#include  <glib.h>
#include  <unistd.h>
#include  <sys/uio.h>
#include  <sys/types.h>
#include  <sys/stat.h>
#include  <dlfcn.h>
#include  <fcntl.h>
#include  <stdio.h>


static void sioxFinal() __attribute__((destructor));
static void sioxInit() __attribute__((constructor));
siox_unid global_unid;
                 char global_pid;
                 char hostname[1024];

siox_dtid dtid_fileName;
siox_dtid dtid_fileHandle;
siox_mid mid_bytesWritten;
siox_mid mid_bytesRead;

static char* dllib = RTLD_NEXT;
static int (*__real_open) (const char * pathname, int  flags, ... );
static int (*__real_creat) (const char * pathname, mode_t  mode);
static int (*__real_close) (int  fd);
static ssize_t (*__real_write) (int  fd, const void * buf, size_t  count);
static ssize_t (*__real_read) (int  fd, void * buf, size_t  count);
static ssize_t (*__real_pread) (int  fd, void * buf, size_t  count, off_t  offset);
static ssize_t (*__real_pwrite) (int  fd, const void * buf, size_t  count, off_t  offset);
static ssize_t (*__real_readv) (int  fd, const struct iovec * iov, int  iovcnt);
static ssize_t (*__real_writev) (int  fd, const struct iovec * iov, int  iovcnt);
static void (*__real_sync) (void );
static int (*__real_fsync) (int  fildes);
static int (*__real_fdatasync) (int  fildes);
static void * (*__real_mmap) (void * address, size_t  length, int  protect, int  flags, int  filedes, off_t  offset);
static void * (*__real_mremap) (void * address, size_t  length, size_t  new_length, int  flag);
static int (*__real_munmap) (void * addr, size_t  length);
static int (*__real_madvise) (void * addr, size_t  length, int  advice);
static int (*__real_posix_fadvise) (int  fd, __off_t  offset, __off_t  len, int  advise);
static int (*__real_dup_fd) (int  fd);

static void sioxInit() {

	hostname[1023] = '\0';
			   gethostname(hostname, 1023);
               sprintf( &global_pid, "%d", getpid() );
               global_unid = siox_register_node(hostname, "POSIX" , &global_pid);
               siox_set_ontology("OntologyName");
               GHashTable *aidHashTable = g_hash_table_new(g_str_hash, g_str_equal);
	dtid_fileName = siox_register_datatype( "File Name", SIOX_STORAGE_STRING  );
	dtid_fileHandle = siox_register_datatype( "File Handle", SIOX_STORAGE_64_BIT_INTEGER  );
	mid_bytesWritten = siox_register_metric( "Bytes written.", SIOX_UNIT_BYTES, SIOX_STORAGE_64_BIT_INTEGER, SIOX_SCOPE_SUM  );
	mid_bytesRead = siox_register_metric( "Bytes read.", SIOX_UNIT_BYTES, SIOX_STORAGE_64_BIT_INTEGER, SIOX_SCOPE_SUM  );
__real_open = (int (*) (const char * pathname, int  flags, ... )) dlsym(dllib, (const char*) "open");
__real_creat = (int (*) (const char * pathname, mode_t  mode)) dlsym(dllib, (const char*) "creat");
__real_close = (int (*) (int  fd)) dlsym(dllib, (const char*) "close");
__real_write = (ssize_t (*) (int  fd, const void * buf, size_t  count)) dlsym(dllib, (const char*) "write");
__real_read = (ssize_t (*) (int  fd, void * buf, size_t  count)) dlsym(dllib, (const char*) "read");
__real_pread = (ssize_t (*) (int  fd, void * buf, size_t  count, off_t  offset)) dlsym(dllib, (const char*) "pread");
__real_pwrite = (ssize_t (*) (int  fd, const void * buf, size_t  count, off_t  offset)) dlsym(dllib, (const char*) "pwrite");
__real_readv = (ssize_t (*) (int  fd, const struct iovec * iov, int  iovcnt)) dlsym(dllib, (const char*) "readv");
__real_writev = (ssize_t (*) (int  fd, const struct iovec * iov, int  iovcnt)) dlsym(dllib, (const char*) "writev");
__real_sync = (void (*) (void )) dlsym(dllib, (const char*) "sync");
__real_fsync = (int (*) (int  fildes)) dlsym(dllib, (const char*) "fsync");
__real_fdatasync = (int (*) (int  fildes)) dlsym(dllib, (const char*) "fdatasync");
__real_mmap = (void * (*) (void * address, size_t  length, int  protect, int  flags, int  filedes, off_t  offset)) dlsym(dllib, (const char*) "mmap");
__real_mremap = (void * (*) (void * address, size_t  length, size_t  new_length, int  flag)) dlsym(dllib, (const char*) "mremap");
__real_munmap = (int (*) (void * addr, size_t  length)) dlsym(dllib, (const char*) "munmap");
__real_madvise = (int (*) (void * addr, size_t  length, int  advice)) dlsym(dllib, (const char*) "madvise");
__real_posix_fadvise = (int (*) (int  fd, __off_t  offset, __off_t  len, int  advise)) dlsym(dllib, (const char*) "posix_fadvise");
__real_dup_fd = (int (*) (int  fd)) dlsym(dllib, (const char*) "dup_fd");
}

static void sioxFinal() {
	siox_unregister_node(global_unid);
}
int open(const char * pathname, int  flags, ... )
{
	va_list args;
	va_start(args, flags);
	int ret;
	siox_aid *aID_open = (siox_aid*) malloc(sizeof(siox_aid));
    			 *aID_open = siox_start_activity( global_unid, NULL, "Open File."  );
	ret = (__real_open)(pathname, flags, args);
	siox_stop_activity( *aID_open, NULL );
	siox_end_activity( *aID_open, NULL );
	va_end(args);
	return ret;
}

int creat(const char * pathname, mode_t  mode)
{
	int ret;
	ret = (__real_creat)(pathname, mode);
	return ret;
}

int close(int  fd)
{
	int ret;
	siox_aid *aID_close = (siox_aid*) malloc(sizeof(siox_aid));
    			 *aID_close = siox_start_activity( global_unid, NULL, "Close File."  );
	ret = (__real_close)(fd);
	siox_stop_activity( *aID_close, NULL );
	siox_end_activity( *aID_close, NULL );
	return ret;
}

ssize_t write(int  fd, const void * buf, size_t  count)
{
	ssize_t ret;
	siox_aid *aID_write = (siox_aid*) malloc(sizeof(siox_aid));
    			 *aID_write = siox_start_activity( global_unid, NULL, "Write to file."  );
	ret = (__real_write)(fd, buf, count);
	siox_stop_activity( *aID_write, NULL );
	siox_report( global_unid, mid_bytesWritten, (void *) &count  );
	siox_end_activity( *aID_write, NULL );
	return ret;
}

ssize_t read(int  fd, void * buf, size_t  count)
{
	ssize_t ret;
	siox_aid *aID_read = (siox_aid*) malloc(sizeof(siox_aid));
    			 *aID_read = siox_start_activity( global_unid, NULL, "Read from file."  );
	ret = (__real_read)(fd, buf, count);
	siox_stop_activity( *aID_read, NULL );
	siox_report( global_unid, mid_bytesRead, (void *) &count  );
	siox_end_activity( *aID_read, NULL );
	return ret;
}

ssize_t pread(int  fd, void * buf, size_t  count, off_t  offset)
{
	ssize_t ret;
	ret = (__real_pread)(fd, buf, count, offset);
	return ret;
}

ssize_t pwrite(int  fd, const void * buf, size_t  count, off_t  offset)
{
	ssize_t ret;
	ret = (__real_pwrite)(fd, buf, count, offset);
	return ret;
}

ssize_t readv(int  fd, const struct iovec * iov, int  iovcnt)
{
	ssize_t ret;
	ret = (__real_readv)(fd, iov, iovcnt);
	return ret;
}

ssize_t writev(int  fd, const struct iovec * iov, int  iovcnt)
{
	ssize_t ret;
	ret = (__real_writev)(fd, iov, iovcnt);
	return ret;
}

void sync(void )
{
	(__real_sync)();

}

int fsync(int  fildes)
{
	int ret;
	ret = (__real_fsync)(fildes);
	return ret;
}

int fdatasync(int  fildes)
{
	int ret;
	ret = (__real_fdatasync)(fildes);
	return ret;
}

void * mmap(void * address, size_t  length, int  protect, int  flags, int  filedes, off_t  offset)
{
	void * ret;
	ret = (__real_mmap)(address, length, protect, flags, filedes, offset);
	return ret;
}

void * mremap(void * address, size_t  length, size_t  new_length, int  flag)
{
	void * ret;
	ret = (__real_mremap)(address, length, new_length, flag);
	return ret;
}

int munmap(void * addr, size_t  length)
{
	int ret;
	ret = (__real_munmap)(addr, length);
	return ret;
}

int madvise(void * addr, size_t  length, int  advice)
{
	int ret;
	ret = (__real_madvise)(addr, length, advice);
	return ret;
}

int posix_fadvise(int  fd, __off_t  offset, __off_t  len, int  advise)
{
	int ret;
	ret = (__real_posix_fadvise)(fd, offset, len, advise);
	return ret;
}

int dup_fd(int  fd)
{
	int ret;
	ret = (__real_dup_fd)(fd);
	return ret;
}

