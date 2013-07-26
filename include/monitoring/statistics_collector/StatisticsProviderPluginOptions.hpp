#ifndef STATISTICS_PROVIDER_PLUGIN_OPTIONS
#define STATISTICS_PROVIDER_PLUGIN_OPTIONS

#include <core/component/component-macros.hpp>


using namespace core;

namespace monitoring{


class StatisticsProviderPluginOptions: public ComponentOptions{
public:
	ComponentReference statisticsCollector;

	SERIALIZE_CONTAINER(MEMBER(statisticsCollector))
};

}

#define SERIALIZE_OPTIONS(VAR_) SERIALIZE(VAR_ PARENT_CLASS(StatisticsProviderPluginOptions))

#endif
