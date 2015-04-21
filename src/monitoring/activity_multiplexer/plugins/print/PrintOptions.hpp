#ifndef  PrintOptions_INC
#define  PrintOptions_INC

#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginOptions.hpp>

//@serializable
class PrintPluginOptions : public monitoring::ActivityMultiplexerPluginOptions {
	public:
		uint32_t verbosity;
};
#endif