#include <hdf5.h>
#include "siox-datatypes-helper-hdf5.h"

static inline unsigned translateHDFOpenFlagsToSIOX( unsigned flags )
{
	return ( ( flags & H5F_ACC_RDWR ) > 0 ? SIOX_HDF5_ACCESS_RDWR : 0 ) | ( ( flags & H5F_ACC_TRUNC ) > 0 ? SIOX_HDF5_ACCESS_TRUNCATE : 0 )
	       | ( ( flags & H5F_ACC_RDONLY ) > 0 ? SIOX_HDF5_ACCESS_RDONLY : 0 ) | ( ( flags & H5F_ACC_EXCL ) > 0 ? SIOX_HDF5_ACCESS_EXCLUSIVE : 0 )
	       | ( ( flags & H5F_ACC_CREAT ) > 0 ? SIOX_HDF5_ACCESS_CREATE : 0 )  | ( ( flags & H5F_ACC_DEBUG ) > 0 ? SIOX_HDF5_ACCESS_DEBUG : 0 );
}