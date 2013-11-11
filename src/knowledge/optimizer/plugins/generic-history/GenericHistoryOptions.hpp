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
	vector<string> accessTokens;
	vector<string> closeTokens;
	vector<string> hintTokens;
	// where are the hints stored: activity name and vector of attribute names.
	//vector<pair<string, vector<pair<string,string>>>> hintAttributes;

	vector<pair<string,string>> accessRelevantOntologyAttributes; // e.g. MPI descriptor/fileHandle
	// Plug this into siox.conf to comply with new format:
/*		<accessRelevantOntologyAttributes>
			<count>2</count>
			<item_version>0</item_version>
		        <item>
	        	        <first>MPI</first>
		                <second>quantity/BytesToRead</second>
		        </item>
		        <item>
	        	        <first>MPI</first>
		                <second>quantity/BytesToWrite</second>
		        </item>
		</accessRelevantOntologyAttributes>
*/
	vector<pair<string,string> hintAttributes;	// attributes containing hints, e.g. ('MPI','hint/noncollReadBuffSize')
};

