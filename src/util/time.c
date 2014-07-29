#include <time.h>
#include <stdio.h>

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

void siox_time_to_str( siox_timestamp time, char buff[40], int convertYear ){
	time_t t = (time_t) ( time / 1000000000ll );

   struct tm time_struct;
   localtime_r(& t, & time_struct);
   int pos = 0;
   if (convertYear ){
		pos = strftime(buff, 39, "%F %R:%S", & time_struct);
	}else{
		pos = strftime(buff, 39, "%R:%S", & time_struct);
	}
	sprintf(buff + pos, ".%.9llu", time % 1000000000ll);

	buff[39] = 0;
}