/*
 * =====================================================================================
 *
 *       Filename:  SGVectorSerialization.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/25/2015 11:27:44 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */


#ifndef  SGVectorSerialization_INC
#define  SGVectorSerialization_INC

#include "BasicTypesSerialization.hpp"
#include <iostream>
#include <shogun/lib/SGVector.h>

namespace io {
	std::ostream& operator<< (std::ostream& ofs, const shogun::SGVector<float64_t>& vector);
	std::istream& operator>> (std::istream& ifs, shogun::SGVector<float64_t>& vector);
	bool operator==	(const shogun::SGVector<float64_t>& v1, const shogun::SGVector<float64_t>& v2);
}		/* -----  end of namespace io  ----- */
#endif   /* ----- #ifndef SGVectorSerialization_INC  ----- */
