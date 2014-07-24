#include <string>

#include <monitoring/statistics/collector/StatisticsProviderPluginOptions.hpp>


//@serializable
class ProviderLikwidOptions: public monitoring::StatisticsProviderPluginOptions {
	public:
		std::string groups; // groups are separated by comma (,)
};