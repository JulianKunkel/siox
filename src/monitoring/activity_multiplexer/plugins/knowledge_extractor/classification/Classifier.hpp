/*
 * =====================================================================================
 *
 *       Filename:  Hierachical.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/02/2015 07:36:16 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef  Hierachical_INC
#define  Hierachical_INC

#include <cassert>
#include <shogun/clustering/Hierarchical.h>
#include <shogun/clustering/KMeans.h>

#include <shogun/io/CSVFile.h>
#include <shogun/io/SerializableAsciiFile.h>
#include <shogun/base/init.h>
#include <shogun/mathematics/Math.h>

#include <shogun/features/DenseFeatures.h>
#include <shogun/labels/RegressionLabels.h>
#include <shogun/evaluation/MulticlassAccuracy.h>
#include <shogun/distance/ChiSquareDistance.h>
#include <shogun/distance/EuclideanDistance.h>
#include <shogun/labels/MulticlassLabels.h>

#include <shogun/multiclass/tree/CARTree.h>
#include <shogun/multiclass/tree/TreeMachine.h>
#include <shogun/multiclass/tree/TreeMachineNode.h>

#include <fstream>
#include "../core/functions.hpp"
#include "../serialization/TreeSerialization.hpp"

namespace ml {
	namespace alg {
		
		template <typename FT, typename LT>
			class Classifier {
				public:
				using F = FT; // Featuretype
				using L = LT; // Labeltype
				using Sample = std::pair<F, L>;

				static void init();
				Classifier();
				void open(const std::string& filename);
				void save(const std::string& filename);
			  void load(const std::string& filename);
				void setNumOfClasses(const unsigned int numOfClasses);
				int train();
				int train(const std::vector<FT> features, unsigned int numOfClasses);
				double evaluate();
				LT predict(const FT feature);
				std::vector<LT> predict(const std::vector<FT> featurecSet);
				void exportDot(const std::string& filename);

				std::vector<LT> getTrainLabels();
				std::vector<FT> getTrainFeatures();

				private:
				shogun::CDenseFeatures<float64_t>* m_train_dfeats = nullptr;
				shogun::CMulticlassLabels* m_train_dlabels = nullptr;
				shogun::CCARTree* m_model;
				unsigned int m_numOfClasses;
			}; /* -----  end of class Classifier  ----- */



		/**
		 * @brief 
		 *
		 * @tparam F
		 * @tparam L
		 *
		 * @return 
		 */
		template <typename F, typename L> 
			std::vector<L> Classifier<F, L>::getTrainLabels() {
				return ml::core::convert<L>(m_train_dlabels->get_labels());
			}



		/**
		 * @brief 
		 *
		 * @tparam F
		 * @tparam L
		 *
		 * @return 
		 */
		template <typename F, typename L> 
			std::vector<F> Classifier<F, L>::getTrainFeatures() {
				return ml::core::convert<double>(m_train_dfeats->get_feature_matrix());
			}



		/**
		 * @brief 
		 *
		 * @tparam F
		 * @tparam L
		 */
		template <typename F, typename L> 
			Classifier<F, L>::Classifier() {
				using namespace shogun;

				SGVector<bool> attr_types(4);
				for (index_t i = 0; i < attr_types.size(); ++i) {
					attr_types[i] = false;
				}

				m_model = new CCARTree(attr_types, PT_MULTICLASS, 10, false);
//				using namespace shogun;
//				init();
//				open(filename);
//				CDenseFeatures<float64_t>* train_dfeats = m_train_dfeats;
//				assert(train_dfeats);
//				SG_REF(train_dfeats);
//				CEuclideanDistance* distance = new CEuclideanDistance(train_dfeats, train_dfeats);
//				SG_REF(distance);
//				m_model = new CKMeans(3, distance);
//				SG_UNREF(distance);
//				SG_UNREF(train_dfeats);
			}


		template <typename F, typename L> 
			void Classifier<F, L>::init() {
				std::cout << __PRETTY_FUNCTION__ << std::endl;
				using namespace ml::core;
				shogun::init_shogun(&print_message, &print_warning, &print_error);
			}



		/**
		 * @brief 
		 *
		 * @tparam F
		 * @tparam L
		 * @param filename
		 */
		template <typename F, typename L> 
			void Classifier<F, L>::open(const std::string& filename) {
				using namespace shogun;
				using namespace std;

				CCSVFile file(filename.c_str());	
				file.set_delimiter(',');

				SGMatrix<float64_t>* samples = new SGMatrix<float64_t>();
				samples->load(&file);

				// separate features from labels
				SGMatrix<float64_t> train_feats(samples->num_rows - 1, samples->num_cols);
				SGVector<float64_t> train_labels(samples->num_cols);

				const int32_t nrows = samples->num_rows;
				const int32_t ncols = samples->num_cols;
				const int32_t n_tf_rows = train_feats.num_rows;

				for (int32_t ncol = 0; ncol < ncols ; ncol++) {
					for (int32_t nrow = 0; nrow < train_feats.num_rows ;  nrow++) {
						train_feats.matrix[ncol * n_tf_rows + nrow] = samples->matrix[ncol * nrows + nrow];
					}
				}

				for (int32_t i = 0; i < train_labels.size(); i++) {
					train_labels[i] = samples->matrix[i * samples->num_rows + samples->num_rows - 1];
				}

				// save feature and labels in member variables
				SG_UNREF(m_train_dfeats);
				m_train_dfeats = new CDenseFeatures<float64_t>(train_feats);
				SG_REF(m_train_dfeats);

				SG_UNREF(m_train_dlabels);
				m_train_dlabels = new CMulticlassLabels(train_labels);
				SG_REF(m_train_dlabels);
				samples->matrix = nullptr; // workaround: csv file deletes the matrix
				delete samples;
			}



		/**
		 * @brief 
		 *
		 * @tparam F
		 * @tparam L
		 * @param filename
		 */
		template <typename F, typename L> 
			void Classifier<F, L>::load(const std::string& filename){
				using namespace std;
				using namespace shogun;
				using namespace io;
				ifstream ifs{filename};
				CTreeMachineNode<CARTreeNodeData>* root{nullptr};

				if (ifs.is_open()) {
					root = io::loadTree(ifs);
					SG_REF(root);
					assert(root);
					m_model->set_root(root);
				}
			}


		/**
		 * @brief 
		 *
		 * @tparam F
		 * @tparam L
		 * @param filename
		 */
		template <typename F, typename L> 
			void Classifier<F, L>::save(const std::string& filename){
				using namespace std;
				using namespace shogun;
				using namespace io;
				ofstream ofs{filename};
				CTreeMachineNode<CARTreeNodeData>* root = m_model->get_root();
				saveTree(ofs, *root);
			}



		/**
		 * @brief 
		 *
		 * @tparam F
		 * @tparam L
		 * @param numOfClasses
		 */
		template <typename F, typename L> 
			void Classifier<F, L>::setNumOfClasses(const unsigned int numOfClasses) {
				m_numOfClasses = numOfClasses;
			}



		/**
		 * @brief Train
		 *
		 * @tparam F Featuretype
		 * @tparam L Labeltype
		 *
		 * @return TODO
		 */
		template <typename F, typename L> 
			int Classifier<F, L>::train() {
				using namespace std;

				// 1. create feature set and labels	with KMeans
					cout << __PRETTY_FUNCTION__ << endl;
					cout << "\tstart training of clustering algorithm" << endl;
					
					assert(m_train_dfeats->get_num_vectors() > 0);
					assert(m_train_dfeats->get_num_features() > 1);
					assert(m_numOfClasses > 0);

					shogun::CKMeans* clusterAlg;
					shogun::CEuclideanDistance* distance = new shogun::CEuclideanDistance(m_train_dfeats, m_train_dfeats);
					clusterAlg = new shogun::CKMeans(m_numOfClasses, distance);
					clusterAlg->train();
					m_train_dlabels = shogun::CLabelsFactory::to_multiclass(clusterAlg->apply());
//				for (index_t i = 0; i < m_train_dlabels->get_num_labels(); ++i) {
//					std::cout << m_train_dlabels->get_label(i) << std::endl;
//				}
					delete clusterAlg;
					cout << "\tfinish training of clustering algorithm" << endl;

				// 2. train decision tree
					cout << "\tstart training of classification algorithm" << endl;
					
					assert(m_train_dlabels->get_num_labels() > 0);
					assert(m_train_dlabels->get_num_labels() == m_train_dfeats->get_num_vectors());

					m_model->set_labels(m_train_dlabels);
					m_model->train(m_train_dfeats);
					cout << "\tfinish training of classification algorithm" << endl;
				return 0;
			}



		/**
		 * @brief Train classification machine
		 *
		 * @tparam F Featuretype
		 * @tparam L Labletype
		 * @param features training features
		 * @param numOfClasses number of classes
		 *
		 * @return TODO
		 */
		template <typename F, typename L> 
			int Classifier<F, L>::train(const std::vector<F> features, unsigned int numOfClasses) {
				m_numOfClasses = numOfClasses;
				SG_UNREF(m_train_dfeats);
				shogun::SGMatrix<float64_t> train_feats = ml::core::convert(features);
				m_train_dfeats = new shogun::CDenseFeatures<float64_t>(train_feats);
				SG_REF(m_train_dfeats);
				train();
				return 0;
			}



		/**
		 * @brief 
		 *
		 * @tparam F
		 * @tparam L
		 * @param feature
		 *
		 * @return 
		 */
		template <typename F, typename L> 
			L Classifier<F, L>::predict(const F feature) {
				std::vector<F> featureSet;
				featureSet.push_back(feature);
				std::vector<L> labelSet = predict(featureSet);
				assert(labelSet.size() == 1);
				return labelSet[0];
			}



		/**
		 * @brief Predict a class to each feature.
		 *
		 * @tparam F feature set type
		 * @tparam L label type
		 * @param featureSet a feature vector
		 *
		 * @return labels
		 */
		template <typename F, typename L> 
			std::vector<L> Classifier<F, L>::predict(const std::vector<F> featureSet) {
				assert(featureSet.size() > 0);
				shogun::SGMatrix<float64_t> feats = ml::core::convert(featureSet);
				shogun::CDenseFeatures<float64_t>* dfeats = new shogun::CDenseFeatures<float64_t>(feats);
				shogun::CMulticlassLabels* prediction = m_model->apply_multiclass(dfeats);
				shogun::SGVector<float64_t> l = prediction->get_labels();
				std::vector<L> labels = ml::core::convert<unsigned int>(l);
				return labels;
			}



		/**
		 * @brief 
		 *
		 * @tparam F
		 * @tparam L
		 *
		 * @return 
		 */
		template <typename F, typename L> 
			double Classifier<F, L>::evaluate() {
				return 0;
			}



		/**
		 * @brief 
		 *
		 * @tparam F
		 * @tparam L
		 * @param filename
		 */
		template <typename F, typename L> 
			void Classifier<F,L>::exportDot(const std::string& filename) {
				ml::core::exportDot(filename, m_model->get_root());
			}


	}		/* -----  end of namespace alg  ----- */
}		/* -----  end of namespace ml  ----- */
#endif   /* ----- #ifndef Hierachical_INC  ----- */
