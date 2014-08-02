#include <string>

#include <monitoring/statistics/collector/StatisticsProviderPluginOptions.hpp>


//@serializable
class ProviderLikwidOptions: public monitoring::StatisticsProviderPluginOptions {
	public:
		// groups are separated by comma (,)
		std::string groups; 
};

