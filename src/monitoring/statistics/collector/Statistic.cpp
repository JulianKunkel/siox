/**
 * @file Statistic.cpp
 *
 * @author Nathanael Hübbe
 * @date   2013
 */

#include <atomic>

#include <monitoring/statistics/collector/Statistic.hpp>
#include <monitoring/ontology/Ontology.hpp>

monitoring::Statistic::Statistic( const StatisticsProviderDatatypes& source, const StatisticsProviderPlugin* provider, Ontology* ontology ) throw() :
	StatisticsDescription( ontology->register_attribute( "Statistics", source.metrics, source.value.type() ), source.topology ),
	curValue( source.value ),
	provider( provider ),
	lastIndex( 0 ),
	reductionOp( SUM )
{}

#define wrapIndex(index) ((index) % (kHistorySize + 1))

void monitoring::Statistic::getHistoricValues( StatisticsInterval interval, std::array<StatisticsValue, kHistorySize>* values, std::array<std::chrono::high_resolution_clock::time_point, kHistorySize>* returnTimes) throw() {
	static_assert( sizeof( history[0] )/sizeof( history[0][0] ) == kHistorySize + 1, "assumption about the history size is wrong" );
	static_assert( sizeof( times[0] )/sizeof( times[0][0] ) == kHistorySize + 1, "assumption about the history size is wrong" );
	size_t lastIntervalIndex = lastIndex/measurementIncrement( interval );
	for( size_t i = kHistorySize; i--; ) {
		size_t historyIndex = wrapIndex(lastIntervalIndex + i + 2);	//+2 to skip the value thats reserved for updating
		if( values ) (*values)[i] = history[interval][historyIndex];
		if( returnTimes ) (*returnTimes)[i] = times[interval][historyIndex];
	}
}

monitoring::StatisticsValue monitoring::Statistic::inferValue( monitoring::StatisticsInterval interval, size_t sourceIndex ) const throw() {
	assert( interval != HUNDRED_MILLISECONDS && "This condition should be handled in the caller.");
	size_t integrationPeriod = measurementIncrement(interval) / measurementIncrement((StatisticsInterval)( (size_t)interval - 1 ) );
	StatisticsValue result = curValue;	//set the correct type
	switch( reductionOp ) {
		case MIN:
			result.setMax();
			for( size_t timeOffset = integrationPeriod; timeOffset--; ) {
				if( history[interval - 1][wrapIndex( sourceIndex - timeOffset )] < result) {
					result = history[interval - 1][wrapIndex( sourceIndex - timeOffset )];
				}
			}
			break;
		case MAX:
			result.setMin();
			for( size_t timeOffset = integrationPeriod; timeOffset--; ) {
				if( result < history[interval - 1][wrapIndex( sourceIndex - timeOffset )]) {
					result = history[interval - 1][wrapIndex( sourceIndex - timeOffset )];
				}
			}
			break;
		case COUNT: assert(0 && "TODO: What is this reduction op supposed to do? Always return the latest value?"), abort(); break;
		case AVERAGE:
			result.setZero();
			for( size_t timeOffset = integrationPeriod; timeOffset--; ) {
				result += history[interval - 1][wrapIndex( sourceIndex - timeOffset )];
			}
			result /= integrationPeriod;
			break;
		case SUM:
			result.setZero();
			for( size_t timeOffset = integrationPeriod; timeOffset--; ) {
				result += history[interval - 1][wrapIndex( sourceIndex - timeOffset )];
			}
			break;
	}
	return result;
}

monitoring::StatisticsValue monitoring::Statistic::getRollingValue( monitoring::StatisticsInterval interval ) throw() {
	return history[interval][wrapIndex( lastIndex/measurementIncrement( interval ) )];
}

monitoring::StatisticsValue monitoring::Statistic::getReducedValue( monitoring::StatisticsInterval interval ) throw() {
	if( interval == HUNDRED_MILLISECONDS ) return curValue;
	return inferValue( interval, lastIndex/measurementIncrement( (StatisticsInterval)( (size_t)interval - 1 ) ) );
}

void monitoring::Statistic::update( std::chrono::high_resolution_clock::time_point time ) throw() {
	//write the current value into the history
	size_t newIndex = lastIndex + 1;
	if( history[HUNDRED_MILLISECONDS][wrapIndex( newIndex )].type() == VariableDatatype::Type::INVALID ) {
		//The history has not been initialized yet, but that is required for performing arithmetic on its values. So do it now.
		for( size_t curInterval = INTERVALLS_NUMBER; curInterval--; ) {
			for( size_t i = kHistorySize + 1; i--; ) history[curInterval][i] = curValue;
		}
	}
	history[HUNDRED_MILLISECONDS][wrapIndex( newIndex )] = curValue;
	times[HUNDRED_MILLISECONDS][wrapIndex( newIndex )] = time;
	//now the hard part: calculate the aggregated history values
	size_t previousIncrement = measurementIncrement(HUNDRED_MILLISECONDS), curIncrement;
	for(
		size_t curInterval = SECOND;
		curInterval < INTERVALLS_NUMBER && ! ( newIndex % ( curIncrement = measurementIncrement( (StatisticsInterval)curInterval ) ) );
		curInterval++
	) {
		history[curInterval][wrapIndex( newIndex/curIncrement )] = inferValue( (StatisticsInterval)curInterval, newIndex/previousIncrement );
		previousIncrement = curIncrement;
	}
	std::atomic_thread_fence( std::memory_order_release );
	lastIndex = newIndex;
}
