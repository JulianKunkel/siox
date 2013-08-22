#include <netcdf.h>
#include "siox-datatypes-helper-netcdf.h"

static inline unsigned translateNetCDFOpenFlagsToSIOX( unsigned flags )
{
	 return ( ( flags & NC_WRITE ) > 0 ? SIOX_NC_WRITE : 0 ) 
	 		| ( ( flags & NC_SHARE ) > 0 ? SIOX_NC_SHARE : 0 )
	 		| ( ( flags & NC_NOCLOBBER ) > 0 ? SIOX_NC_NOCLOBBER : 0 )
	 		| ( ( flags & NC_64BIT_OFFSET ) > 0 ? SIOX_NC_64BIT_OFFSET : 0 )
	 		| ( ( flags & NC_NETCDF4 ) > 0 ? SIOX_NC_NETCDF4 : 0 )
	 		| ( ( flags & NC_CLASSIC_MODEL ) > 0 ? SIOX_NC_CLASSIC_MODEL : 0 );
}

// , NC_SHARE, , , . 