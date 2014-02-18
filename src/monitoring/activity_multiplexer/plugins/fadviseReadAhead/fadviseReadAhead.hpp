#ifndef INCLUDE_GUARD_MONITORING_ACTIVITY_MULTIPLEXER_PLUGINS_FADVISE_READ_AHEAD_FADVISE_READ_AHEAD_PLUGIN
#define INCLUDE_GUARD_MONITORING_ACTIVITY_MULTIPLEXER_PLUGINS_FADVISE_READ_AHEAD_FADVISE_READ_AHEAD_PLUGIN

#include <string.h>
#include <mutex>
#include <unordered_map>

#include <core/reporting/ComponentReportInterface.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginImplementation.hpp>
#include <monitoring/datatypes/Activity.hpp>
#include <knowledge/reasoner/AnomalyPlugin.hpp>

struct FileStatistics{
	FileStatistics(){}
	FileStatistics(string filename) : filename(filename){}

	string filename;

	uint64_t correctPredictions = 0;
	uint64_t missPredictions = 0;
	uint64_t prefetchesConducted = 0;
	uint64_t prefetchedData = 0;

	// any missprediction will reset the following counter to zero
	uint64_t correctPredictionInARow = 0;

	// the prediction for the next access
	uint64_t nextPredictionSize = 0;
	uint64_t nextPredictionOffset = 0;

	// the last offset, size accessed (needed to predict the stride)
	uint64_t lastReadOffset = 0;
	uint64_t lastReadSize = 0;
};

class FadviseReadAheadPlugin : public monitoring::ActivityMultiplexerPlugin, public core::ComponentReportInterface, public knowledge::AnomalyPlugin {
	public:
		void initPlugin() override;
		void Notify( const shared_ptr<Activity> & activity, int lost );
		ComponentReport prepareReport() override;

		ComponentOptions * AvailableOptions() override;
		void finalize() override;
	private:
		mutex giant_mutex;
		
		// statistics by parent file_open
		unordered_map<ActivityID, FileStatistics> statistics;
		unordered_map<int, FileStatistics> statisticsUnnamedFiles;

		unordered_map<UniqueComponentActivityID, void (FadviseReadAheadPlugin::*)(Activity*)> activityHandlers;

		FileStatistics * findParentFile( const Activity * activity );
		FileStatistics * findParentFileByFh( const Activity * activity );

		void handlePOSIXOpen(Activity * activity);
		void handlePOSIXWrite(Activity * activity);
		void handlePOSIXRead(Activity * activity);
		void handlePOSIXClose(Activity * activity);

		void addReport(ComponentReport & report, FileStatistics & statistics, GroupEntry * ge);

		void addActivityHandler(ActivityPluginDereferencing * f, const string & interface, const string & impl, const string & a, void (FadviseReadAheadPlugin::* handler)(Activity*) );
		
		// Ontology attributes needed in the plugin
		OntologyAttributeID fhID;
		OntologyAttributeID fname;
		OntologyAttributeID bytesReadID;
		OntologyAttributeID positionID;
		OntologyAttributeID bytesWrittenID;
};

#endif
