/*
 * =====================================================================================
 *
 *       Filename:  SGVectorSerialization.cpp
 *
 *    Description: 
 *
 *        Version:  1.0
 *        Created:  04/25/2015 09:24:14 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include <cassert>
#include "SGVectorSerialization.hpp"

namespace io {

	std::ostream& operator<< (std::ostream& ofs, const shogun::SGVector<float64_t>& vector) {
//		std::cout << __PRETTY_FUNCTION__ << std::endl;
		const int32_t size = vector.size();
		ofs << size << " ";

		for (int32_t i = 0; i < size; ++i) {
			ofs << vector[i] << " ";
		}
		return ofs;
	}

	std::istream& operator>> (std::istream& ifs, shogun::SGVector<float64_t>& vector) {
//		std::cout << __PRETTY_FUNCTION__ << std::endl;
		int32_t size{0};
		ifs >> size;
		vector.resize_vector(size);
		for (int32_t i = 0; i < size; ++i) {
			ifs >> vector[i];
		}
		return ifs;
	}

	bool operator==	(const shogun::SGVector<float64_t>& v1, const shogun::SGVector<float64_t>& v2) {
//		std::cout << __PRETTY_FUNCTION__ << std::endl;
		assert(v1.size() == v2.size());
		for (int32_t i = 0; i < v1.size(); ++i) {
			if (v1[i] != v2[i]) {
				return false;
			}
		}
		return true;
	}

}		/* -----  end of namespace io  ----- */
