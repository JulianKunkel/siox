/*
 * =====================================================================================
 *
 *       Filename:  function.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/08/2015 12:16:22 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "functions.hpp"

#include <cassert>
#include <fstream>

namespace ml {
	namespace core {
		void print_message(FILE* target, const char* str) {
			//	fprintf(target, "%s", str);
//			printf("%s", str);
		}

		void print_warning(FILE* target, const char* str) {
			//	fprintf(target, "%s", str);
//			printf("%s", str);
		}

		void print_error(FILE* target, const char* str) {
			//	fprintf(target, "%s", str);
			printf("%s", str);
		}

//		std::vector<double> convert(const shogun::SGVector<float64_t>& sg_vector) {
//			using namespace std;
//			using namespace shogun;
//
//			vector<double> v(sg_vector.size());
//
//			for (size_t i = 0; i < sg_vector.size(); ++i) {
//				v[i] = sg_vector[i];
//			}
//
//			return v;
//		}


		shogun::SGVector<float64_t> convert(const std::vector<double>& v) {
			using namespace std;
			using namespace shogun;

			SGVector<float64_t> sg_vector(v.size());

			for (size_t i = 0; i < v.size(); ++i) {
				sg_vector[i] = v[i];
			}
			return sg_vector;
		}


//		std::vector<std::vector<double>> convert(const shogun::SGMatrix<float64_t>& sg_matrix) {
//			using namespace std;
//			using namespace shogun;
//
//			CDenseFeatures<float64_t> features(sg_matrix);
//
//			index_t size = features.get_num_vectors();
//
//			vector<vector<double>> matrix;
//			for (index_t i = 0; i < size; ++i) {
//				SGVector<float64_t> sg_feature_vector = features.get_feature_vector(i);
//				std::vector<double> feature_vector = convert<double>(sg_feature_vector);
//				matrix.push_back(feature_vector);
//			}
//			return matrix;
//		}

		shogun::SGMatrix<float64_t> convert(const std::vector<std::vector<double>>& matrix) {
			using namespace shogun;

			const size_t rows = matrix.size(); // x-axis
			const size_t cols = matrix[0].size(); // y-axis

			assert(rows);
			assert(cols);

			SGMatrix<float64_t> empty_matrix(cols, rows);
			CDenseFeatures<float64_t> features(empty_matrix);
			features.set_num_features(cols);
			features.set_num_vectors(rows);

			for (size_t row = 0; row < rows; ++row) {
				SGVector<float64_t> sg_feature_vector = convert(matrix[row]);
				features.set_feature_vector(sg_feature_vector, row);
			}

			SGMatrix<float64_t> filled_matrix = features.get_feature_matrix();
			return filled_matrix;
		}


		/**
		 * @brief In-place matrix normalization
		 *
		 * @param matrix
		 */
		void normalize(shogun::SGMatrix<float64_t>& matrix) {
			using namespace shogun;
			using namespace std;

			const index_t nrows = matrix.num_rows;
			const index_t ncols = matrix.num_cols;

			vector<float64_t> max_values(nrows);

			// find max values
			for (index_t row = 0; row < nrows; ++row){
				SGVector<float64_t> sg_vector = matrix.get_row_vector(row);
				max_values[row]	= CMath::max(sg_vector.vector, sg_vector.vlen);
			}

			// normalize	
			for (index_t col = 0; col < ncols; ++col) {
				for (index_t row = 0; row < nrows; ++row){
					matrix.matrix[col * nrows + row] = matrix.matrix[col * nrows + row] / max_values[row];
				}
			}
		}

		std::ostream& operator<< (std::ostream& os, const shogun::SGMatrix<float64_t>& matrix) {
			using namespace shogun;

			CDenseFeatures<float64_t> features(matrix);

			index_t size = features.get_num_vectors();
			for (index_t i = 0; i < size; ++i) {
				os << features.get_feature_vector(i);
			}
			return os;
		}

		std::ostream& operator<< (std::ostream& os, const shogun::SGVector<float64_t>& vector) {
			using namespace shogun;

			for (int32_t i = 0; i < vector.size(); i++) {
				os << vector[i] << " ";
			}
			os << std::endl;
			return os;
		}


		std::ostream& operator<< (std::ostream& os, const std::vector<std::vector<double>>& matrix){
			for (const auto& features : matrix) {
				for (const auto feature : features) {
					os << feature << " : ";
				}
				os << std::endl;
			}
			return os;
		}


		std::ostream& operator<< (std::ostream& os, const std::vector<double>& vector) {
			for (const auto& feature : vector) {
				os << feature << " : ";
			}
			return os;
		}


			void exportDot(const std::string& filename, shogun::CTreeMachineNode<shogun::CARTreeNodeData>* root) {
				using namespace std;
				using namespace shogun;
				//	using namespace io;
				ofstream ofs{filename};
				ofs << "digraph G {" << endl;
//				CTreeMachineNode<CARTreeNodeData>* root = m_model->get_root();
				exportDot(ofs, root);
				ofs << "}" << endl;
			}

		std::ostream& exportDot(std::ostream& ofs, shogun::CTreeMachineNode<shogun::CARTreeNodeData>* node) {
			using namespace shogun;
			CDynamicObjectArray* children = node->get_children();
			const int32_t size = children->get_num_elements();
			ofs << "\"" << node << "\"" << "[" << "label=\"";
			const CARTreeNodeData& data = node->data;
			//		ofs << "weight_minus_node "   << std::fixed << data.weight_minus_node    << "\\n";
			//		ofs << "weight_minus_branch " << data.weight_minus_branch  << "\\n";
			//		ofs << "num_leaves "          << data.num_leaves           << "\\n";
			//		ofs << "transit_into_values " << data.transit_into_values;
			ofs << "node_label "          << std::fixed << data.node_label           << "\\n";
			ofs << "attribute_id "        << data.attribute_id         << "\\n";
			ofs << "total_weight "        << data.total_weight;
			ofs << "\"" << "]" << std::endl;
			for (int32_t i = 0; i < size; ++i) {
				CTreeMachineNode<CARTreeNodeData>* child = dynamic_cast<CTreeMachineNode<CARTreeNodeData>*>(children->element(i));
				ofs << "\t" << "\"" <<  node << "\"" << " -> " << "\"" << child << "\"" << std::endl;
				exportDot(ofs, child);
			}
			return ofs;
		}

	}		/* -----  end of namespace core  ----- */
}		/* -----  end of namespace ml  ----- */

