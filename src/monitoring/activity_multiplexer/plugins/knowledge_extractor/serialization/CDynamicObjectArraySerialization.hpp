/*
 * =====================================================================================
 *
 *       Filename:  CDynamicObjectArraySerialization.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/26/2015 12:59:00 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */


#ifndef  CDynamicObjectArraySerialization_INC
#define  CDynamicObjectArraySerialization_INC

#include <iostream>
#include <shogun/lib/DynamicObjectArray.h>

namespace io {
	std::ostream& dynObjArray (std::ostream& ofs, shogun::CDynamicObjectArray*& array);
	std::istream& dynObjArray (std::istream& ifs, shogun::CDynamicObjectArray*& array);
	bool operator==	(const shogun::CDynamicObjectArray& a1, const shogun::CDynamicObjectArray& a2);
}		/* -----  end of namespace io  ----- */
#endif   /* ----- #ifndef CDynamicObjectArraySerialization_INC  ----- */
