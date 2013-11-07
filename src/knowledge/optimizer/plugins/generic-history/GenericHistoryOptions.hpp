#include <knowledge/optimizer/OptimizerPluginOptions.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginOptions.hpp>

#include <vector>
#include <utility>

using namespace std;
using namespace monitoring;


//@serializable
struct GenericHistoryOptions : public ActivityMultiplexerPluginOptions{
	ComponentReference optimizer;

	// the layer we are operating on must be the layer we are connected to.
	string interface;	
	string implementation;

	vector<string> openTokens;
	vector<string> closeTokens;
	vector<string> accessTokens;
	// where are the hints stored: activity name and vector of attribute names.
	//vector<pair<string, vector<pair<string,string>>>> hintAttributes;
	vector<pair<string,string>> accessRelevantOntologyAttributes; // e.g. MPI descriptor/fileHandle
};

