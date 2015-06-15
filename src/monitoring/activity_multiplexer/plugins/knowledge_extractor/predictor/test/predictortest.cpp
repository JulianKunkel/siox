/*
 * =====================================================================================
 *
 *       Filename:  predictor.cpp
 *
 *    Description: 
 *
 *        Version:  1.0
 *        Created:  03/06/2015 09:05:59 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */


#include <iostream>
#include <cassert>
//#include "src/predictor/NeuralNetwork.hpp"
#include "../CART.hpp"
#include <tuple>
#include <random>

namespace test {

	using namespace std;
	using Feature = std::vector<double>;
	using Label = double;
	using Features = std::vector<Feature>;
	using Labels = std::vector<Label>;
	using Predictor = ml::alg::CART<Feature, Label>;

	/**
	 * @brief Convert
	 */
	void testConversion() {
		using namespace ml::core;
		cout << __PRETTY_FUNCTION__ << endl;
		std::vector<Predictor::Features> features{{1, 2, 3, 4}, {2, 3, 4, 3}, {2, 1, 4, 2}, {5.8, 2.7, 5.1, 1.9}, {2, 3.4, 5, 1.22}};
		Predictor::init();

		cout << "std::vector<std::vector<double>>:" << endl;
		cout << features << endl;
		shogun::SGMatrix<float64_t> sgmatrix = convert(features);
		cout << "SGMatrix<float64_t>:" << endl;
		cout << sgmatrix << endl;
		std::vector<Predictor::Features> convert_test_features = convert<double>(sgmatrix);
		cout << "std::vector<std::vector<double>>:" << endl;
		cout << convert_test_features << endl;
		shogun::SGMatrix<float64_t> convert_test_sgmatrix = convert(features);
		cout << "SGMatrix<float64_t>:" << endl;
		cout << convert_test_sgmatrix << endl;
	}

	/**
	 * @brief Open
	 */
	void testOpenFile() {
		cout << __PRETTY_FUNCTION__ << endl;
		//
		//	cout << "open file" << endl;
		//	predictor.open("/scratch/pool/CIS/m215026/git/predictor2/data/iris/iris.data");
		////	predictor.open("/scratch/pool/CIS/m215026/git/siox_ml_plugin/data/input.csv");
		//	predictor.open("/scratch/pool/CIS/m215026/git/siox_ml_plugin/data/input_clean_data_fill.csv");
		//	predictor.train();
		//

		//	std::vector<Predictor::Labels> labels{1, 2, 3, 4, 1};
		//	assert(features.size() == labels.size());
		//	predictor.train(features, labels);
		//
		////	std::vector<FPredictor::Features> features{{1, 2, 3, 4}, {2, 3, 4, 3}, {2, 1, 4, 2}, {5.8, 2.7, 5.1, 1.9}};
		//
		//	cout << "test_main" << endl;
		//
		//	std::vector<Predictor::Labels> predictions = predictor.predict(features);
		//	cout << "predicted value: ";
		//	for (size_t i = 0; i < predictions.size(); ++i) {
		//		cout << predictions[i] << " : " ;
		//	}
		//	cout << endl;
		//	predictor.evaluate();

		//	cout << labels << endl;
	}



	/**
	 * @brief Empty
	 */
	void testEmptyModel() {
		using namespace ml::core;
		cout << __PRETTY_FUNCTION__ << endl;

		std::vector<Predictor::Features> features{{1, 2, 3, 4}, {2, 3, 4, 3}, {2, 1, 4, 2}, {5.8, 2.7, 5.1, 1.9}, {2, 3.4, 5, 1.22}};
		std::vector<Predictor::Features> features1{{1, 2, 3, 4}};
		std::vector<Predictor::Labels> labels1{1};

		Predictor::init();
		Predictor predictor;	

		if (predictor.isReady()) {
			cout << "predictor is ready now" << endl;
			vector<Predictor::Labels> predictions = predictor.predict(features);
			cout << predictions << endl;
		}
		else {
			cout << "predictor is not ready" << endl;
		}

		// make predictor ready	
		predictor.train(features1, labels1);

		if (predictor.isReady()) {
			cout << "predictor is ready now" << endl;
			std::vector<Predictor::Labels> predictions = predictor.predict(features);
			cout << predictions << endl;
		}
		else {
			cout << "predictor is not ready" << endl;
		}
	}

	/**
	 * @brief Save model
	 */
	void testSaveModel() {
		cout << __PRETTY_FUNCTION__ << endl;
		std::vector<Predictor::Features> features{{1, 2, 3, 4}, {2, 3, 4, 3}, {2, 1, 4, 2}, {5.8, 2.7, 5.1, 1.9}, {2, 3.4, 5, 1.22}};
		std::vector<Predictor::Labels> labels{1, 2, 1, 4, 1};

		Predictor::init();
		Predictor predictor;	
		predictor.train(features, labels);
		//	predictor.open("/scratch/pool/CIS/m215026/git/predictor2/data/input_clean_data_fill.csv");
		//	predictor.train();
		//		predictor.save("model.dat");
	}

	/**
	 * @brief Save
	 */
	void testSaveTree() {
		cout << __PRETTY_FUNCTION__ << endl;
		std::vector<Predictor::Features> features{{1, 2, 3, 4}, {2, 3, 4, 3}, {2, 1, 4, 2}, {5.8, 2.7, 5.1, 1.9}, {2, 3.4, 5, 1.22}};
		std::vector<Predictor::Labels> labels{1, 2, 1, 4, 1};

		Predictor::init();
		Predictor predictor;	
//		predictor.train(features, labels);
			predictor.open("/scratch/pool/CIS/m215026/git/predictor2/data/input_clean_data_fill.csv");
			predictor.train();
		predictor.save("tree.dat");
	}

	/**
	 * @brief Load
	 */
	void testLoadTree() {
		cout << __PRETTY_FUNCTION__ << endl;

		Predictor::init();
		Predictor predictor{};	
		predictor.load("tree.dat");
		predictor.save("tree2.dat");
	}


	void testPrediction() {
		using namespace ml::core;
		cout << __PRETTY_FUNCTION__ << endl;
		std::vector<Predictor::Features> features{{1, 2, 3, 4}, {2, 3, 4, 3}, {2, 1, 4, 2}, {5.8, 2.7, 5.1, 1.9}, {2, 3.4, 5, 1.22}};
		std::vector<Predictor::Labels> labels{1, 2, 1, 4, 1};

		Predictor::init();
		Predictor predictor1;	
		predictor1.train(features, labels);
//			predictor1.open("/scratch/pool/CIS/m215026/git/predictor2/data/input_clean_data_fill.csv");
//			predictor1.train();
		predictor1.save("tree.dat");
		auto y = predictor1.predict(features);

		Predictor::init();
		Predictor predictor2{};	
		predictor2.init();
		predictor2.load("tree.dat");
		auto x = predictor2.predict(features);

		cout << "prediction original tree: " << x << endl;
		cout << "prediction loaded tree: " << y << endl;

		assert(x == y);
	}
}		/* -----  end of namespace test  ----- */

int main(int argc, char** argv) {
	using namespace test;
//		testConversion();
//		testEmptyModel();
//		testSaveModel();
//	testSaveTree();
	testLoadTree();
//	testPrediction();
	//	testPrintTree();

	return 0;
}
