/*
 * =====================================================================================
 *
 *       Filename:  NodeProxy.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/30/2015 02:03:36 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include <shogun/multiclass/tree/TreeMachineNode.h>
#include <shogun/multiclass/tree/BinaryTreeMachineNode.h>
#include <shogun/multiclass/tree/CARTreeNodeData.h>



#ifndef  NodeProxy_INC
#define  NodeProxy_INC

#include <cassert>
#include <iostream>
#include <vector>
#include <unordered_map>


// This stores real nodes without parent and children.
// Parent and children are saved outside of real node.
// The are linked later, after all nodes exists in memory.
//
// Implements move-semantic
//
// Programmer is responsible for destroying object, after the object was fetched.
namespace io {
	class NodeProxy {
		public:
			NodeProxy() : m_parent{nullptr} {
				m_node = new shogun::CBinaryTreeMachineNode<shogun::CARTreeNodeData>{};
				m_node->parent(nullptr);
				SG_REF(m_node);
			}

			NodeProxy(NodeProxy&& proxy) {
				m_node = proxy.m_node;
				m_parent = proxy.m_parent;
				m_children = proxy.m_children;
				proxy.m_node = nullptr;
				proxy.m_parent = nullptr;
				proxy.m_children.clear();
			}

			~NodeProxy() {
//				SG_UNREF(m_node);
			}

			NodeProxy& operator= (NodeProxy&& proxy) {
				m_node = proxy.m_node;
				m_parent = proxy.m_parent;
				m_children = proxy.m_children;
				proxy.m_node = nullptr;
				proxy.m_parent = nullptr;
				proxy.m_children.clear();
				return *this;
			}


			/**
			 * @brief Fetch node from proxy container. Class is unusable, after node is fetch.
			 *
			 * @return node
			 */
			shogun::CBinaryTreeMachineNode<shogun::CARTreeNodeData>* fetchNode() {
				assert(m_node);
				shogun::CBinaryTreeMachineNode<shogun::CARTreeNodeData>* node = m_node;
				m_node = nullptr;
				m_parent = nullptr;
				m_children.clear();
				return node;
			}

			std::vector<void*> getChildren() const {
				return m_children;
			}
			void setChildren(const std::vector<void*>& children) {
				m_children = children;
			}
			void setParent(void* parent) {
				m_parent = parent;
			}
			void* getParent() const {
				return m_parent;
			}
			void setMachine(const int32_t machine) {
				assert(m_node);
				m_node->machine(machine);
			}
			void setData(const shogun::CARTreeNodeData& data) {
				assert(m_node);
				m_node->data = data;
			}

			shogun::CBinaryTreeMachineNode<shogun::CARTreeNodeData>* getNode() const {
				return m_node;
			}

			void replaceFakePointers(std::unordered_map<void*, NodeProxy>& proxyNodeTable) {
				assert(m_node);
				if (m_parent == nullptr) {
					m_node->parent(nullptr);
				}
				else {
					if (proxyNodeTable.find(m_parent) != proxyNodeTable.end()) {
						m_node->parent(proxyNodeTable[m_parent].m_node);
					}
					else {
						std::cerr << "could not find " << m_parent << " in the table" << std::endl;
						exit(1);
					}
				}
				for (const auto& child : m_children) {
					if (proxyNodeTable.find(child) != proxyNodeTable.end()) {
						m_node->add_child(proxyNodeTable[child].m_node);
					}
					else {
						std::cerr << "could not find " << m_parent << " in the table" << std::endl;
						exit(1);
					}
				}
			}

		private:
			// copy constructors
			NodeProxy(NodeProxy& proxy) = delete;
			NodeProxy(const NodeProxy& proxy) = delete;

			// move constructors
//			NodeProxy(NodeProxy&& proxy) = delete;
			NodeProxy(const NodeProxy&& proxy) = delete;

			// move assignments
//			NodeProxy& operator= (NodeProxy&& proxy) = delete;
			NodeProxy& operator= (const NodeProxy&& proxy) = delete;

			// copy assignments
			NodeProxy& operator= (NodeProxy& proxy) = delete;
			NodeProxy& operator= (const NodeProxy& proxy) = delete;

			shogun::CBinaryTreeMachineNode<shogun::CARTreeNodeData>* m_node;
			void* m_parent;
			std::vector<void*> m_children;
	};


	std::ostream& operator<< (std::ostream& os, NodeProxy& proxy);

}		/* -----  end of namespace io  ----- */
#endif   /* ----- #ifndef NodeProxy_INC  ----- */
