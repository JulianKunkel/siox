/*
 * =====================================================================================
 *
 *       Filename:  Debug.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/04/2017 02:34:45 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */


#ifndef  Debug_INC
#define  Debug_INC

#include <iostream>

#ifndef DEBUGFUNC
#define DEBUGFUNC std::cout << "CALL -- " <<  __PRETTY_FUNCTION__ << ":" << __LINE__ << std::endl;
#endif

#endif   /* ----- #ifndef Debug_INC  ----- */
