/*
 * =====================================================================================
 *
 *       Filename:  FileAccessInfo.hpp
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


#ifndef  FileAccessInfo_INC
#define  FileAccessInfo_INC

#include <regex>
#include <unordered_map>

#include <monitoring/ontology/Ontology.hpp>
#include <monitoring/datatypes/Activity.hpp>
#include <monitoring/association_mapper/AssociationMapper.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginImplementation.hpp>

#include "FileAccessInfoOptions.hpp"



struct IOInterfaceHash
{
    template <typename T>
    std::size_t operator()(T t) const
    {
        return static_cast<std::size_t>(t);
    }
};

enum class IOInterface {
	POSIX, MPI
};

enum class IOAccessType {
	READ, WRITE, SYNC
};

struct Operation {
	Timestamp startTime;
	Timestamp endTime;
};

struct Access {
	IOAccessType type;
	Timestamp startTime;
	Timestamp endTime;
	uint64_t offset;
	uint64_t size;
};

struct OpenFiles {
	string name;
	Timestamp openTime;
	Timestamp closeTime;
	Timestamp openDuration;
	Timestamp closeDuration;
	uint64_t currentPosition;
	ActivityID aid; 
	vector<Access> accesses;
	vector<Operation> syncOperations;
};

class FileAccessInfoPlugin : public ActivityMultiplexerPlugin {
	public:
		void initPlugin() override;
		FileAccessInfoPluginOptions* AvailableOptions() override;
		void finalize() override;
		void notify (const std::shared_ptr<Activity>& activity, int lost);

	private:
		ofstream ofile;
		Ontology* o;
		size_t verbosity = 0;
		size_t fh_counter = 0; // dummy file handler counter
		int enableSyscallStats;
		int enableTrace;
		std::unordered_map<ActivityID, OpenFiles> openFiles;
		std::unordered_map<int, OpenFiles> unnamedFiles;
		std::unordered_map<UniqueComponentActivityID, void (FileAccessInfoPlugin::*)(std::shared_ptr<Activity>)> activityHandlers;
		IOInterface io_iface;
		size_t file_limit;
		SystemInformationGlobalIDManager* sys_info;
		std::unordered_map<UniqueComponentActivityID, size_t> accessCounter;

		// Ontology attributes needed in the plugin
		std::unordered_map<IOInterface, OntologyAttributeID, IOInterfaceHash> fhID;
		std::unordered_map<IOInterface, OntologyAttributeID, IOInterfaceHash> fname;
		std::unordered_map<IOInterface, OntologyAttributeID, IOInterfaceHash> bytesReadID;
		std::unordered_map<IOInterface, OntologyAttributeID, IOInterfaceHash> bytesToReadID;
		std::unordered_map<IOInterface, OntologyAttributeID, IOInterfaceHash> positionID;
		std::unordered_map<IOInterface, OntologyAttributeID, IOInterfaceHash> bytesWrittenID;		
		std::unordered_map<IOInterface, OntologyAttributeID, IOInterfaceHash> bytesToWriteID;		

		
		void addActivityHandler(const string & interface, const string & impl, const string & activity, void (FileAccessInfoPlugin::* handler)(std::shared_ptr<Activity>));
		void printFileAccess(const OpenFiles & file);
		void handleOpen(std::shared_ptr<Activity> activity);
		void handleSync(std::shared_ptr<Activity> activity);
		void handleWrite(std::shared_ptr<Activity> activity);
		void handleRead(std::shared_ptr<Activity> activity);
		void handleClose(std::shared_ptr<Activity> activity);
		void handleSeek(std::shared_ptr<Activity> a);

		OpenFiles* findParentFile( const std::shared_ptr<Activity> activity );
		OpenFiles* findParentFileByFh( const std::shared_ptr<Activity> activity );
};

#endif   /* ----- #ifndef FileAccessInfo_INC  ----- */
