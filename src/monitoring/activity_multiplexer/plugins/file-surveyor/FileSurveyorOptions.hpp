//#include <knowledge/optimizer/OptimizerPluginOptions.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginOptions.hpp>

#include <vector>
#include <utility>

using namespace std;
using namespace monitoring;


//@serializable
struct FileSurveyorOptions : public ActivityMultiplexerPluginOptions{
	//ComponentReference optimizer;

	// the layer we are operating on must be the layer we are connected to.
	string interface;
	string implementation;

	// The list of file extensions we are to watch out for
	vector<string> fileExtensionsToWatch;

	// The lists of activity domains and names belonging to each of these groups
	// (open calls, file access calls, close calls)
	vector<string> openTokens;
	//vector<string> seekTokens; // TODO Recognize and process seek() calls!
	vector<string> readTokens;
	vector<string> writeTokens;
	vector<string> closeTokens;

	// The highest number of bytes that will still be classified as a "short jump";
	// anything above will be deemed a "long jump".
	uint64_t seekSizeLimit;

	// Reporting options; set to 1 for true, 0 for false.
	// Setting BOTH of these to 0 will switch off reports from this plugin!
	// Should every combination of (UserID, FileName) be reported separately?
	bool individualReports = false;
	// Should an aggregation over all combinations of (UserID, FileName) be reported?
	bool aggregatedReport = false;
};

