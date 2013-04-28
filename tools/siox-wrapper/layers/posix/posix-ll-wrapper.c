#include  <stdlib.h>
#include  <stdio.h>
#include  <siox-ll.h>
#include  <stdarg.h>
#include  <glib.h>
#include  <unistd.h>
#include  <sys/uio.h>
#include  <sys/types.h>
#include  <sys/stat.h>
#include  <dlfcn.h>
#include  <fcntl.h>
#include  <errno.h>


siox_component * global_component;
				 siox_ontology * global_ontology;
                 char global_pid;
                 char hostname[1024];
				
siox_attribute * bytesToRead;
siox_attribute * bytesToWrite;
siox_attribute * fileName;
siox_attribute * fileHandle;
siox_metric * bytesWritten;
siox_metric * bytesRead;
GHashTable * activityHashTable_int ;

static void sioxFinal() __attribute__((destructor));
static void sioxInit() __attribute__((constructor));
int __real_open(const char * pathname, int  flags, ... );
int __real_creat(const char * pathname, mode_t  mode);
int __real_close(int  fd);
ssize_t __real_write(int  fd, const void * buf, size_t  count);
ssize_t __real_read(int  fd, void * buf, size_t  count);
ssize_t __real_pread(int  fd, void * buf, size_t  count, off_t  offset);
ssize_t __real_pwrite(int  fd, const void * buf, size_t  count, off_t  offset);
ssize_t __real_readv(int  fd, const struct iovec * iov, int  iovcnt);
ssize_t __real_writev(int  fd, const struct iovec * iov, int  iovcnt);
void __real_sync(void );
int __real_fsync(int  fildes);
int __real_fdatasync(int  fildes);
void * __real_mmap(void * address, size_t  length, int  protect, int  flags, int  filedes, off_t  offset);
void * __real_mremap(void * address, size_t  length, size_t  new_length, int  flag);
int __real_munmap(void * addr, size_t  length);
int __real_madvise(void * addr, size_t  length, int  advice);
int __real_posix_fadvise(int  fd, __off_t  offset, __off_t  len, int  advise);

static void sioxInit() {
	hostname[1023] = '\0';
			   gethostname(hostname, 1023);
               sprintf( &global_pid, "%d", getpid() );
               global_component = siox_component_register(hostname, "POSIX" , &global_pid);
               global_ontology = siox_ontology_connect();
	bytesToRead = siox_attribute_register( "Bytes to read", SIOX_STORAGE_64_BIT_INTEGER  );
	bytesToWrite = siox_attribute_register( "Bytes to write", SIOX_STORAGE_64_BIT_INTEGER  );
	fileName = siox_attribute_register( "File Name", SIOX_STORAGE_STRING  );
			   siox_component_register_descriptor( global_component, fileName );
	fileHandle = siox_attribute_register( "POSIX File Handle", SIOX_STORAGE_64_BIT_INTEGER  );
			   siox_component_register_descriptor( global_component, fileHandle );
	bytesWritten = siox_ontology_register_metric( global_ontology, "/Throughput/Write", "/Data/Volume/Bytes", SIOX_STORAGE_64_BIT_INTEGER, SIOX_SCOPE_SUM  );
	bytesRead = siox_ontology_register_metric( global_ontology, "/Troughput/Read", "/Data/Volume/Bytes", SIOX_STORAGE_64_BIT_INTEGER, SIOX_SCOPE_SUM  );
	activityHashTable_int  = g_hash_table_new(g_direct_hash, g_direct_equal);
}
static void sioxFinal() {
	siox_component_unregister(global_component);
}
int __wrap_open(const char * pathname, int  flags, ... )
{
	va_list args;
	va_start(args, flags);
	int  val = va_arg(args, int );
	int ret;
	siox_activity * sioxActivity = siox_activity_start( global_component, NULL, __FUNCTION__ );
	siox_activity_set_attribute( sioxActivity , fileName, &pathname );
	ret = __real_open(pathname, flags, val);
	siox_activity_stop( sioxActivity, NULL  );
	g_hash_table_insert( activityHashTable_int, GINT_TO_POINTER(ret), sioxActivity  );
	if ( ret < 0 )
					siox_activity_report_error( sioxActivity , errno );
	siox_activity_end( sioxActivity );
	va_end(args);
	return ret;
}

