#include <hdf5.h>

// splice_before printf("SIOX: Call H5Fcreate.\n");
// splice_after printf("SIOX: H5Fcreate terminated.\n");
hid_t H5Fcreate(const char *filename, unsigned flags, hid_t create_plist, hid_t access_plist);
