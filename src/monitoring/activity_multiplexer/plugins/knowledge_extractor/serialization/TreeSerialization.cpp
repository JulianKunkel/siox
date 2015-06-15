/*
 * =====================================================================================
 *
 *       Filename:  TreeSerialization.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/26/2015 01:59:48 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include <cassert>
#include <string>
#include <sstream>
#include <unordered_map>

#include "TreeSerialization.hpp"


namespace io {
	void saveTree(std::ostream& os, shogun::CTreeMachineNode<shogun::CARTreeNodeData>& node) {
//		std::cout << __PRETTY_FUNCTION__ << std::endl;
		using namespace std;
		using namespace shogun;
//		std::cout << __PRETTY_FUNCTION__ << std::endl;
		os << "id " << &node << " ";
		os << node << endl;

		CDynamicObjectArray* children = node.get_children();
		
		const int32_t size = children->get_num_elements();
		for (int32_t i = 0; i < size; ++i) {
			CTreeMachineNode<CARTreeNodeData>* child = dynamic_cast<CTreeMachineNode<CARTreeNodeData>*>(children->element(i));
			assert(child != nullptr);
			saveTree(os, *child);
		}
	}



	shogun::CTreeMachineNode<shogun::CARTreeNodeData>* loadTree(std::istream& is) {
//		std::cout << __PRETTY_FUNCTION__ << std::endl;
		using namespace io;
		using namespace std;
		using namespace shogun;
		unordered_map<void*, NodeProxy> nodes;

		// find root node
		// Assume the first pointer in the file is the pointer to the old root node
		std::string tag{};
		void* fakeRoot;
		is >> tag >> fakeRoot;

		// Each line is a node
		// Iterate through the nodes
		void* id;
		is.seekg(0);
		while (is.good()) {
			string line;
			getline(is, line);
//			cout << line << endl;
			if (line.size() != 0) {
				istringstream iss(line);
				iss >> tag >> id;
				NodeProxy proxy;
				iss >> proxy;
				nodes[id] = std::move(proxy);
//				cout << nodes[id] << endl;
			}
		}

//		cout << endl;
//		cout << "Before: " << endl;
//		for (auto& node : nodes) {
//			cout << node.first << " : " << node.second << endl; 
//		}
//		cout << endl << endl;
//
//		cout << "Nodes map: " << endl;
//		for (auto& node : nodes) {
//			cout << node.first << " : " << node.second.getNode() << endl; 
//		}
//		cout << endl << endl;
//
		// Replace invalid pointer through new pointers
		for (auto& node : nodes) {
			node.second.replaceFakePointers(nodes);
		}
//
//		cout << "After: " << endl;
//		for (auto& node : nodes) {
//			cout << node.first << " : " << node.second << endl; 
//		}
//		cout << endl << endl;

		shogun::CTreeMachineNode<shogun::CARTreeNodeData>* root = nodes[fakeRoot].fetchNode();
		return root;
	}



	void printTree(shogun::CTreeMachineNode<shogun::CARTreeNodeData>* node) {
		using namespace std;
		using namespace shogun;
//		cout << __PRETTY_FUNCTION__ << endl;

		cout << node << " ";
		cout << *node << endl;
		CDynamicObjectArray* children = node->get_children();
		const int32_t size = children->get_num_elements();
		for (int32_t i = 0; i < size; ++i) {
			CSGObject* child = children->element(i);
			printTree(dynamic_cast<CTreeMachineNode<CARTreeNodeData>*>(child));
		}
	}

}		/* -----  end of namespace io  ----- */
