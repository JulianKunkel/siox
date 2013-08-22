#include  <hdf5.h>



static void sioxFinal() __attribute__( ( destructor ) );
static void sioxInit() __attribute__( ( constructor ) );
hid_t __real_H5Fcreate( const char * filename, unsigned  flags, hid_t  create_plist, hid_t  access_plist );

static void sioxInit()
{
}
static void sioxFinal()
{
}
hid_t __wrap_H5Fcreate( const char * filename, unsigned  flags, hid_t  create_plist, hid_t  access_plist )
{
	hid_t ret;
	printf( "SIOX: Call H5Fcreate.\n" );
	ret = __real_H5Fcreate( filename, flags, create_plist, access_plist );
	printf( "SIOX: H5Fcreate terminated.\n" );
	return ret;
}

