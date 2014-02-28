/**
 * @file StatisticsCollection.cpp
 *
 * @date 2014
 * @author Nathanael Hübbe
 */

#include <monitoring/statistics/StatisticsCollection.hpp>
#include <monitoring/statistics/collector/StatisticsCollector.hpp>

using namespace monitoring;
using namespace std;

StatisticsCollection* StatisticsCollection::makeCollection( StatisticsCollector* collector, const std::vector<std::pair<std::string, std::string> >& ontologyAttributeTopologyPathPairs, bool threadSafe ) throw() {
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

StatisticsCollection::StatisticsCollection( StatisticsCollector* collector, size_t statisticsCount, shared_ptr<Statistic>* statisticsArray, bool threadSafe ) throw() :
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

void StatisticsCollection::pushValues() throw() {
	if( !communicationBuffer ) return;
	communicationLock.lock();
	for( size_t i = statisticsCount; i--; ) communicationBuffer[i] = statistics[i]->curValue;
	communicationLock.unlock();
}

void StatisticsCollection::fetchValues() throw() {
	if( !communicationBuffer ) return;
	communicationLock.lock();
	for( size_t i = statisticsCount; i--; ) readBuffer[i] = communicationBuffer[i];
	communicationLock.unlock();
}

StatisticsValue& StatisticsCollection::operator[]( size_t index ) throw() {
	return readBuffer[index];
}

StatisticsCollection::~StatisticsCollection() throw() {
	collector->unregisterCollection( this );
	delete[] statistics;
	delete[] communicationBuffer;
	delete[] readBuffer;
}
