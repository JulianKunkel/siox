#ifndef UTIL_HPP
#define UTIL_HPP

#include <arpa/inet.h>

namespace util {

inline uint64_t htonull(const uint64_t n)
{
	uint64_t retval;
	
	retval = n;

	retval = ((uint64_t) htonl(n & 0xFFFFFFFF)) << 32;
	retval |= htonl((n & 0xFFFFFFFF00000000) >> 32);

	return retval;
	
};


inline uint32_t htonul(const uint32_t n)
{
	uint32_t retval;
	
	retval = n;

	retval = ((uint32_t) htonl(n & 0xFFFF)) << 16;
	retval |= htonl((n & 0xFFFF0000) >> 16);

	return retval;

}

};

#endif
