#include <knowledge/optimizer/OptimizerPluginOptions.hpp>

#include <vector>
#include <utility>

using namespace std;


/*
 * Per file (extension) the options contains a list of hints for the attributes.
 */
//@serializable
struct HintOption{
	string domain;
	string name;
	string value; // will be converted to the respective data type as listed in the ontology!
};

//@serializable
struct FileOptions{
	string filename;
	vector<HintOption> hints;
};

//@serializable
struct attribute{
	string domain;
	string name;
};

//@serializable
struct activityDescription{
	string interface;
	string implementation;
	string activityName;
};

//@serializable
struct PersistentStoreOptions : public OptimizerPluginOptions{
	// values are set to show how to use the XML
	vector<FileOptions> fileHints = {{ "test", {{"MPI", "hints/noncollReadBuffSize", "102400"}} }}; 

	attribute 				filenameAttribute;
	activityDescription  openActivity;
};

