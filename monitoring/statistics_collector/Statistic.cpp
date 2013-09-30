#include <monitoring/statistics_collector/Statistic.hpp>
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
	size_t previousIncrement = measurementIncrement(HUNDRED_MILLISECONDS), curIncrement = measurementIncrement(SECOND);
	for( size_t curInterval = SECOND; curInterval < INTERVALLS_NUMBER && !( newIndex % curIncrement ); curInterval++ ) {
		size_t sourceIndex = newIndex/previousIncrement;
		StatisticsValue* curResult = &history[curInterval][wrapIndex( newIndex/curIncrement )];
		switch(reductionOp) {
			case MIN:
				curResult->setMax();
				for( size_t timeOffset = 10; timeOffset--; ) {
					if( history[curInterval][wrapIndex( sourceIndex - timeOffset )] < *curResult ) {
						*curResult = history[curInterval][wrapIndex( sourceIndex - timeOffset )];
					}
				}
			break; case MAX:
				curResult->setMin();
				for( size_t timeOffset = 10; timeOffset--; ) {
					if( *curResult < history[curInterval][wrapIndex( sourceIndex - timeOffset )] ) {
						*curResult = history[curInterval][wrapIndex( sourceIndex - timeOffset )];
					}
				}
			break; case COUNT:
				assert(0 && "TODO: What is this reduction op supposed to do? Always return the latest value?"), abort();
			break; case AVERAGE:
				curResult->setZero();
				for( size_t timeOffset = 10; timeOffset--; ) {
					*curResult += history[curInterval][wrapIndex( sourceIndex - timeOffset )];
				}
				*curResult /= 10;
			break; case SUM:
				curResult->setZero();
				for( size_t timeOffset = 10; timeOffset--; ) {
					*curResult += history[curInterval][wrapIndex( sourceIndex - timeOffset )];
				}
			break;
		}
		previousIncrement = curIncrement, curIncrement = measurementIncrement( (StatisticsInterval)curInterval );
	}
	lastIndex = newIndex;
}