int __wrap_creat(const char * pathname, mode_t  mode)
{
	int ret;
	siox_activity * sioxActivity = siox_activity_start( global_component, NULL, __FUNCTION__ );
	ret = __real_creat(pathname, mode);
	siox_activity_stop( sioxActivity, NULL  );
	g_hash_table_insert( activityHashTable_int, GINT_TO_POINTER(ret), sioxActivity  );
	if ( ret < 0 )
					siox_activity_report_error( sioxActivity , errno );
	siox_activity_end( sioxActivity );
	return ret;
}

int __wrap_close(int  fd)
{
	int ret;
	siox_activity * sioxActivity = siox_activity_start( global_component, NULL, __FUNCTION__ );
	ret = __real_close(fd);
	siox_activity_stop( sioxActivity, NULL  );
	g_hash_table_remove( activityHashTable_int, GINT_TO_POINTER(fd) );
	siox_activity_end( sioxActivity );
	return ret;
}

ssize_t __wrap_write(int  fd, const void * buf, size_t  count)
{
	ssize_t ret;
	siox_activity * sioxActivity = siox_activity_start( global_component, NULL, __FUNCTION__ );
	siox_activity_set_attribute( sioxActivity , bytesToWrite, &count );
	ret = __real_write(fd, buf, count);
	siox_activity_stop( sioxActivity, NULL  );
	siox_activity_report( sioxActivity , bytesWritten, (void *) &ret );
	siox_activity * Parent = (siox_activity*) g_hash_table_lookup( activityHashTable_int, GINT_TO_POINTER(fd) );
    			siox_activity_link_to_parent( sioxActivity , Parent );
	if ( ret < 0 )
					siox_activity_report_error( sioxActivity , errno );
	siox_activity_end( sioxActivity );
	return ret;
}

ssize_t __wrap_read(int  fd, void * buf, size_t  count)
{
	ssize_t ret;
	siox_activity * sioxActivity = siox_activity_start( global_component, NULL, __FUNCTION__ );
	siox_activity_set_attribute( sioxActivity , bytesToRead, &count );
	ret = __real_read(fd, buf, count);
	siox_activity_stop( sioxActivity, NULL  );
	siox_activity_report( sioxActivity , bytesRead, (void *) &ret );
	if ( ret < 0 )
					siox_activity_report_error( sioxActivity , errno );
	siox_activity_end( sioxActivity );
	return ret;
}

ssize_t __wrap_pread(int  fd, void * buf, size_t  count, off_t  offset)
{
	ssize_t ret;
	ret = __real_pread(fd, buf, count, offset);
	return ret;
}

ssize_t __wrap_pwrite(int  fd, const void * buf, size_t  count, off_t  offset)
{
	ssize_t ret;
	ret = __real_pwrite(fd, buf, count, offset);
	return ret;
}

ssize_t __wrap_readv(int  fd, const struct iovec * iov, int  iovcnt)
{
	ssize_t ret;
	ret = __real_readv(fd, iov, iovcnt);
	return ret;
}

ssize_t __wrap_writev(int  fd, const struct iovec * iov, int  iovcnt)
{
	ssize_t ret;
	ret = __real_writev(fd, iov, iovcnt);
	return ret;
}

void __wrap_sync(void )
{
	__real_sync();

}

int __wrap_fsync(int  fildes)
{
	int ret;
	ret = __real_fsync(fildes);
	return ret;
}

int __wrap_fdatasync(int  fildes)
{
	int ret;
	ret = __real_fdatasync(fildes);
	return ret;
}

void * __wrap_mmap(void * address, size_t  length, int  protect, int  flags, int  filedes, off_t  offset)
{
	void * ret;
	ret = __real_mmap(address, length, protect, flags, filedes, offset);
	return ret;
}

void * __wrap_mremap(void * address, size_t  length, size_t  new_length, int  flag)
{
	void * ret;
	ret = __real_mremap(address, length, new_length, flag);
	return ret;
}

int __wrap_munmap(void * addr, size_t  length)
{
	int ret;
	ret = __real_munmap(addr, length);
	return ret;
}

int __wrap_madvise(void * addr, size_t  length, int  advice)
{
	int ret;
	ret = __real_madvise(addr, length, advice);
	return ret;
}

int __wrap_posix_fadvise(int  fd, __off_t  offset, __off_t  len, int  advise)
{
	int ret;
	ret = __real_posix_fadvise(fd, offset, len, advise);
	return ret;
}

