/*
 * =====================================================================================
 *
 *       Filename:  CARTreeNodeDataSerialization.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/25/2015 11:35:17 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */


#ifndef  CARTreeNodeDataSerialization_INC
#define  CARTreeNodeDataSerialization_INC

#include <iostream>

#include <shogun/lib/SGVector.h>
#include <shogun/io/SGIO.h>
#include <shogun/multiclass/tree/CARTreeNodeData.h>

namespace io {
	std::ostream& operator<< (std::ostream& ofs, const shogun::CARTreeNodeData& data);
	std::istream& operator>> (std::istream& ifs, shogun::CARTreeNodeData& data);
	bool operator== (const shogun::CARTreeNodeData& nd1, const shogun::CARTreeNodeData& nd2);
}		/* -----  end of namespace io  ----- */
#endif   /* ----- #ifndef CARTreeNodeDataSerialization_INC  ----- */
