#ifndef UTIL_HPP
#define UTIL_HPP

#include <iostream>
#include <arpa/inet.h>
#include <sys/time.h>

namespace util {

	inline uint64_t htonll(const uint64_t n)
	{
		uint64_t retval;

		retval = n;

		retval = ((uint64_t) htonl(n & 0xFFFFFFFF)) << 32;
		retval |= htonl((n & 0xFFFFFFFF00000000) >> 32);

		return retval;

	};

	
	inline uint64_t ntohll(const uint64_t n) {
		
		union { uint32_t ln[2]; uint64_t lln; } u;
		
		u.lln = n;
		
		return ((uint64_t) ntohl(u.ln[0]) << 32) | (uint64_t) ntohl(u.ln[1]);
	}

	inline uint64_t time64()
	{
		struct timeval tv;
		gettimeofday(&tv, NULL);
		return 1000000 * tv.tv_sec + tv.tv_usec;
	}


	inline uint32_t time32()
	{
		struct timeval tv;
		gettimeofday(&tv, NULL);
		return tv.tv_sec;
	}

	inline void fail(const std::string &msg)
	{
		std::cerr << msg << std::endl;
		exit(1);
	}

};

#endif