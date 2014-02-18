#ifndef INCLUDE_GUARD_MONITORING_ACTIVITY_MULTIPLEXER_PLUGINS_HISTOGRAM_ADPI_HISTOGRAM_ADPI
#define INCLUDE_GUARD_MONITORING_ACTIVITY_MULTIPLEXER_PLUGINS_HISTOGRAM_ADPI_HISTOGRAM_ADPI

#include <limits>
#include <vector>
#include <unordered_map>
#include <mutex>

#include <core/reporting/ComponentReportInterface.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginImplementation.hpp>
#include <monitoring/datatypes/Activity.hpp>
#include <monitoring/topology/Topology.hpp>
#include <knowledge/reasoner/AnomalyPlugin.hpp>

struct ActivityTimeStatistics{
	uint64_t minTimeS = numeric_limits<uint64_t>::max();
	Timestamp maxTimeS = 0;
	Timestamp histogramBucketWidth = 0; // (max - min) / binSize

	uint64_t totalOperationCount = 0;

	vector<uint64_t> histogram;
};

class HistogramAdpiPlugin: public monitoring::ActivityMultiplexerPlugin, public core::ComponentReportInterface, public knowledge::AnomalyPlugin {
	public:
		void initPlugin() override;
		void Notify( const shared_ptr<Activity> & activity, int lost );
		ComponentReport prepareReport() override;

		ComponentOptions * AvailableOptions() override;
		void finalize() override;
	private:
		mutex giant_mutex;
		unordered_map<UniqueComponentActivityID, ActivityTimeStatistics> statistics;
		TopologyTypeId   pluginTopoTypeID;
		TopologyObjectId pluginTopoObjectID;

		TopologyAttributeId bucketMinAttributID;
		TopologyAttributeId bucketMaxAttributID;
		Topology * topology;
};

#endif
