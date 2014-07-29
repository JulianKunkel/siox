#include <time.h>

#include "time.h"

siox_timestamp siox_gettime( void )
{
	struct timespec tp;
	if( clock_gettime( CLOCK_REALTIME, &tp ) != 0 ) {
		return ( siox_timestamp ) - 1;
	} else {
		return ( tp.tv_sec * 1000000000ull ) + tp.tv_nsec;
	}
}

double siox_time_in_s( siox_timestamp time ){
	return time / 1000000000.0;
}