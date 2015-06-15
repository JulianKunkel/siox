/*
 * =====================================================================================
 *
 *       Filename:  CDynamicObjectArraySerialization.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/26/2015 12:54:12 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */



#include <cassert>
#include <shogun/base/init.h>
#include "CDynamicObjectArraySerialization.hpp"
#include "CSGObjectPtrSerialization.hpp"
namespace io {

	// const shogun::CDynamicObjectArray& array is not possible

	std::ostream& dynObjArray (std::ostream& ofs, shogun::CDynamicObjectArray*& array) {
//		std::cout << __PRETTY_FUNCTION__ << std::endl;
		using namespace shogun;
		const int32_t size = array->get_num_elements();
		ofs << size << " ";
		for (int32_t i = 0; i < size; ++i) { 
			ofs << array->element(i) << " "; // save address of element only
		}
		return ofs;
	}

	std::istream& dynObjArray (std::istream& ifs, shogun::CDynamicObjectArray*& array) {
//		std::cout << __PRETTY_FUNCTION__ << std::endl;
		using namespace shogun;
		int32_t size{0};
		ifs >> size;
//		array->reset_array();

		CSGObject** objects = new CSGObject*[size];
		
		for (int32_t i = 0; i < size; ++i) {
			void* ptr;
			CSGObject* objPtr{nullptr};
			ifs >> ptr;
			objPtr = static_cast<CSGObject*>(ptr);
			assert(objPtr != nullptr);
			objects[i] = objPtr;
		}
		
		array = new CDynamicObjectArray {objects, size};
		assert(array->get_num_elements() == size);
		return ifs;
	}

	bool operator==	(const shogun::CDynamicObjectArray& a1, const shogun::CDynamicObjectArray& a2) {
//		std::cout << __PRETTY_FUNCTION__ << std::endl;
		assert(a1.get_num_elements() == a2.get_num_elements());
		for (int32_t i = 0; i < a1.get_num_elements(); ++i) {
			if (a1.get_element(i) != a1.get_element(i)) {
				return false;
			}
		}
		return true;
	}

}		/* -----  end of namespace io  ----- */
