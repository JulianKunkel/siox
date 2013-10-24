#ifndef STATISTICS_MULTIPLEXER_H
#define STATISTICS_MULTIPLEXER_H

#include <monitoring/datatypes/StatisticsTypes.hpp>
#include <monitoring/statistics_collector/Statistic.hpp>
#include <monitoring/statistics_multiplexer/StatisticsMultiplexerListener.hpp>
#include <core/component/Component.hpp>

namespace monitoring {


	/**
	 * StatisticsMultiplexer
	 * Notifies all listeners when new statistics data becomes available, providing them with a short list of statistics they requested.
	 */


	class StatisticsMultiplexer : public core::Component {
		public:

			virtual void newDataAvailable( const std::vector<std::shared_ptr<Statistic> >& statistics ) throw() = 0;

			/**
			 * Register listener to multiplexer
			 *
			 * @param   listener [in]   listener to notify in the future
			 */
			virtual void registerListener( StatisticsMultiplexerListener * listener ) throw() = 0;

			/**
			 * Unregister listener from multiplexer
			 *
			 * @param   listener [in]   listener to remove
			 */
			virtual void unregisterListener( StatisticsMultiplexerListener * listener ) throw() = 0;

	};


#define STATISTICS_MULTIPLEXER_INTERFACE "monitoring_statisticsmultiplexer"

}
#endif /* STATISTICS_MULTIPLEXER_H */
