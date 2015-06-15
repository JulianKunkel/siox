/*
 * =====================================================================================
 *
 *       Filename:  CART.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/04/2015 06:20:53 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CART_HPP
#define CART_HPP

#include <utility>
#include <vector>
#include <stdio.h>
#include <cassert>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <sstream>


#include <shogun/io/CSVFile.h>
#include <shogun/base/init.h>
#include <shogun/mathematics/Math.h>

#include <shogun/features/DenseFeatures.h>
#include <shogun/labels/RegressionLabels.h>
//#include <shogun/evaluation/MulticlassAccuracy.h>
#include <shogun/evaluation/MeanSquaredError.h>

#include <shogun/neuralnets/NeuralNetwork.h>
#include <shogun/neuralnets/NeuralLayers.h>
#include "PredictorInterface.hpp"
#include "../core/functions.hpp"
#include "../serialization/TreeSerialization.hpp"

#include <shogun/multiclass/tree/CARTree.h>
#include <shogun/multiclass/tree/TreeMachine.h>
#include <shogun/multiclass/tree/TreeMachineNode.h>



namespace ml {
	namespace alg {
		template <typename FT, typename LT> 
			class CART : public PredictorInterface<FT, LT>
		{
			public:
				using F = FT;
				using L = LT;
				using Sample = std::pair<F, L>;

				CART() {
					using namespace shogun;
					std::cout << __PRETTY_FUNCTION__ << std::endl;
					shogun::init_shogun(&ml::core::print_message, &ml::core::print_warning, &ml::core::print_error);
					//			SGVector<bool> attr_types(m_train_dfeats->get_num_features());
					SGVector<bool> attr_types(4);
					for (index_t i = 0; i < attr_types.size(); ++i) {
						attr_types[i] = false;
					}

					m_model = new CCARTree(attr_types, PT_REGRESSION, 10, false);
					SG_REF(m_model);
					m_model->io->set_loglevel(MSG_INFO);
					m_model->set_max_depth(6);
					//	m_model->set_cv_pruning();
					//			m_model->set_max_depth(3);
				}

				~CART() {
					SG_UNREF(m_train_dfeats);
					SG_UNREF(m_train_dlabels);
					SG_UNREF(m_model);
					//			exit_shogun();
				}

				static void init() {
					using namespace ml::core;
					shogun::init_shogun(&print_message, &print_warning, &print_error);
				}

				L predict(const F& features) override;
				std::vector<L> predict(const std::vector<F>& features) override;
				void save(const std::string& filename);
				void load(const std::string& filename);
				bool isReady() const; // TODO: Move declaration to interface
				void open(const std::string& filename) override;
				void printTree() {io::printTree(m_model->get_root());}
				void printTree(shogun::CTreeMachineNode<shogun::CARTreeNodeData>* node);
				int train() override;
				int train(const std::vector<F>&, const std::vector<L>&) override;
				double evaluate(const std::vector<double>& real, const std::vector<double>& predicted);
				void exportDot(const std::string& filename);
				std::ostream& exportDot(std::ostream& ofs, shogun::CTreeMachineNode<shogun::CARTreeNodeData>* node);

			private:
				shogun::CDenseFeatures<float64_t>* m_train_dfeats = nullptr;
				shogun::CRegressionLabels* m_train_dlabels = nullptr;
				shogun::CCARTree* m_model = nullptr;
				bool m_isReady = false;
		};



		template <typename F, typename L> 
			void CART<F, L>::save(const std::string& filename){
				using namespace std;
				using namespace shogun;
				using namespace io;
				ofstream ofs{filename};
				CTreeMachineNode<CARTreeNodeData>* root = m_model->get_root();
				saveTree(ofs, *root);
			}

		template <typename F, typename L> 
			void CART<F, L>::load(const std::string& filename){
				using namespace std;
				using namespace shogun;
				using namespace io;
				ifstream ifs{filename};
				//	CTreeMachineNode<CARTreeNodeData>* root{nullptr};
				//	CTreeMachineNode<CARTreeNodeData>* root{new CBinaryTreeMachineNode<CARTreeNodeData>{}};
				CTreeMachineNode<CARTreeNodeData>* root{nullptr};

				if (ifs.is_open()) {
					//		root = m_model->get_root();
					root = io::loadTree(ifs);
					SG_REF(root);
					assert(root);
					//		io::printTree(root);
					m_model->set_root(root);
					//		assert(m_model->get_root());
					m_isReady = true;
				}
			}

		template <typename F, typename L> 
			bool CART<F, L>::isReady() const {
				return m_isReady;
			}


		template <typename F, typename L> 
			void CART<F, L>::open(const std::string& filename) {
				using namespace shogun;
				using namespace std;

				CCSVFile* file = new CCSVFile(filename.c_str());	
				SGMatrix<float64_t> samples;
				samples.load(file);
				SG_UNREF(file);

				SGMatrix<float64_t> train_feats(samples.num_rows - 1, samples.num_cols);
				SGVector<float64_t> train_labels(samples.num_cols);

				const int32_t nrows = samples.num_rows;
				const int32_t ncols = samples.num_cols;
				const int32_t n_tf_rows = train_feats.num_rows;

				for (int32_t ncol = 0; ncol < ncols ; ncol++) {
					for (int32_t nrow = 0; nrow < train_feats.num_rows ;  nrow++) {
						train_feats.matrix[ncol * n_tf_rows + nrow] = samples.matrix[ncol * nrows + nrow];
					}
				}

				for (int32_t i = 0; i < train_labels.size(); i++) {
					train_labels[i] = samples.matrix[i * samples.num_rows + samples.num_rows - 1];
				}

				SG_UNREF(m_train_dfeats);
				m_train_dfeats = new CDenseFeatures<float64_t>(train_feats);
				SG_REF(m_train_dfeats);

				SG_UNREF(m_train_dlabels);
				m_train_dlabels = new CRegressionLabels(train_labels);
				SG_REF(m_train_dlabels);
			}


		template <typename F, typename L> 
			int CART<F, L>::train() {
				using namespace shogun;
				using namespace std;

				m_model->set_labels(m_train_dlabels);
				m_model->train(m_train_dfeats);
				m_isReady = true;
				return 0;
			}


		template <typename F, typename L> 
			int CART<F, L>::train(const std::vector<F>& features, const std::vector<L>& labels) {
				using namespace shogun;
				using namespace std;
				using namespace ml::core;

				SGMatrix<float64_t> f = convert(features);
				SGVector<float64_t> l = convert(labels);

				SG_UNREF(m_train_dfeats);
				m_train_dfeats = new CDenseFeatures<float64_t>(f);
				SG_REF(m_train_dfeats);
				SG_UNREF(m_train_dlabels);
				m_train_dlabels = new CRegressionLabels(l);
				SG_REF(m_train_dlabels);

				m_model->set_labels(m_train_dlabels);
				m_model->train(m_train_dfeats);

				m_isReady = true;
				return 0;
			}

		template <typename F, typename L> 
				double CART<F, L>::evaluate(const std::vector<double>& real, const std::vector<double>& predicted) {
				using namespace shogun;
				using namespace std;
				SGVector<float64_t> rVector = ml::core::convert(real);
				SGVector<float64_t> pVector = ml::core::convert(predicted);
				CRegressionLabels* r = new CRegressionLabels(rVector);
				CRegressionLabels* p = new CRegressionLabels(pVector);
				CMeanSquaredError* evaluator = new CMeanSquaredError();
//				CMulticlassAccuracy* evaluator = new CMulticlassAccuracy();
				const double accuracy = evaluator->evaluate(r, p);
				SG_UNREF(r);
				SG_UNREF(p);
				SG_UNREF(evaluator);
				return accuracy;
			}



		template <typename F, typename L> 
			typename CART<F, L>::L CART<F, L>::predict(const F& fv) {
				using namespace shogun;
				using namespace std;
				const index_t size = fv.size();

				SGMatrix<float64_t> sg_feature_matrix(size, 10);
				for (size_t i = 0; i < fv.size(); i++) {
					sg_feature_matrix.matrix[i] = fv[i];
				}

				CDenseFeatures<float64_t>* df = new CDenseFeatures<float64_t>(sg_feature_matrix);
				SG_REF(df);
				//	CMulticlassLabels* ml = m_network->apply_multiclass(df);
				CRegressionLabels* ml = m_model->apply_regression(df);
				SGVector<float64_t> values = ml->get_labels_copy();

				SG_UNREF(df);
				return values[0];
			}



		template <typename F, typename L> 
			std::vector<typename CART<F, L>::L> CART<F, L>::predict(const std::vector<F>& fv) {
				using namespace shogun;
				using namespace std;
				using namespace ml::core;

				SGMatrix<float64_t> sg_feature_matrix = convert(fv);

				CDenseFeatures<float64_t>* df = new CDenseFeatures<float64_t>(sg_feature_matrix);
				SG_REF(df);
				//	CMulticlassLabels* ml = m_network->apply_multiclass(df);
				assert(m_model);
				assert(m_model->get_root());
				CRegressionLabels* ml = m_model->apply_regression(df);
				//	SGVector<float64_t> values = ml->get_labels_copy();

				SG_UNREF(df);

				SGVector<float64_t> rlabels = ml->get_labels();	
				vector<L> x = ml::core::convert<double>(rlabels);
				return x;
			}

		template <typename F, typename L> 
			void CART<F,L>::exportDot(const std::string& filename) {
				ml::core::exportDot(filename, m_model->get_root());
			}

	}		/* -----  end of namespace alg  ----- */
}		/* -----  end of namespace ml  ----- */
#endif
