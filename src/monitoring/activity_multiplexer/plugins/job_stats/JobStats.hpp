/*
 * =====================================================================================
 *
 *       Filename:  JobStats.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/20/2016 03:02:08 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */


#ifndef  JobStats_INC
#define  JobStats_INC

#include <regex>
#include <unordered_map>

#include <monitoring/ontology/Ontology.hpp>
#include <monitoring/datatypes/Activity.hpp>
#include <monitoring/association_mapper/AssociationMapper.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginImplementation.hpp>

#include "JobStatsOptions.hpp"


struct Operation {
	Timestamp startTime;
	Timestamp endTime;
};

struct Access {
	Timestamp startTime;
	Timestamp endTime;
	uint64_t offset;
	uint64_t size;
};

struct OpenFiles {
	string name;
	Timestamp openTime;
	Timestamp closeTime;
	uint64_t currentPosition;
	ActivityID aid; 
	vector<Access> readAccesses;
	vector<Access> writeAccesses;
	vector<Operation> syncOperations;
};

class JobStatsPlugin : public ActivityMultiplexerPlugin {
	public:
		void initPlugin() override;
		JobStatsPluginOptions* AvailableOptions() override;
		void finalize() override;
		void notify (const std::shared_ptr<Activity>& activity, int lost);

	private:
		ofstream ofile;
		Ontology* o;
		size_t verbosity = 0;
		size_t fh_counter = 0; // dummy file handler counter
		unordered_map<ActivityID, OpenFiles> openFiles;
		unordered_map<int, OpenFiles> unnamedFiles;
		unordered_map<UniqueComponentActivityID, void (JobStatsPlugin::*)(std::shared_ptr<Activity>)> activityHandlers;

		// Ontology attributes needed in the plugin
		OntologyAttributeID fhID;
		OntologyAttributeID fname;
		OntologyAttributeID bytesReadID;
		OntologyAttributeID positionID;
		OntologyAttributeID bytesWrittenID;		
		
		void addActivityHandler(const string & interface, const string & impl, const string & activity, void (JobStatsPlugin::* handler)(std::shared_ptr<Activity>));
		void printFileAccess(const OpenFiles & file);
//		void plotSingleFileAccess(const string & type, const vector<Access> & accessVector, ofstream & f, const OpenFiles & file);
//		void plotSingleFileOperation(const string & type, const vector<Operation> & ops, ofstream & f, const OpenFiles & file);
		void handlePOSIXOpen(std::shared_ptr<Activity> activity);
		void handlePOSIXSync(std::shared_ptr<Activity> activity);
		void handlePOSIXWrite(std::shared_ptr<Activity> activity);
		void handlePOSIXRead(std::shared_ptr<Activity> activity);
		void handlePOSIXClose(std::shared_ptr<Activity> activity);
		void handlePOSIXSeek(std::shared_ptr<Activity> a);

		OpenFiles* findParentFile( const std::shared_ptr<Activity> activity );
		OpenFiles* findParentFileByFh( const std::shared_ptr<Activity> activity );
};

#endif   /* ----- #ifndef JobStats_INC  ----- */
