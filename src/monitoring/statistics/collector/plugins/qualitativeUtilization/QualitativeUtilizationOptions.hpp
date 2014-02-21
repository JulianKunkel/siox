#ifndef INCLUDE_GUARD_MONITORING_STATISTICS_COLLECTOR_PLUGINS_QUALITATIVE_UTILIZATION_QUALITATIVE_UTILIZATION_OPTIONS
#define INCLUDE_GUARD_MONITORING_STATISTICS_COLLECTOR_PLUGINS_QUALITATIVE_UTILIZATION_QUALITATIVE_UTILIZATION_OPTIONS

#include <monitoring/statistics/StatisticsIntegratorOptions.hpp>

//@serializable
class QualitativeUtilizationOptions : public monitoring::StatisticsIntegratorOptions {
	public:
		float availableIoBandwidth;
		float availableMemoryBandwidth;
		float availableNetworkBandwidth;
};

#endif
