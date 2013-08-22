#ifndef SIOX_STATISTICS_MULTIPLEXER_MODULE
#define SIOX_STATISTICS_MULTIPLEXER_MODULE

#include <monitoring/statistics_multiplexer/StatisticsMultiplexer.hpp>

#define PLUGIN(x) \
	extern "C"{\
		void * get_instance_monitoring_statisticsmultiplexer() { return new x(); }\
	}

#endif
