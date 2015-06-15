/*
 * =====================================================================================
 *
 *       Filename:  TreeSerialization.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/26/2015 02:08:56 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef  TreeSerialization_INC
#define  TreeSerialization_INC

#include <iostream>
#include "CTreeMachineNodeSerialization.hpp"
#include <shogun/multiclass/tree/BinaryTreeMachineNode.h>

namespace io {
	void saveTree(std::ostream& os, shogun::CTreeMachineNode<shogun::CARTreeNodeData>& node);
	shogun::CTreeMachineNode<shogun::CARTreeNodeData>* loadTree(std::istream& is);
	void printTree(shogun::CTreeMachineNode<shogun::CARTreeNodeData>* root);
}		/* -----  end of namespace io  ----- */
#endif   /* ----- #ifndef TreeSerialization_INC  ----- */
