/*
 * =====================================================================================
 *
 *       Filename:  OnlineMonitoring.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/20/2016 03:02:08 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Eugen Betke (betke@dkrz.de), Julian Kunkel (julian.kunkel@googlemail.com)
 *   Organization:  Deutsches Klimarechenzentrum
 *
 * =====================================================================================
 */


#ifndef  OnlineMonitoring_INC
#define  OnlineMonitoring_INC

#include <regex>
#include <unordered_map>
#include <thread>
#include <mutex>

#include <monitoring/ontology/Ontology.hpp>
#include <monitoring/datatypes/Activity.hpp>
#include <monitoring/association_mapper/AssociationMapper.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginImplementation.hpp>

#include "OnlineMonitoringOptions.hpp"
#include "clients/TSDBClient.hpp"
#include "clients/ElasticClient.hpp"


typedef struct MetricAggregation {
  size_t bytes;
  size_t count;
  size_t time;
	
	void reset() {
		bytes = 0;
		count = 0;
		time = 0;
	}
} MetricAggregation;


struct EnumClassHash {
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
	OPEN, CLOSE, READ, WRITE, SYNC, SEEK
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

class OnlineMonitoringPlugin : public ActivityMultiplexerPlugin {
	public:
		void initPlugin() override;
		OnlineMonitoringPluginOptions* AvailableOptions() override;
		void finalize() override;
		void notify (const std::shared_ptr<Activity>& activity, int lost);

	private:

		int m_tsdb_enabled;
		std::string m_tsdb_host;
		std::string m_tsdb_port;
		std::string m_tsdb_username;
		std::string m_tsdb_password;

		int m_elastic_enabled;
		std::string m_elastic_host;
		std::string m_elastic_port;
		std::string m_elastic_username;
		std::string m_elastic_password;

		std::string m_metric_host;
		std::string m_metric_username;
		std::string m_metric_jobid;
		std::string m_metric_procid;

		std::thread m_thread;
		bool m_thread_stop{false};

		std::unordered_map<std::string, std::unordered_map<IOAccessType, std::mutex, EnumClassHash>> m_mutex;
		std::unordered_map<std::string, std::unordered_map<IOAccessType, MetricAggregation, EnumClassHash>> m_agg;

		ofstream ofile;
		Ontology* o;
		size_t verbosity = 0;
		size_t fh_counter = 0; // dummy file handler counter
		int enableSyscallStats;
		int enableTrace;
		std::unordered_map<ActivityID, OpenFiles> openFiles;
		std::unordered_map<int, OpenFiles> unnamedFiles;
		std::unordered_map<UniqueComponentActivityID, void (OnlineMonitoringPlugin::*)(std::shared_ptr<Activity>)> activityHandlers;
		IOInterface io_iface;
		size_t file_limit;
		SystemInformationGlobalIDManager* sys_info;
		std::unordered_map<UniqueComponentActivityID, size_t> accessCounter;

		// Ontology attributes needed in the plugin
		std::unordered_map<IOInterface, OntologyAttributeID, EnumClassHash> fhID;
		std::unordered_map<IOInterface, OntologyAttributeID, EnumClassHash> fname;
		std::unordered_map<IOInterface, OntologyAttributeID, EnumClassHash> bytesReadID;
		std::unordered_map<IOInterface, OntologyAttributeID, EnumClassHash> bytesToReadID;
		std::unordered_map<IOInterface, OntologyAttributeID, EnumClassHash> positionID;
		std::unordered_map<IOInterface, OntologyAttributeID, EnumClassHash> bytesWrittenID;		
		std::unordered_map<IOInterface, OntologyAttributeID, EnumClassHash> bytesToWriteID;		

		TSDBClient m_tsdb_client;
		ElasticClient m_elastic_client;

		void aggregate(const IOAccessType access_type, const Timestamp start, const Timestamp stop, const uint64_t position, const uint64_t bytes, const OpenFiles& file);
		void sendToDB();
		void addActivityHandler(const string & interface, const string & impl, const string & activity, void (OnlineMonitoringPlugin::* handler)(std::shared_ptr<Activity>));
		void printFileAccess(const OpenFiles & file);

		void handleOpen(std::shared_ptr<Activity> activity);
		void handleSync(std::shared_ptr<Activity> activity);
		void handleWrite(std::shared_ptr<Activity> activity);
		void handleRead(std::shared_ptr<Activity> activity);
		void handleClose(std::shared_ptr<Activity> activity);
		void handleSeek(std::shared_ptr<Activity> a);

		OpenFiles* findParentFile( const std::shared_ptr<Activity> activity );
		OpenFiles* findParentFileByFh( const std::shared_ptr<Activity> activity );

		Ontology * ontology = nullptr;
		SystemInformationGlobalIDManager * system_information_manager = nullptr;
		AssociationMapper * association_mapper = nullptr;
};

#endif   /* ----- #ifndef OnlineMonitoring_INC  ----- */
