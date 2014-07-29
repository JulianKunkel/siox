#ifndef INCLUDE_GUARD_MONITORING_STATISTICS_COLLECTOR_PLUGINS_TEST_GENERATOR
#define INCLUDE_GUARD_MONITORING_STATISTICS_COLLECTOR_PLUGINS_TEST_GENERATOR

#include <monitoring/statistics/StatisticsIntegratorOptions.hpp>

//@serializable
class TestGeneratorOptions : public monitoring::StatisticsIntegratorOptions {
	public:
		int statisticsToCreate = 10;
		double minValue = 0;
		double maxValue = 100;
};

#endif
