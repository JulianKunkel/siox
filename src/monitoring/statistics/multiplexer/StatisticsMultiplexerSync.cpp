/**
 * @file StatisticsMultiplexerSync.cpp
 *
 * @author Nathanael Hübbe
 * @date 2013
 */

#include <monitoring/statistics/StatisticsTypes.hpp>
#include <monitoring/statistics/multiplexer/StatisticsMultiplexerImplementation.hpp>
#include <monitoring/statistics/multiplexer/StatisticsMultiplexerListener.hpp>
#include "StatisticsMultiplexerSyncOptions.hpp"

#include <boost/thread/shared_mutex.hpp>

using namespace core;

namespace monitoring {

	/**
	 * StatisticsMultiplexer
	 * Notify listeners about new statistics data.
	 */
	class StatisticsMultiplexerSync : public StatisticsMultiplexer {
		public:
			virtual void init() {};
			virtual ComponentOptions* AvailableOptions();

			virtual void notifyAvailableStatisticsChange( const std::vector<std::shared_ptr<Statistic> > & statistics, bool addedStatistics, bool removedStatistics ) throw();
			virtual void newDataAvailable() throw();
			virtual void registerListener( StatisticsMultiplexerListener * listener ) throw();
			virtual void unregisterListener( StatisticsMultiplexerListener * listener ) throw();

			virtual ~StatisticsMultiplexerSync() throw();

		private:
			class ListenerData {
				public:
					StatisticsMultiplexerListener* listener;
					ListenerData( StatisticsMultiplexerListener* listener ) : listener( listener ) {}
					~ListenerData() {}
			};
			std::vector<ListenerData> listeners;	//protected by listenersLock
			const std::vector<std::shared_ptr<Statistic> > * lastStatistics;	//protected by listenersLock
			boost::shared_mutex listenersLock;
	};

	ComponentOptions* StatisticsMultiplexerSync::AvailableOptions() {
		return new StatisticsMultiplexerSyncOptions();
	}

	void StatisticsMultiplexerSync::notifyAvailableStatisticsChange( const std::vector<std::shared_ptr<Statistic> > & statistics, bool addedStatistics, bool removedStatistics ) throw() {
		listenersLock.lock_shared();
		for( size_t i = listeners.size(); i--; ) {
			ListenerData& cur = listeners[i];
			cur.listener->notifyAvailableStatisticsChange( statistics, addedStatistics, removedStatistics );
		}
		lastStatistics = & statistics;
		listenersLock.unlock_shared();
	}

	void StatisticsMultiplexerSync::newDataAvailable() throw() {
		listenersLock.lock_shared();
		for( size_t i = listeners.size(); i--; ) {
			ListenerData& cur = listeners[i];
			cur.listener->newDataAvailable();
		}
		listenersLock.unlock_shared();
	}

	void StatisticsMultiplexerSync::registerListener( StatisticsMultiplexerListener * listener ) throw() {
		listenersLock.lock();
		for( size_t i = listeners.size(); i--; ) if( listeners[i].listener == listener ) goto doUnlock;
		listeners.emplace_back( listener );
		if( lastStatistics != nullptr ){
			listener->notifyAvailableStatisticsChange( *lastStatistics, true, false );
		}
	doUnlock:
		listenersLock.unlock();
	}

	void StatisticsMultiplexerSync::unregisterListener( StatisticsMultiplexerListener * listener ) throw() {
		listenersLock.lock();
		for( size_t i = listeners.size(); i--; ) {
			if( listeners[i].listener == listener ) {
				listeners.erase(listeners.begin() + i);
			}
		}
		listenersLock.unlock();
	}

	StatisticsMultiplexerSync::~StatisticsMultiplexerSync() throw() {
		if( !listenersLock.try_lock() ) {
			assert( 0 && "Someone tried to destruct a StatisticsMultiplexerSync while another thread is still using it!" ), abort();
		}
	}


}


//CREATE_SERIALIZEABLE_CLS(StatisticsSyncOptions)

extern "C" {
	void * MONITORING_STATISTICS_MULTIPLEXER_INSTANCIATOR_NAME()
	{
		return new monitoring::StatisticsMultiplexerSync();
	}
}
