#include <monitoring/datatypes/Statistics.hpp>
#include <monitoring/activity_multiplexer/StatisticsMultiplexerImplementation.hpp>
#include <template/monitoring/multiplexer/MultiplexerFiltered.hpp>
#include <monitoring/activity_multiplexer/StatisticsMultiplexerListener.hpp>

using namespace core;
using namespace monitoring;

namespace monitoring{

/**
 * ActivityMultiplexer
 * Forwards logged activities to registered listeners (e.g. Plugins) either
 * in an syncronised or asyncronous manner.
 */
class StatisticsMultiplexerSync : public MultiplexerFilteredTemplate<Statistics>
{

};

}


CREATE_SERIALIZEABLE_CLS(StatisticsAsyncOptions)

PLUGIN(StatisticsMultiplexerAsyncThreaded)
