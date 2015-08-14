#ifndef  ReplayOptions_INC
#define  ReplayOptions_INC

#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginOptions.hpp>

//@serializable
class ReplayPluginOptions : public monitoring::ActivityMultiplexerPluginOptions {
	public:
		uint32_t verbosity;
};
#endif
