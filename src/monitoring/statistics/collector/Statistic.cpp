/**
 * @file Statistic.cpp
 *
 * @author Nathanael Hübbe
 * @date   2013
 */

#include <atomic>

#include <monitoring/statistics/Statistic.hpp>
#include <monitoring/ontology/Ontology.hpp>

monitoring::Statistic::Statistic( const StatisticsValue & value, OntologyAttributeID attribute, const vector<pair<string, string> > & topology ) throw() :
		StatisticsPluginDescriptionValue( value, attribute, topology),
	lastIndex( 0 ),
	requestCounts{0}
{}

#define wrapIndex(index) ((index) % (kHistorySize + 1))

Timestamp monitoring::Statistic::curTimestamp(){
	return lastTimestamp;
}

void monitoring::Statistic::requestReduction( StatisticsReduceOperator reductionOp ) throw() {
	assert( reductionOp >= 0 && reductionOp < STATISTICS_REDUCE_OPERATOR_COUNT );
	requestCounts[reductionOp]++;
}

void monitoring::Statistic::cancelReductionRequest( StatisticsReduceOperator reductionOp ) throw() {
	assert( reductionOp >= 0 && reductionOp < STATISTICS_REDUCE_OPERATOR_COUNT );
	assert( requestCounts[reductionOp] );
	requestCounts[reductionOp]--;
}

void monitoring::Statistic::getHistoricValues( StatisticsReduceOperator reductionOp, StatisticsInterval interval, std::array<StatisticsValue, kHistorySize>* values, std::array<Timestamp, kHistorySize>* returnTimes) throw() {
	static_assert( sizeof( history[0][0] )/sizeof( history[0][0][0] ) == kHistorySize + 1, "assumption about the history size is wrong" );
	static_assert( sizeof( times[0][0] )/sizeof( times[0][0][0] ) == kHistorySize + 1, "assumption about the history size is wrong" );
	size_t lastIntervalIndex = lastIndex/measurementIncrement( interval );
	for( size_t i = kHistorySize; i--; ) {
		size_t historyIndex = wrapIndex(lastIntervalIndex + i + 2);	//+2 to skip the value thats reserved for updating
		if( values ) (*values)[i] = history[reductionOp][interval][historyIndex];
		if( returnTimes ) (*returnTimes)[i] = times[reductionOp][interval][historyIndex];
	}
}

monitoring::StatisticsValue monitoring::Statistic::inferValue( StatisticsReduceOperator reductionOp, monitoring::StatisticsInterval interval, size_t sourceIndex ) const throw() {
	assert( interval != HUNDRED_MILLISECONDS && "This condition should be handled in the caller.");
	size_t integrationPeriod = measurementIncrement(interval) / measurementIncrement(interval - 1 );
	StatisticsValue result = curValue;	//set the correct type
	switch( reductionOp ) {
		case MIN:
			result.setMax();
			for( size_t timeOffset = integrationPeriod; timeOffset--; ) {
				if( history[reductionOp][interval - 1][wrapIndex( sourceIndex - timeOffset )] < result) {
					result = history[reductionOp][interval - 1][wrapIndex( sourceIndex - timeOffset )];
				}
			}
			break;
		case MAX:
			result.setMin();
			for( size_t timeOffset = integrationPeriod; timeOffset--; ) {
				if( result < history[reductionOp][interval - 1][wrapIndex( sourceIndex - timeOffset )]) {
					result = history[reductionOp][interval - 1][wrapIndex( sourceIndex - timeOffset )];
				}
			}
			break;
		case AVERAGE:
			result.setZero();
			for( size_t timeOffset = integrationPeriod; timeOffset--; ) {
				result += history[reductionOp][interval - 1][wrapIndex( sourceIndex - timeOffset )];
			}
			result /= integrationPeriod;
			break;
		case SUM:
			result.setZero();
			for( size_t timeOffset = integrationPeriod; timeOffset--; ) {
				result += history[reductionOp][interval - 1][wrapIndex( sourceIndex - timeOffset )];
			}
			break;
		case STATISTICS_REDUCE_OPERATOR_COUNT:
			assert(0), abort();
	}
	return result;
}

monitoring::StatisticsValue monitoring::Statistic::getRollingValue( StatisticsReduceOperator reductionOp, monitoring::StatisticsInterval interval ) throw() {
	return history[reductionOp][interval][wrapIndex( lastIndex/measurementIncrement( interval ) )];
}

monitoring::StatisticsValue monitoring::Statistic::getReducedValue( StatisticsReduceOperator reductionOp, monitoring::StatisticsInterval interval ) throw() {
	if( interval == HUNDRED_MILLISECONDS ) return curValue;
	return inferValue( reductionOp, interval, lastIndex/measurementIncrement( interval - 1 ) );
}

void monitoring::Statistic::update( Timestamp time ) throw() {
	size_t newIndex = lastIndex + 1;
	lastTimestamp = time;
	for(StatisticsReduceOperator reductionOp = STATISTICS_REDUCE_OPERATOR_COUNT; reductionOp--; ) {
		if(!requestCounts[reductionOp]) continue;
		//write the current value into the history
		if( history[reductionOp][HUNDRED_MILLISECONDS][wrapIndex( newIndex )].type() == VariableDatatype::Type::INVALID ) {
			//The history has not been initialized yet, but that is required for performing arithmetic on its values. So do it now.
			for( StatisticsInterval curInterval = INTERVALLS_NUMBER; curInterval--; ) {
				for( size_t i = kHistorySize + 1; i--; ) history[reductionOp][curInterval][i] = curValue;
			}
		}
		history[reductionOp][HUNDRED_MILLISECONDS][wrapIndex( newIndex )] = curValue;
		times[reductionOp][HUNDRED_MILLISECONDS][wrapIndex( newIndex )] = time;
		//now the hard part: calculate the aggregated history values
		size_t previousIncrement = measurementIncrement(HUNDRED_MILLISECONDS), curIncrement;
		for(
			StatisticsInterval curInterval = SECOND;
			curInterval < INTERVALLS_NUMBER && ! ( newIndex % ( curIncrement = measurementIncrement( curInterval ) ) );
			curInterval++
		) {
			history[reductionOp][curInterval][wrapIndex( newIndex/curIncrement )] = inferValue( reductionOp, curInterval, newIndex/previousIncrement );
			previousIncrement = curIncrement;
		}
	}
	std::atomic_thread_fence( std::memory_order_release );
	lastIndex = newIndex;
}
