#ifndef TOOLS_TRACE_CSVEXTRACTOR_H
#define TOOLS_TRACE_CSVEXTRACTOR_H

#include <regex>
#include <unordered_map>

#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginImplementation.hpp>

struct OpenFiles{
	string filename;
	uint64_t currentPosition;
	uint64_t lastAccessPosition;
};

class CSVExtractor: public monitoring::ActivityMultiplexerPlugin {
	public:
		ComponentOptions* AvailableOptions() override; 

		void finalize() override;

		void initPlugin();
	private:
		void notify(const std::shared_ptr<Activity>& a, int lost);

		Ontology * o;
		
		void addActivityHandler(const string & interface, const string & impl, const string & activity, void (CSVExtractor::* handler)(std::shared_ptr<Activity>));
	
		unordered_map<ActivityID, OpenFiles> openFiles;
		unordered_map<int, OpenFiles> unnamedFiles;

		unordered_map<UniqueComponentActivityID, void (CSVExtractor::*)(std::shared_ptr<Activity>)> activityHandlers;

		OpenFiles * findParentFile( std::shared_ptr<Activity>& activity );
		OpenFiles * findParentFileByFh( std::shared_ptr<Activity>& activity );


		void handlePOSIXOpen(std::shared_ptr<Activity> activity);
		void handlePOSIXSync(std::shared_ptr<Activity> activity);
		void handlePOSIXWrite(std::shared_ptr<Activity> activity);
		void handlePOSIXRead(std::shared_ptr<Activity> activity);
		void handlePOSIXClose(std::shared_ptr<Activity> activity);
		void handlePOSIXSeek(std::shared_ptr<Activity> a);

		void handlePOSIXAccess(std::shared_ptr<Activity> activity, string type);

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