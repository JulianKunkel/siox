/*
 * =====================================================================================
 *
 *       Filename:  CTreeMachineNodeSerialization.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/26/2015 12:24:37 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "string"
#include "CSGObjectPtrSerialization.hpp"
#include "CTreeMachineNodeSerialization.hpp"
#include "CDynamicObjectArraySerialization.hpp"
#include "CARTreeNodeDataSerialization.hpp"
#include "NodeProxy.hpp"

namespace io {


//	CTreeMachineNode * 	m_parent
//	int32_t 	m_machine
//	CDynamicObjectArray * 	m_children
//	T 	data


	std::ostream& operator<< (std::ostream& ofs, shogun::CTreeMachineNode<shogun::CARTreeNodeData>& node) {
//		std::cout << __PRETTY_FUNCTION__ << std::endl;
		using namespace shogun;
		using namespace std;
		ofs << "parent ";
		ofs << node.parent() << " ";
		ofs << "machine ";
		ofs << node.machine() << " ";
		ofs << "children ";
		CDynamicObjectArray* objArray{node.get_children()};
		dynObjArray(ofs, objArray);
		ofs << "data ";
		ofs << node.data;
		return ofs;
	}

//	std::istream& operator>> (std::istream& ifs, shogun::CTreeMachineNode<shogun::CARTreeNodeData>& node) {
////		std::cout << __PRETTY_FUNCTION__ << std::endl;
//		using namespace shogun;
//		using namespace std;
////		assert(node.ref_count() != 0);
//
//		CSGObject* parent{nullptr};
//		int32_t machine{0};
//		CDynamicObjectArray* children{nullptr};
//		CARTreeNodeData data{};
//
//		string tag;
//
//		ifs >> tag >> parent;
//		ifs >> tag >> machine;
//		ifs >> tag;
//		dynObjArray(ifs, children);
//		assert(children != nullptr);
//		ifs >> tag >> data;
//
//		node.parent(static_cast<CTreeMachineNode<shogun::CARTreeNodeData>*>(parent));
//		node.machine(machine);
//		assert(node.machine() == machine);
////		node.set_children(children);
//		node.data = data;
//
//		return ifs;
//	}
//
//
	std::istream& operator>> (std::istream& ifs, io::NodeProxy& node) {
//		std::cout << __PRETTY_FUNCTION__ << std::endl;
		using namespace shogun;
		using namespace std;
//		assert(node.ref_count() != 0);

		void* parent;
		int32_t machine{0};
		std::vector<void*> children;
		CARTreeNodeData data{};

		string tag;

		ifs >> tag >> parent;
		ifs >> tag >> machine;
		int32_t size = 0;
		ifs >> tag >> size;
		void* ptr;
		for (int32_t i = 0; i < size; ++i) {
			ifs >> ptr;
			children.push_back(ptr);
		}
		ifs >> tag >> data;

		node.setParent(parent);
		node.setMachine(machine);
		node.setChildren(children);
		node.setData(data);

		return ifs;
	}

	bool operator==(shogun::CTreeMachineNode<shogun::CARTreeNodeData>& node1, shogun::CTreeMachineNode<shogun::CARTreeNodeData>& node2) {
//		std::cout << __PRETTY_FUNCTION__ << std::endl;
		return
			node1.parent() == node2.parent() &&
			node1.machine() == node2.machine() &&
			*node1.get_children() == *node2.get_children() &&
			node1.data == node2.data;
	}
}		/* -----  end of namespace io  ----- */
