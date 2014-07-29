/*
 * File:   siox_time.h
 * Author: mickler, kunkel
 *
 * Created on July 15, 2013, 2:18 PM
 */

#ifndef TIME_H
#define TIME_H

#include "C/siox-types.h"

#ifdef  __cplusplus
extern "C" {
#endif

siox_timestamp siox_gettime( void );
double siox_time_in_s( siox_timestamp time );
void siox_time_to_str( siox_timestamp time, char buff[40], int convertYear );

#ifdef  __cplusplus
}
#endif

#endif  /* TIME_H */

