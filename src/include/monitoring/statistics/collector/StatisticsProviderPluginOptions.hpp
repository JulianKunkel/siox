#ifndef STATISTICS_PROVIDER_PLUGIN_OPTIONS
#define STATISTICS_PROVIDER_PLUGIN_OPTIONS

#include <core/component/component-options.hpp>
#include <core/component/ComponentReference.hpp>


namespace monitoring {

	//@serializable
	class StatisticsProviderPluginOptions: public core::ComponentOptions {
		public:
			core::ComponentReference statisticsCollector;
	};

}

#endif
