#ifndef STATISTICS_MULTIPLEXER_H
#define STATISTICS_MULTIPLEXER_H 

#include <monitoring/datatypes/Statistics.hpp>
#include <monitoring/statistics_multiplexer/StatisticsMultiplexerListener.hpp>
#include <core/component/Component.hpp>

namespace monitoring{


/**
 * StatisticsMultiplexer
 * Forwards and filters statistics to registered listeners (e.g. Plugins) either
 * in an syncronised or asyncronous manner.
 */


class StatisticsMultiplexer : public core::Component{
public:

	virtual void Log(Statistics * element) = 0;

	/**
	 * Register listener to multiplexer
	 *
	 * @param	listener [in]	listener to notify in the future
	 */
	virtual void registerListener(StatisticsMultiplexerListener * listener) = 0;

	/**
	 * Unregister listener from multiplexer
	 *
	 * @param	listener [in]	listener to remove
	 */
	virtual void unregisterListener(StatisticsMultiplexerListener * listener) = 0;
	
};


#define STATISTICS_MULTIPLEXER_INTERFACE "monitoring_statisticsmultiplexer"

}
#endif /* STATISTICS_MULTIPLEXER_H */
