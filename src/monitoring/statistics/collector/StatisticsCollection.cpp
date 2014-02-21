/**
 * @file StatisticsCollection.cpp
 *
 * @date 2014
 * @author Nathanael Hübbe
 */

#include <monitoring/statistics/StatisticsCollection.hpp>

using namespace monitoring;
using namespace std;

StatisticsCollection* StatisticsCollection::makeCollection( StatisticsCollector* collector, const std::vector<std::pair<std::string, std::string> >& ontologyAttributeTopologyPathPairs, bool threadSafe ) {
	size_t statisticsCount = ontologyAttributeTopologyPathPairs.size();
	shared_ptr<Statistic> statisticsArray[statisticsCount];
	for( size_t i = statisticsCount; i--; ) {
		const pair<string, string>& curDescription = ontologyAttributeTopologyPathPairs[i];
		if( !( statisticsArray[i] = collector->getStatistic( curDescription.first, curDescription.second ) ) ) {
			return NULL;	//failing to lookup one statistic is fatal
		}
	}
	return new StatisticsCollection( collector, statisticsCount, statisticsArray, threadSafe );
}

StatisticsCollection::StatisticsCollection( StatisticsCollector* collector, size_t statisticsCount, shared_ptr<Statistic>* statisticsArray, bool threadSafe = true ) :
	collector(collector),
	statisticsCount(statisticsCount),
	statistics( new shared_ptr<Statistic>[statisticsCount] ),
	communicationBuffer( ( threadSafe ) ? new StatisticsValue[statisticsCount] : NULL ),
	readBuffer( ( threadSafe ) ? new StatisticsValue[statisticsCount] : NULL )
{
	if( threadSafe ) {
		for( size_t i = statisticsCount; i--; ) {
			readBuffer[i] = communicationBuffer[i] = ( statistics[i] = statisticsArray[i] )->curValue;
		}
	}
	collector->registerCollection( this );
}

void StatisticsCollection::pushValues() {
	if( !communicationBuffer ) return;
	communicationsLock.lock();
	for( size_t i = statisticsCount; i--; ) communicationBuffer[i] = statistics[i]->curValue;
	communicationsLock.unlock();
}

void StatisticsCollection::fetchValues() {
	if( !communicationBuffer ) return;
	communicationsLock.lock();
	for( size_t i = statisticsCount; i--; ) readBuffer[i] = communicationBuffer[i];
	communicationsLock.unlock();
}

StatisticsValue& StatisticsCollection::operator[]( size_t index ) {
	return readBuffer[i];
}

StatisticsCollection::~StatisticsCollection() {
	collector->unregisterCollection( this );
	delete[] statistics;
	delete[] communicationBuffer;
	delete[] readBuffer;
}
