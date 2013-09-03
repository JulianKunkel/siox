#ifndef STATISTICS_PROVIDER_PLUGIN_OPTIONS
#define STATISTICS_PROVIDER_PLUGIN_OPTIONS

#include <core/component/component-options.hpp>
#include <core/component/ComponentReference.hpp>


using namespace core;

namespace monitoring {

	//@serializable
	class StatisticsProviderPluginOptions: public ComponentOptions {
		public:
			ComponentReference statisticsCollector;
	};

}

#endif
