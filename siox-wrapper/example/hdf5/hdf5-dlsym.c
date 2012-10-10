#define _GNU_SOURCE
#include <dlfcn.h>

#include  <stdarg.h>
#include  <hdf5.h>


static void sioxFinal() __attribute__((destructor));
static void sioxInit() __attribute__((constructor));

static char* dllib = RTLD_NEXT;
static hid_t (*__real_H5Fcreate) (const char * filename, unsigned  flags, hid_t  create_plist, hid_t  access_plist);

static void sioxInit() {

__real_H5Fcreate = (hid_t (*) (const char * filename, unsigned  flags, hid_t  create_plist, hid_t  access_plist)) dlsym(dllib, (const char*) "H5Fcreate");
}

static void sioxFinal() {
}
hid_t H5Fcreate(const char * filename, unsigned  flags, hid_t  create_plist, hid_t  access_plist)
{
	hid_t ret;
	printf( "SIOX: Call H5Fcreate.\n");
	ret = (__real_H5Fcreate)(filename, flags, create_plist, access_plist);
	printf( "SIOX: H5Fcreate terminated.\n");
	return ret;
}

