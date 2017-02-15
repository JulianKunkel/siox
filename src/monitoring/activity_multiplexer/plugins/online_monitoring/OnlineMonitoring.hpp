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
#include <atomic>
#include <mutex>

#include <monitoring/ontology/Ontology.hpp>
#include <monitoring/datatypes/Activity.hpp>
#include <monitoring/association_mapper/AssociationMapper.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginImplementation.hpp>

#include "OnlineMonitoringOptions.hpp"
#include "clients/TSDBClient.hpp"
#include "clients/ElasticClient.hpp"


typedef struct MetricAggregation {
  size_t write_bytes;
  size_t write_count;
  size_t write_time;
  size_t read_bytes;
  size_t read_count;
  size_t read_time;
	
	void reset() {
		write_bytes = 0;
		write_count = 0;
		write_time = 0;
		read_bytes = 0;
		read_count = 0;
		read_time = 0;
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


struct OpenFile {
	string name;
	Timestamp openTime;
	Timestamp closeTime;
	Timestamp openDuration;
	Timestamp closeDuration;
	uint64_t currentPosition;
	ActivityID aid; 
	size_t bytesWritten;
	size_t bytesRead;
	Timestamp write_time;
	Timestamp read_time;
	size_t rnd_access;
	size_t seq_access;
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

		std::string m_metric_layer;
		std::string m_metric_host;
		std::string m_metric_username;
		std::string m_metric_jobid;
		std::string m_metric_procid;

		std::thread m_thread;
		bool m_thread_stop{false};

		std::unordered_map<std::string, std::mutex> m_read_mutex;
		std::unordered_map<std::string, std::mutex> m_write_mutex;
		std::unordered_map<std::string, MetricAggregation> m_agg;

		ofstream ofile;
		Ontology* o;
		size_t verbosity = 0;
		size_t fh_counter = 0; // dummy file handler counter
		int enableSyscallStats;
		int enableTrace;
		std::unordered_map<ActivityID, OpenFile> openFiles;
		std::unordered_map<int, OpenFile> unnamedFiles;
		std::unordered_map<UniqueComponentActivityID, void (OnlineMonitoringPlugin::*)(const std::shared_ptr<Activity>&)> activityHandlers;
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

		void sendToDB();
		void addActivityHandler(const string& interface, const string& impl, const string& activity, void (OnlineMonitoringPlugin::*handler)(const std::shared_ptr<Activity>&));
		void printFileAccess(const OpenFile& file);

		void handleOpen(const std::shared_ptr<Activity>& activity);
		void handleSync(const std::shared_ptr<Activity>& activity);
		void handleWrite(const std::shared_ptr<Activity>& activity);
		void handleRead(const std::shared_ptr<Activity>& activity);
		void handleClose(const std::shared_ptr<Activity>& activity);
		void handleSeek(const std::shared_ptr<Activity>& a);

		OpenFile* findParentFile(const std::shared_ptr<Activity>& activity);
		OpenFile* findParentFileByFh(const std::shared_ptr<Activity>& activity);

		Ontology* ontology = nullptr;
		SystemInformationGlobalIDManager* system_information_manager = nullptr;
		AssociationMapper* association_mapper = nullptr;
};

#endif   /* ----- #ifndef OnlineMonitoring_INC  ----- */
