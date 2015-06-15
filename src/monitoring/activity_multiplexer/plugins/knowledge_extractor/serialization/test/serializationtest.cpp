/*
 * =====================================================================================
 *
 *       Filename:  serializationtest.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/25/2015 07:15:19 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "../BasicTypesSerialization.hpp"
#include "../SGVectorSerialization.hpp"
#include "../CARTreeNodeDataSerialization.hpp"
#include "../CSGObjectPtrSerialization.hpp"
#include "../CDynamicObjectArraySerialization.cpp"
#include "../CTreeMachineNodeSerialization.hpp"
#include "../TreeSerialization.hpp"
#include <string>
#include <fstream>
#include <cassert>

#include <shogun/io/CSVFile.h>
#include <shogun/base/init.h>
#include <shogun/mathematics/Math.h>

#include <shogun/features/DenseFeatures.h>
#include <shogun/labels/RegressionLabels.h>
#include <shogun/evaluation/MulticlassAccuracy.h>

#include <shogun/multiclass/tree/CARTree.h>
#include <shogun/multiclass/tree/TreeMachine.h>
#include <shogun/multiclass/tree/TreeMachineNode.h>
#include <shogun/multiclass/tree/CARTreeNodeData.h>

void testBasicTypesSerialization() {
	using namespace std;
	cout << __PRETTY_FUNCTION__ << endl;
	using namespace shogun;
	string filename{"float64_t_test.dat"};

	ofstream ofs{filename};
	float64_t fsource = 5.43;
	ofs << fsource;
	ofs.close();

	ifstream ifs{filename};
	float64_t ftarget = 0;
	ifs >> ftarget;
	ifs.close();

	assert(fsource == ftarget);
}


void testSGVectorSerialization() {
	using namespace std;
	cout << __PRETTY_FUNCTION__ << endl;
	using namespace shogun;
	using namespace io;
	string filename{"sgvector_test.dat"};

	ofstream ofs{filename};
	SGVector<float64_t> fsource{3};
	fsource[0] = 4.32;
	fsource[1] = 4.55;
	fsource[2] = 9;
	ofs << fsource;
	ofs.close();

	ifstream ifs{filename};
	SGVector<float64_t> ftarget{};
	ifs >> ftarget;
	ifs.close();

	assert(fsource == ftarget);
}

void testCARTreeNodeDataSerialization() {
	using namespace std;
	cout << __PRETTY_FUNCTION__ << endl;
	using namespace shogun;
	using namespace io;
	string filename{"cartnodedata_test.dat"};

	SGVector<float64_t> tiv{3};
	tiv[0] = 4.32;
	tiv[1] = 4.55;
	tiv[2] = 9;

	ofstream ofs{filename};
	CARTreeNodeData source{};
	source.weight_minus_node = 20.32;
	source.weight_minus_branch = 23.5;
	source.num_leaves = 20;
	source.transit_into_values = tiv;
	source.node_label = 32;
	source.attribute_id = 2;
	source.total_weight = 3200;
	ofs << source;
	ofs.close();

	ifstream ifs{filename};
	CARTreeNodeData target{};
	ifs >> target;
	ifs.close();

	assert(source == target);
}

void testCSGObjectPtrSerialization() {
	using namespace std;
	cout << __PRETTY_FUNCTION__ << endl;
	using namespace shogun;
	using namespace io;
	string filename{"sgobjectptr_test.dat"};

	init_shogun();
	ofstream ofs{filename};
	CSGObject* source = new CBinaryTreeMachineNode<CARTreeNodeData>{};
	ofs << source;
	cout << "source: " << source << endl;
	ofs.close();

	ifstream ifs{filename};
	CSGObject* target{nullptr};
	ifs >> target;
	assert(target != nullptr);
	cout << "target: " << target << endl;
	ifs.close();
	exit_shogun();

	assert(source == target);
}

void testCDynamicObjectArraySerialization() {
	using namespace std;
	cout << __PRETTY_FUNCTION__ << endl;
	using namespace shogun;
	using namespace io;
	string filename{"dynamicobjectarray_test.dat"};

	init_shogun();
	ofstream ofs{filename};
	CDynamicObjectArray* source = new CDynamicObjectArray{};
	SG_REF(source);
	source->push_back(new CTreeMachineNode<CARTreeNodeData>{});
	source->push_back(new CTreeMachineNode<CARTreeNodeData>{});
	dynObjArray(ofs, source);
	ofs.close();

	ifstream ifs{filename};
	CDynamicObjectArray* target{};
	SG_REF(target);
	dynObjArray(ifs, target);
	ifs.close();
	exit_shogun();

//	dynObjArray(cout, source);
//	dynObjArray(cout, target);

	assert(*source == *target);
}

void testCTreeMachineNodeSerialization() {
	using namespace std;
	cout << __PRETTY_FUNCTION__ << endl;
	using namespace shogun;
	using namespace io;
	string filename{"treemachinenode_test.dat"};
	
	init_shogun();
	ofstream ofs{filename};
	CTreeMachineNode<shogun::CARTreeNodeData> source{};
	ofs << source;
	ofs.close();

//	ifstream ifs{filename};
//	CTreeMachineNode<shogun::CARTreeNodeData> target{};
//	ifs >> target;
//	ifs.close();
//	exit_shogun();
//
//	cout << source << endl;
//	cout << target << endl;
//
//	assert(source == target);
}

void testTreeSerialization() {
	using namespace std;
	using namespace shogun;
	init_shogun();
	std::cout << __PRETTY_FUNCTION__ << std::endl;

	std::string filename{"/scratch/pool/CIS/m215026/git/predictor2/data/input_clean_data_fill.csv"};

	shogun::CCARTree* m_model = nullptr;
	SGVector<bool> attr_types(4);
	for (index_t i = 0; i < attr_types.size(); ++i) {
		attr_types[i] = false;
	}
	m_model = new CCARTree(attr_types, PT_REGRESSION, 10, false);
	SG_REF(m_model);

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

	CDenseFeatures<float64_t>* m_train_dfeats = new CDenseFeatures<float64_t>(train_feats);
	SG_REF(m_train_dfeats);
	CRegressionLabels* m_train_dlabels = new CRegressionLabels(train_labels);
	SG_REF(m_train_dlabels);

	m_model->set_labels(m_train_dlabels);
	m_model->train(m_train_dfeats);

	CTreeMachineNode<CARTreeNodeData>* root = m_model->get_root();
	assert(root);
	ofstream ofs{"treeserialization.dat"};
	io::saveTree(ofs, *root);
	io::saveTree(cout, *root);
	ofs.close();	

	ifstream ifs{"treeserialization.dat"};	
	CTreeMachineNode<CARTreeNodeData>* root2 = io::loadTree(ifs);
	assert(root2);
	io::saveTree(cout, *root2);
	ifs.close();
}

int main(int argc, char** argv) {
	using namespace std;
	cout << __PRETTY_FUNCTION__ << endl;
//	testBasicTypesSerialization();
//	testSGVectorSerialization();
//	testCARTreeNodeDataSerialization();
//	testCSGObjectPtrSerialization();
//	testCDynamicObjectArraySerialization();
//	testCTreeMachineNodeSerialization();
	testTreeSerialization();
	return 0;
}
