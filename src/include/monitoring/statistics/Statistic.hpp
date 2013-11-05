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

#include <array>

#include <monitoring/statistics/collector/StatisticsProviderDatatypes.hpp>
#include <monitoring/statistics/StatisticsTypes.hpp>

namespace monitoring {
	class StatisticsProviderPlugin;
	class Ontology;

	class StatisticsPluginDescriptionValue : public StatisticsDescription{
		public:
			const StatisticsValue & curValue;

			StatisticsPluginDescriptionValue(const StatisticsValue & value, OntologyAttributeID attribute, const vector<pair<string, string> > & topology): StatisticsDescription(attribute, topology), curValue(value){};
	};

	class Statistic : public StatisticsPluginDescriptionValue {
		public:
			const static size_t kHistorySize = 10;

			//const StatisticsProviderPlugin* const provider;

			Statistic( const StatisticsValue & value, const OntologyAttributeID attribute, const vector<pair<string, string> > & topology ) throw();

			void requestReduction( StatisticsReduceOperator reductionOp ) throw();	///< Tell the Statistic object that it should calculate the given reductionOp. Each call must be matched with a call to cancelReductionRequest().
			void cancelReductionRequest( StatisticsReduceOperator reductionOp ) throw();	///< Tell the Statistic object that the caller does not need the given reductionOp anymore.
			void getHistoricValues( StatisticsReduceOperator reductionOp, StatisticsInterval interval, std::array<StatisticsValue, kHistorySize>* values, std::array<Timestamp, kHistorySize>* times ) throw();	///<Both values and times may be null pointers, if that information is irrelevant. Behaviour is undefined if the caller did not first request the given reductionOp. Also, within the first 100 minutes after a reduction request, the history is not necessarily filled with valid values; it is up to the caller to gracefully handle this situation.
			StatisticsValue getRollingValue( StatisticsReduceOperator reductionOp, StatisticsInterval interval ) throw();	///< As with getHistoricValues(), the reductionOp must be requested.
			StatisticsValue getReducedValue( StatisticsReduceOperator reductionOp, StatisticsInterval interval ) throw();	///< As with getHistoricValues(), the reductionOp must be requested.

			Timestamp curTimestamp();

			void update(Timestamp time) throw();	//The StatisticsCollector is expected to call this ten times per second.
			static size_t measurementIncrement( StatisticsInterval pollInterval ) throw();

		private:
			size_t lastIndex;
			size_t requestCounts[STATISTICS_REDUCE_OPERATOR_COUNT];	//Keeps track of how many objects have requested each reduction operator.
			StatisticsValue history[STATISTICS_REDUCE_OPERATOR_COUNT][INTERVALLS_NUMBER][kHistorySize + 1];
			Timestamp times[STATISTICS_REDUCE_OPERATOR_COUNT][INTERVALLS_NUMBER][kHistorySize + 1];
			Timestamp lastTimestamp;

			StatisticsValue inferValue( StatisticsReduceOperator reductionOp, StatisticsInterval interval, size_t sourceIndex ) const throw();	//Aggregates the values of (interval-1) up to the value at sourceIndex. Source index is given in terms of (interval-1).

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
