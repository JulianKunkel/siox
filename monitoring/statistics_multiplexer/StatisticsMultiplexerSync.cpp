

#include <monitoring/datatypes/StatisticsTypes.hpp>
#include <monitoring/statistics_multiplexer/StatisticsMultiplexerImplementation.hpp>
#include <templates/multiplexer/MultiplexerFilteredImpl.hpp>
#include <monitoring/statistics_multiplexer/StatisticsMultiplexerListener.hpp>

using namespace core;
using namespace monitoring;

namespace monitoring {

	/**
	 * ActivityMultiplexer
	 * Forwards logged activities to registered listeners (e.g. Plugins) either
	 * in an syncronised or asyncronous manner.
	 */
//class StatisticsMultiplexerSync : public MultiplexerFilteredTemplate<Statistics>
	class StatisticsMultiplexerSync : public MultiplexerFilteredTemplate<StatisticsValue, StatisticsMultiplexerListener> {

	};

}


//CREATE_SERIALIZEABLE_CLS(StatisticsSyncOptions)

//extern "C"{
//	void * MONITORING_STATISTICS_MULTIPLEXER_INSTANCIATOR_NAME() {
//		return new StatisticsMultiplexerSync();
//	}
//}
