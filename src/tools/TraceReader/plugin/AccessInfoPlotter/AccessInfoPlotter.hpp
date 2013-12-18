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

	uint64_t number; // increasing

	vector<Access> readAccesses;
	vector<Access> writeAccesses;
	vector<Operation> syncOperations;
};

class AccessInfoPlotter: public TraceReaderPlugin{
	protected:
		void init(program_options::variables_map * vm, TraceReader * tr) override;
	public:
		void nextActivity(Activity * activity) override;

		void moduleOptions(program_options::options_description & od) override;

		void finalize() override;
	private:
		Ontology * o;
		
		void addActivityHandler(const string & interface, const string & impl, const string & activity, void (AccessInfoPlotter::* handler)(Activity*));

		void plotFileAccess(const OpenFiles & file);
		void plotSingleFileAccess(const string & type, const vector<Access> & accessVector, ofstream & f, const OpenFiles & file);

		void plotSingleFileOperation(const string & type, const vector<Operation> & ops, ofstream & f, const OpenFiles & file);

		unordered_map<ActivityID, OpenFiles> openFiles;
		unordered_map<int, OpenFiles> unnamedFiles;

		unordered_map<UniqueComponentActivityID, void (AccessInfoPlotter::*)(Activity*)> activityHandlers;

		OpenFiles * findParentFile( const Activity * activity );
		OpenFiles * findParentFileByFh( const Activity * activity );


		void handlePOSIXOpen(Activity * activity);
		void handlePOSIXSync(Activity * activity);
		void handlePOSIXWrite(Activity * activity);
		void handlePOSIXRead(Activity * activity);
		void handlePOSIXClose(Activity * activity);
};

#endif