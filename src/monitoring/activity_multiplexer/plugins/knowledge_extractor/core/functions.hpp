/*
 * =====================================================================================
 *
 *       Filename:  functions.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/08/2015 12:31:32 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */



#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

#include <vector>
#include <iostream>
#include <string>
#include <shogun/labels/MulticlassLabels.h>
#include <shogun/features/DenseFeatures.h>
		//#include <shogun/base/init.h>
#include <shogun/multiclass/tree/TreeMachineNode.h>
#include <shogun/multiclass/tree/CARTreeNodeData.h>

namespace ml {
	namespace core {
		void print_message(FILE* target, const char* str);
		void print_warning(FILE* target, const char* str);
		void print_error(FILE* target, const char* str);

		template <typename TargetT, typename SourceT>
		std::vector<TargetT> convert(const shogun::SGVector<SourceT>& sg_vector) {
			using namespace std;
			using namespace shogun;
			vector<TargetT> v(sg_vector.size());
			for (size_t i = 0; i < sg_vector.size(); ++i) {
				v[i] = sg_vector[i];
			}
			return v;
		}

//		std::vector<std::vector<double>> convert(const shogun::SGMatrix<float64_t>& matrix);

		template <typename TargetT, typename SourceT>
		std::vector<std::vector<TargetT>> convert(const shogun::SGMatrix<SourceT>& sg_matrix) {
			using namespace std;
			using namespace shogun;

			CDenseFeatures<SourceT> features(sg_matrix);

			index_t size = features.get_num_vectors();

			vector<vector<TargetT>> matrix;
			for (index_t i = 0; i < size; ++i) {
				SGVector<SourceT> sg_feature_vector = features.get_feature_vector(i);
				std::vector<TargetT> feature_vector = convert<SourceT>(sg_feature_vector);
				matrix.push_back(feature_vector);
			}
			return matrix;
		}

		//template <typename STDType>
		//shogun::SGMatrix<float64_t> convert(const std::vector<std::vector<STDType>>& matrix);
		//
		shogun::SGVector<float64_t> convert(const std::vector<double>& vector);
		shogun::SGMatrix<float64_t> convert(const std::vector<std::vector<double>>& matrix);

		void normalize(shogun::SGMatrix<float64_t>& matrix);

		std::ostream& operator<< (std::ostream& os, const shogun::SGMatrix<float64_t>& matrix);
		std::ostream& operator<< (std::ostream& os, const shogun::SGVector<float64_t>& vector);
		std::ostream& operator<< (std::ostream& os, const std::vector<std::vector<double>>& matrix);
		std::ostream& operator<< (std::ostream& os, const std::vector<double>& vector);
		void exportDot(const std::string& filename, shogun::CTreeMachineNode<shogun::CARTreeNodeData>* root);
		std::ostream& exportDot(std::ostream& ofs, shogun::CTreeMachineNode<shogun::CARTreeNodeData>* node);

	}		/* -----  end of namespace core  ----- */
}		/* -----  end of namespace ml  ----- */

#endif // FUNCTIONS_HPP
