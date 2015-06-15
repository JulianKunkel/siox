/*
 * =====================================================================================
 *
 *       Filename:  NodeProxy.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/30/2015 02:06:55 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */



#include "NodeProxy.hpp"
#include "CTreeMachineNodeSerialization.hpp"

namespace io {
	std::ostream& operator<< (std::ostream& os, NodeProxy& proxy) {
//		std::cout << __PRETTY_FUNCTION__ << std::endl;
		
		os << "fakeParent " << proxy.getParent() << " ";
		os << "fakeChildren " << proxy.getChildren().size() << " ";

		for (size_t i = 0; i < proxy.getChildren().size(); ++i) {
			os << proxy.getChildren()[i] << " ";
		}

		os << ": " << *proxy.getNode();
		return os;
	}
}		/* -----  end of namespace io  ----- */
