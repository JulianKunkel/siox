/*
 * =====================================================================================
 *
 *       Filename:  CTreeMachineNodeSerialization.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/26/2015 02:45:48 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */


#ifndef  CTreeMachineNodeSerialization_INC
#define  CTreeMachineNodeSerialization_INC

#include <iostream>
#include <shogun/base/init.h>
#include <shogun/multiclass/tree/TreeMachineNode.h>
#include <shogun/multiclass/tree/BinaryTreeMachineNode.h>
#include <shogun/multiclass/tree/CARTreeNodeData.h>
#include "NodeProxy.hpp"

namespace io {
	std::ostream& operator<< (std::ostream& ofs, shogun::CTreeMachineNode<shogun::CARTreeNodeData>& node);
//	std::istream& operator>> (std::istream& ifs, shogun::CTreeMachineNode<shogun::CARTreeNodeData>& node);
	std::istream& operator>> (std::istream& ifs, io::NodeProxy& node);
	bool operator==(shogun::CTreeMachineNode<shogun::CARTreeNodeData>& node1, shogun::CTreeMachineNode<shogun::CARTreeNodeData>& node2);

}		/* -----  end of namespace io  ----- */
#endif   /* ----- #ifndef CTreeMachineNodeSerialization_INC  ----- */
