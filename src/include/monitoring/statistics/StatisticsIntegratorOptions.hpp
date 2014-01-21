/**
 * @author Nathanael Hübbe
 * @date 2014
 */

#ifndef INCLUDE_GUARD_MONITORING_STATISTICS_STATISTICS_INTEGRATOR_OPTIONS
#define INCLUDE_GUARD_MONITORING_STATISTICS_STATISTICS_INTEGRATOR_OPTIONS

#include <monitoring/statistics/collector/StatisticsProviderPluginOptions.hpp>

namespace monitoring {
	//@serializable
	class StatisticsIntegratorOptions : public StatisticsProviderPluginOptions {
		public:
			core::ComponentReference multiplexer;
			core::ComponentReference dereferencingFacade;
	};
}

#endif
