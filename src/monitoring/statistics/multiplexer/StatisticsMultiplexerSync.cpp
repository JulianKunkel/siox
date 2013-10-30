/**
 * @file StatisticsMultiplexerSync.cpp
 *
 * @author Nathanael Hübbe
 * @date 2013
 */

#include <monitoring/datatypes/StatisticsTypes.hpp>
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

			virtual void newDataAvailable( const std::vector<std::shared_ptr<Statistic> >& statistics ) throw();
			virtual void registerListener( StatisticsMultiplexerListener * listener ) throw();
			virtual void unregisterListener( StatisticsMultiplexerListener * listener ) throw();

			virtual ~StatisticsMultiplexerSync() throw();

		private:
			class ListenerData {
				public:
					StatisticsMultiplexerListener* listener;
					//When the listener is registered, both requests and statistics are just set to NULL.
					//The next time new data is available, the listener is asked for its requests, a statistics vector is allocated, and statistics that match the listeners requests are entered.
					//As long as there are requests that cannot be satisfied, requests remains set, and each time new data becomes available, the multiplexer tries to find matches for the outstanding requests.
					//Once all requests are satisfied (no remaining null pointers in statistics), requests is set to NULL so that no further searching needs to be done.
					//const std::vector<std::pair<OntologyAttributeID, Topology::ObjectId> >* requests;
					const std::vector<std::pair<OntologyAttributeID, std::vector< std::pair< std::string, std::string> > > >* requests;
					std::vector<std::shared_ptr<Statistic> >* statistics;
					ListenerData( StatisticsMultiplexerListener* listener ) : listener( listener ), requests( 0 ), statistics( 0 ) {};
					~ListenerData() {
						if(statistics) delete statistics;
					}
			};
			std::vector<ListenerData> listeners;	//When a listener is unregistered, its ListenerData is _not_ destroyed since its statistics vector may still be in use. Instead, its listener field is set to NULL so that it may be easily skipped.
			boost::shared_mutex listenersLock;
	};

	ComponentOptions* StatisticsMultiplexerSync::AvailableOptions() {
		return new StatisticsMultiplexerSyncOptions();
	}

	void StatisticsMultiplexerSync::newDataAvailable( const std::vector<std::shared_ptr<Statistic> >& statistics ) throw() {
		listenersLock.lock_shared();
		for( size_t i = listeners.size(); i--; ) {
			StatisticsMultiplexerListener*& curListener = listeners[i].listener;
			//const std::vector<std::pair<OntologyAttributeID, Topology::ObjectId> >*& curRequests = listeners[i].requests;
			const std::vector<std::pair<OntologyAttributeID, std::vector< std::pair< std::string, std::string> > > >*& curRequests = listeners[i].requests;
			std::vector<std::shared_ptr<Statistic> >*& curStatistics = listeners[i].statistics;

			if( !curListener ) continue;
			//get requests from listener if we didn't do that already
			if( !curStatistics ) {
				curRequests = &curListener->requiredMetrics();
				curStatistics = new std::vector<std::shared_ptr<Statistic> >( curRequests->size() );
			}
			//check for statistics matching outstanding requests
			if( curRequests ) {
				bool outstandingRequests = false;
				for( size_t i = curStatistics->size(); i--; ) {
					if( (*curStatistics)[i] ) continue;
					for( size_t j = statistics.size(); j--; ) {
						#define matches(...) ( assert( 0 && "TODO" ), 0 )
						if( matches( statistics[j], (*curRequests)[i] ) ) (*curStatistics)[i] = statistics[j];
					}
					if( !(*curStatistics)[i] ) outstandingRequests = true;
				}
				if( !outstandingRequests ) curRequests = NULL;
			}
			//notify listener
			curListener->notify( *curStatistics );
		}
		listenersLock.unlock_shared();
	}

	void StatisticsMultiplexerSync::registerListener( StatisticsMultiplexerListener * listener ) throw() {
		listenersLock.lock();
		for( size_t i = listeners.size(); i--; ) if( listeners[i].listener == listener ) goto doUnlock;
		listeners.emplace_back( listener );
	doUnlock:
		listenersLock.unlock();
	}

	void StatisticsMultiplexerSync::unregisterListener( StatisticsMultiplexerListener * listener ) throw() {
		listenersLock.lock();
		for( size_t i = listeners.size(); i--; ) if( listeners[i].listener == listener ) listeners[i].listener = NULL;
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
