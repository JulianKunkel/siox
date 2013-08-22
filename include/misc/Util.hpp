#ifndef UTIL_HPP
#define UTIL_HPP

#include <arpa/inet.h>
#include <sys/time.h>

namespace util {

	inline uint64_t htonll( const uint64_t n )
	{
		uint64_t retval;

		retval = n;

		retval = ( ( uint64_t ) htonl( n & 0xFFFFFFFF ) ) << 32;
		retval |= htonl( ( n & 0xFFFFFFFF00000000 ) >> 32 );

		return retval;

	};


	inline uint64_t time64()
	{
		struct timeval tv;
		gettimeofday( &tv, NULL );
		return 1000000 * tv.tv_sec + tv.tv_usec;
	}


	inline uint32_t time32()
	{
		struct timeval tv;
		gettimeofday( &tv, NULL );
		return tv.tv_sec;
	}

};

#endif
