#include <iostream>

#include <shogun/io/CSVFile.h>
//#include <shogun/base/init.h>
//#include <shogun/mathematics/Math.h>

#include "../Classifier.hpp"

using namespace std;
using Feature = std::vector<double>;
using Label = unsigned int;
using Features = std::vector<Feature>;
using Labels = std::vector<Label>;
using Classifier = ml::alg::Classifier<Feature, Label>;

std::string g_filename{"/scratch/pool/CIS/m215026/git/predictor2/data/iris/iris.data"};


/*
 * load data
 * train
 * predict (evaluate unsupervised learning)
 * evaluate internal learning algorithm
 * serialization
 */

void testLoadData() {
	std::cout << __PRETTY_FUNCTION__ << std::endl;
	Classifier model{};
	model.open(g_filename);
	Features features = model.getTrainFeatures();
	Labels labels = model.getTrainLabels();
	assert(features.size() == 150);
	assert(labels.size() == 150);
	for(size_t i = 0; i < 150; ++i) {
		for(size_t j = 0; j < features[0].size(); ++j) {
			cout << features[i][j] << ":";
		}
		cout << " : " << labels[i] << endl;
	}
}


	// create model
	// pass data
	// train
void testTraining() {
	std::cout << __PRETTY_FUNCTION__ << std::endl;
	Classifier model{};
	model.open(g_filename);
	model.setNumOfClasses(3);
	model.train();
}



	// create mode
	// pass data
	// train model
	// save real classe
	// save predicted classes
	// compare real and predicted classes
void testPrediction() {
	std::cout << __PRETTY_FUNCTION__ << std::endl;
	Classifier model{};
	model.open(g_filename);
	Features features = model.getTrainFeatures();
	Labels labels = model.getTrainLabels();
	unsigned int numOfClasses = 3;
	model.train(features, numOfClasses);
	Labels predictedLabels = model.predict(features);
	assert(labels.size() == predictedLabels.size());
//	for(size_t i = 0; i < labels.size(); i++) {
//		std::cout << i << " : " << labels[i] << " : " << predictedLabels[i] << endl;
//	}
}



	// train model
	// get train classes by internal decision tree
	// get predicted classes by internal decision tree
	// compare classes
void testAlgEvaluation() {
}

	// train model
	// save model
	// load model
	// compare original and loaded objects
void testSerialization() {
	std::cout << __PRETTY_FUNCTION__ << std::endl;
	Classifier model{};
	model.open(g_filename);
	Features features = model.getTrainFeatures();
	Labels labels = model.getTrainLabels();
	unsigned int numOfClasses = 3;
	model.train(features, numOfClasses);
	model.save("model.dat");
}

int main(int argc, char** argv) {
	using namespace std;
	Classifier::init();
	cout <<	__PRETTY_FUNCTION__ << endl;
	testLoadData();
	testTraining();
	testPrediction();
//	testAlgEvaluation();
	testSerialization();
	return 0;
}
