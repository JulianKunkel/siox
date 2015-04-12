#ifndef TOOLS_TRACE_CSVEXTRACTOR_H
#define TOOLS_TRACE_CSVEXTRACTOR_H

#include <regex>
#include <unordered_map>

#include <tools/TraceReader/TraceReaderPluginImplementation.hpp>

using namespace tools;

struct OpenFiles{
	string filename;
	uint64_t currentPosition;
	uint64_t lastAccessPosition;
};

class CSVExtractor: public TraceReaderPlugin{
	protected:
		void init(program_options::variables_map * vm, TraceReader * tr) override;
	public:
		Activity * processNextActivity(Activity * activity) override;

		void moduleOptions(program_options::options_description & od) override;

		void finalize() override;
	private:
		Ontology * o;
		
		void addActivityHandler(const string & interface, const string & impl, const string & activity, void (CSVExtractor::* handler)(Activity*));

	
		unordered_map<ActivityID, OpenFiles> openFiles;
		unordered_map<int, OpenFiles> unnamedFiles;

		unordered_map<UniqueComponentActivityID, void (CSVExtractor::*)(Activity*)> activityHandlers;

		OpenFiles * findParentFile( const Activity * activity );
		OpenFiles * findParentFileByFh( const Activity * activity );


		void handlePOSIXOpen(Activity * activity);
		void handlePOSIXSync(Activity * activity);
		void handlePOSIXWrite(Activity * activity);
		void handlePOSIXRead(Activity * activity);
		void handlePOSIXClose(Activity * activity);
		void handlePOSIXSeek(Activity * a);

		void handlePOSIXAccess(Activity * activity, string type);

		// Ontology attributes needed in the plugin
		OntologyAttributeID fhID;
		OntologyAttributeID fname;
		OntologyAttributeID bytesReadID;
		OntologyAttributeID positionID;
		OntologyAttributeID bytesWrittenID;

		ofstream csv;
		// the timestamp of the first event.
		Timestamp tstart = 0;

		// the timestamp of the last event.
		Timestamp tlast = 0;

		double convertUpdateTime(Timestamp t);
};

#endif