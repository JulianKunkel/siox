#ifndef TOOLS_TRACE_PLOTTER_H
#define TOOLS_TRACE_PLOTTER_H

#include <regex>
#include <unordered_map>

#include <monitoring/ontology/Ontology.hpp>
#include <monitoring/datatypes/Activity.hpp>
#include <monitoring/association_mapper/AssociationMapper.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginImplementation.hpp>
#include <monitoring/activity_multiplexer/plugins/access_info_plotter/AccessInfoPlotterOptions.hpp>


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

class AccessInfoPlotterPlugin : public ActivityMultiplexerPlugin {
	public:
		void initPlugin() override;
		AccessInfoPlotterPluginOptions* AvailableOptions() override;
		void finalize() override;
		void notify (const std::shared_ptr<Activity>& activity, int lost);
	private:
		Ontology * o;
		size_t verbosity = 0;
		
		void addActivityHandler(const string & interface, const string & impl, const string & activity, void (AccessInfoPlotterPlugin::* handler)(std::shared_ptr<Activity>));

		void plotFileAccess(const OpenFiles & file);
		void plotSingleFileAccess(const string & type, const vector<Access> & accessVector, ofstream & f, const OpenFiles & file);

		void plotSingleFileOperation(const string & type, const vector<Operation> & ops, ofstream & f, const OpenFiles & file);

		unordered_map<ActivityID, OpenFiles> openFiles;
		unordered_map<int, OpenFiles> unnamedFiles;

		unordered_map<UniqueComponentActivityID, void (AccessInfoPlotterPlugin::*)(std::shared_ptr<Activity>)> activityHandlers;

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
