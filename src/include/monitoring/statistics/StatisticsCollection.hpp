/**
 * @file StatisticsCollection.hpp
 *
 * StatisticsCollections can be used to get a consistent view of the values of Statistics provided by different plugins.
 * It works largely like an array of a fixed size.
 * If the StatisticsCollection is created as threadSafe, calls to pushValues() and fetchValues() are serialized.
 * However, if the consumer is known to live in the same thread as the producer, the locking overhead can be avoided by passing false to the threadSafe parameter.
 *
 * @author Nathanael Hübbe
 * @date 2014
 */

#ifndef INCLUDE_GUARD_MONITORING_STATISTICS_STATISTICS_COLLECTION
#define INCLUDE_GUARD_MONITORING_STATISTICS_STATISTICS_COLLECTION

#include <vector>
#include <utility>
#include <string>
#include <mutex>
#include <shared_ptr>

namespace monitoring {
	class Statistic;
	class StatisticsCollector;

	class StatisticsCollection {
		public:
			static StatisticsCollection* makeCollection( StatisticsCollector* collector, const std::vector<std::pair<std::string, std::string> >& ontologyAttributeTopologyPathPairs, bool threadSafe = true );

			StatisticsCollection( StatisticsCollector* collector, size_t statisticsCount, shared_ptr<Statistic>* statisticsArray, bool threadSafe = true );

			void pushValues();	//To be called by the StatisticsCollector when new values are available. Serializes with fetchValues() to ensure that fetchValues() sees a consistent state.
			void fetchValues();	//To be called by the consumer interested in the values of the statistics. Will copy the current values into the internal storage.
			StatisticsValue& operator[]( size_t index );	//Returns a reference to the copy of the StatisticsValue in the internal storage.

			~StatisticsCollection();

		private:
			StatisticsCollector* collector;
			size_t statisticsCount;
			shared_ptr<Statistic>* statistics;
			StatisticsValue* communicationBuffer, *readBuffer;	//communicationBuffer is protected by communicationsLock
			std::mutex communicationLock;	//protects communicationBuffer (if it exists)
	};
}

#endif
