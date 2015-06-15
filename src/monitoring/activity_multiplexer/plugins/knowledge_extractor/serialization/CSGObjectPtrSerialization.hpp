/*
 * =====================================================================================
 *
 *       Filename:  CSGObjectPtrSerialization.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/26/2015 03:10:42 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */



#ifndef  CSGObjectPtrSerialization_INC
#define  CSGObjectPtrSerialization_INC

#include <iostream>
#include <cassert>
#include <shogun/base/SGObject.h>

namespace io {
	std::ostream& operator<< (std::ostream& ofs, const shogun::CSGObject*& nodePtr);
	std::istream& operator>> (std::istream& ifs, shogun::CSGObject*& nodePtr);
}		/* -----  end of namespace io  ----- */
#endif   /* ----- #ifndef CSGObjectPtrSerialization_INC  ----- */
