/*
 * =====================================================================================
 *
 *       Filename:  CSGObjectPtrSerialization.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/26/2015 03:09:08 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "CSGObjectPtrSerialization.hpp"

namespace io {
	std::ostream& operator<< (std::ostream& ofs, const shogun::CSGObject*& nodePtr) {
		std::cout << __PRETTY_FUNCTION__ << std::endl;
		ofs << (void*) nodePtr << " ";
		return ofs;
	}

	std::istream& operator>> (std::istream& ifs, shogun::CSGObject*& nodePtr) {
		std::cout << __PRETTY_FUNCTION__ << std::endl;
		using namespace shogun;
		void* ptr = nullptr;
		ifs >> ptr;
		nodePtr = static_cast<CSGObject*>(ptr);
		assert(nodePtr == ptr);
		return ifs;
	}
}		/* -----  end of namespace io  ----- */
