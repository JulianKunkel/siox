#ifndef TOOLS_TRACE_PLOTTER_H
#define TOOLS_TRACE_PLOTTER_H

#include <regex>
#include <unordered_map>

#include <tools/TraceReader/TraceReaderPluginImplementation.hpp>

using namespace tools;

struct Operation{
	Timestamp startTime;
	Timestamp endTime;
};

struct Access{
	Timestamp startTime;
	Timestamp endTime;
	uint64_t offset;
	uint64_t size;
};

struct OpenFiles{
	string name;
	Timestamp openTime;
	Timestamp closeTime;

	uint64_t currentPosition;

	ActivityID aid; 

	vector<Access> readAccesses;
	vector<Access> writeAccesses;
	vector<Operation> syncOperations;
};

class AccessInfoPlotter: public TraceReaderPlugin{
	protected:
		void init(program_options::variables_map * vm, TraceReader * tr) override;
	public:
		std::shared_ptr<Activity> processNextActivity(std::shared_ptr<Activity> activity) override;

		void moduleOptions(program_options::options_description & od) override;

		void finalize() override;
	private:
		Ontology * o;
		
		void addActivityHandler(const string & interface, const string & impl, const string & activity, void (AccessInfoPlotter::* handler)(std::shared_ptr<Activity>));

		void plotFileAccess(const OpenFiles & file);
		void plotSingleFileAccess(const string & type, const vector<Access> & accessVector, ofstream & f, const OpenFiles & file);

		void plotSingleFileOperation(const string & type, const vector<Operation> & ops, ofstream & f, const OpenFiles & file);

		unordered_map<ActivityID, OpenFiles> openFiles;
		unordered_map<int, OpenFiles> unnamedFiles;

		unordered_map<UniqueComponentActivityID, void (AccessInfoPlotter::*)(std::shared_ptr<Activity>)> activityHandlers;

		OpenFiles * findParentFile( const std::shared_ptr<Activity> activity );
		OpenFiles * findParentFileByFh( const std::shared_ptr<Activity> activity );


		void handlePOSIXOpen(std::shared_ptr<Activity> activity);
		void handlePOSIXSync(std::shared_ptr<Activity> activity);
		void handlePOSIXWrite(std::shared_ptr<Activity> activity);
		void handlePOSIXRead(std::shared_ptr<Activity> activity);
		void handlePOSIXClose(std::shared_ptr<Activity> activity);
		void handlePOSIXSeek(std::shared_ptr<Activity> a);

		// Ontology attributes needed in the plugin
		OntologyAttributeID fhID;
		OntologyAttributeID fname;
		OntologyAttributeID bytesReadID;
		OntologyAttributeID positionID;
		OntologyAttributeID bytesWrittenID;		
};

#endif
