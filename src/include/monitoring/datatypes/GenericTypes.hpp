#ifndef __SIOX_GENERICTYPES_HPP
#define __SIOX_GENERICTYPES_HPP

#include <C/siox-types.h>

typedef siox_timestamp Timestamp;

inline Timestamp convert_seconds_to_timestamp(int s){
	return ((Timestamp) s)*1000llu*1000*1000;
}

#endif
