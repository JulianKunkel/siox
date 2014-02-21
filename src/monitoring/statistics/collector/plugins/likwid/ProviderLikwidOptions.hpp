#include <string>

#include <monitoring/statistics/collector/StatisticsProviderPluginOptions.hpp>


//@serializable
class ProviderLikwidOptions: public monitoring::StatisticsProviderPluginOptions {
	public:
		std::string groups; // at the moment only one group can be measured
};


