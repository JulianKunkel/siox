#include <time.h>

#include <monitoring/helpers/siox_time.h>

siox_timestamp siox_gettime(void)
{
	struct timespec tp;
	if( clock_gettime(CLOCK_REALTIME, &tp) != 0 ) {
		return (siox_timestamp)-1;
	}
	else {
		return (tp.tv_sec * 1000000000ull) + tp.tv_nsec;
	}
}
