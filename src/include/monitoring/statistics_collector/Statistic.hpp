/**
 * @file Statistic.hpp
 *
 * This class is used to add history information to a StatisticsDescription.
 *
 * @author Nathanael Hübbe
 * @date   2013
 */

#ifndef INCLUDE_GUARD_MONITORING_STATISTIC
#define INCLUDE_GUARD_MONITORING_STATISTIC

#include <chrono>

#include <monitoring/statistics_collector/StatisticsProviderDatatypes.hpp>
#include <monitoring/datatypes/StatisticsTypes.hpp>

namespace monitoring {
	class StatisticsProviderPlugin;
	class Ontology;

	class Statistic : public StatisticsDescription {
		public:
			const static size_t kHistorySize = 10;

			StatisticsValue& curValue;
			const StatisticsProviderPlugin* const provider;

			Statistic( const StatisticsProviderDatatypes& source, const StatisticsProviderPlugin* provider, Ontology* ontology ) throw();

			void getHistoricValues( StatisticsInterval interval, std::array<StatisticsValue, kHistorySize>* values, std::array<std::chrono::high_resolution_clock::time_point, kHistorySize>* times ) throw();	//Both values and times may be null pointers, if that information is irrelevant.
			StatisticsValue getRollingValue( StatisticsInterval interval ) throw();
			StatisticsValue getReducedValue( StatisticsInterval interval ) throw();

			void update(std::chrono::high_resolution_clock::time_point time) throw();	//The StatisticsCollector is expected to call this ten times per second.
			static size_t measurementIncrement( StatisticsInterval pollInterval ) throw();

		private:
			size_t lastIndex;
			StatisticsReduceOperator reductionOp;
			StatisticsValue history[INTERVALLS_NUMBER][kHistorySize + 1];
			std::chrono::high_resolution_clock::time_point times[INTERVALLS_NUMBER][kHistorySize + 1];

			StatisticsValue inferValue( StatisticsInterval interval, size_t sourceIndex ) const throw();	//Aggregates the values of (interval-1) up to the value at sourceIndex. Source index is given in terms of (interval-1).

			Statistic() = delete;
			Statistic(const Statistic&) = delete;
			Statistic& operator=(const Statistic&) = delete;
	};

	inline size_t Statistic::measurementIncrement( StatisticsInterval pollInterval ) throw() {
		static_assert(kHistorySize >= 10, "kHistorySize is not big enough to aggregate the statistics from one interval level to the next");
		switch(pollInterval) {
			case HUNDRED_MILLISECONDS: return 1;
			case SECOND: return 10;
			case TEN_SECONDS: return 100;
			case MINUTE: return 600;
			case TEN_MINUTES: return 6000;
			default: assert(0 && "Unknown poll interval"), abort();
		}
	}
}

#endif
