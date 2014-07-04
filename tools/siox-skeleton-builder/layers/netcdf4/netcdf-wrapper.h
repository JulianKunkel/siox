#include "netcdf-helper.h"

//@component "NetCDF" "4.1"

//@autoInitializeLibrary

//@register_attribute componentVersion "Global" "descriptor/componentVersion" SIOX_STORAGE_STRING
//@register_attribute fileName "NetCDF" "descriptor/filename" SIOX_STORAGE_STRING
//@register_attribute fileHandle "NetCDF" "descriptor/filehandle" SIOX_STORAGE_32_BIT_INTEGER
//@register_attribute fileOpenFlags "NetCDF" "hints/openFlags" SIOX_STORAGE_32_BIT_UINTEGER
//@register_attribute fileBufferHint "NetCDF" "hints/buffersize" SIOX_STORAGE_64_BIT_UINTEGER
//@register_attribute fileInitialSizeHint "NetCDF" "hints/initalSize" SIOX_STORAGE_64_BIT_UINTEGER
//@component_attribute componentVersion implVersion ''const char * implVersion = nc_inq_libvers();''

//@horizontal_map_create_int

//////////////////////////////////////////////////////////////////////////////////////////

// HDF5 API prefixes:
// P: Property
// O: Object
// F: File
// I: Identifier
// T: Type
// L: Link
// G: Group
// E: Error
// D: Dataset
// A: Attribute

//@activity
//@activity_attribute_pointer fileName filename
//@splice_before uint32_t translatedFlags = translateNetCDFOpenFlagsToSIOX(mode);
//@activity_attribute fileOpenFlags translatedFlags
//@activity_attribute fileBufferHint *bufrsizehintp
//@activity_attribute fileHandle *ncidp
//@horizontal_map_put_int *ncidp
//@error ''ret!=NC_NOERR'' ret
int nc__open(const char *filename, int mode, size_t *bufrsizehintp, int *ncidp);

//@activity
//@activity_attribute_pointer fileName filename
//@splice_before uint32_t translatedFlags = translateNetCDFOpenFlagsToSIOX(mode);
//@activity_attribute fileOpenFlags translatedFlags
//@activity_attribute fileHandle *ncidp
//@horizontal_map_put_int *ncidp
//@error ''ret!=NC_NOERR'' ret
int nc_open(const char *filename, int mode, int *ncidp);

//@activity
//@activity_attribute_pointer fileName filename
//@splice_before uint32_t translatedFlags = translateNetCDFOpenFlagsToSIOX(cmode);
//@activity_attribute fileOpenFlags translatedFlags
//@activity_attribute fileHandle *ncidp
//@horizontal_map_put_int *ncidp
//@error ''ret!=NC_NOERR'' ret
int nc_create(const char *filename, int cmode, int *ncidp);


//@activity
//@activity_attribute_pointer fileName filename
//@splice_before uint32_t translatedFlags = translateNetCDFOpenFlagsToSIOX(cmode);
//@activity_attribute fileOpenFlags translatedFlags
//@activity_attribute fileBufferHint *bufrsizehintp
//@activity_attribute fileInitialSizeHint initialsz
//@activity_attribute fileHandle *ncidp
//@horizontal_map_put_int *ncidp
//@error ''ret!=NC_NOERR'' ret
int nc__create(const char *filename, int cmode, size_t initialsz, size_t *bufrsizehintp, int *ncidp);


//@activity
//@activity_attribute fileHandle ncid
//@activity_link_int ncid
//@horizontal_map_remove_int ncid
//@error ''ret!=NC_NOERR'' ret
int nc_close(int ncid);


//@activity_link_int dataset_id MapName=activityHashTable_dtypeMaps
//@horizontal_map_remove_int dataset_id MapName=activityHashTable_dtypeMaps
