/*
 * =====================================================================================
 *
 *       Filename:  CARTreeNodeDataSerialization.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/25/2015 10:05:05 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include <string>
#include "SGVectorSerialization.hpp"
#include "CARTreeNodeDataSerialization.hpp"

//int32_t 	attribute_id
//SGVector< float64_t > 	transit_into_values
//float64_t 	node_label
//float64_t 	total_weight
//float64_t 	weight_minus_node
//float64_t 	weight_minus_branch
//int32_t 	num_leaves
namespace io {

	std::ostream& operator<< (std::ostream& ofs, const shogun::CARTreeNodeData& data) {
//		std::cout << __PRETTY_FUNCTION__ << std::endl;
		ofs << "weight_minus_node "   << data.weight_minus_node    << " ";
		ofs << "weight_minus_branch " << data.weight_minus_branch  << " ";
		ofs << "num_leaves "          << data.num_leaves           << " ";
		ofs << "transit_into_values " << data.transit_into_values;
		ofs << "node_label "          << data.node_label           << " ";
		ofs << "attribute_id "        << data.attribute_id         << " ";
		ofs << "total_weight "        << data.total_weight;
		return ofs;
	}

	std::istream& operator>> (std::istream& ifs, shogun::CARTreeNodeData& data) {
//		std::cout << __PRETTY_FUNCTION__ << std::endl;
		std::string tag{};
		ifs >> tag >> data.weight_minus_node;
		ifs >> tag >> data.weight_minus_branch;
		ifs >> tag >> data.num_leaves;
		ifs >> tag >> data.transit_into_values;
		ifs >> tag >> data.node_label;
		ifs >> tag >> data.attribute_id;
		ifs >> tag >> data.total_weight;
		return ifs;
	}

	bool operator== (const shogun::CARTreeNodeData& nd1, const shogun::CARTreeNodeData& nd2) {
//		std::cout << __PRETTY_FUNCTION__ << std::endl;
		return 
			nd1.weight_minus_node   == nd2.weight_minus_node && 
			nd1.weight_minus_branch == nd2.weight_minus_branch && 
			nd1.num_leaves          == nd2.num_leaves && 
			nd1.transit_into_values == nd2.transit_into_values && 
			nd1.node_label          == nd2.node_label && 
			nd1.attribute_id        == nd2.attribute_id && 
			nd1.total_weight        == nd2.total_weight;
	}
}		/* -----  end of namespace io  ----- */
