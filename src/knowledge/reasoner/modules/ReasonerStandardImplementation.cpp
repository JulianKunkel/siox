#include <unistd.h>

#include <list>
#include <map>
#include <algorithm>
#include <set>
#include <thread>
#include <mutex>
#include <condition_variable>

#include <core/component/Component.hpp>
#include <core/comm/CommunicationModule.hpp>

#include <knowledge/reasoner/ReasonerImplementation.hpp>

#include "ReasonerStandardImplementationOptions.hpp"

using namespace std;
using namespace core;

namespace knowledge {

/*
 Data is pushed into a set of recent PerformanceIssues.
 A background thread fetches these issues and abstracts/refines them into knownIssues.
 */
class ReasonerStandardImplementation : public Reasoner, ServerCallback, MessageCallback, ConnectionCallback {
	private:
		list<AnomalyTrigger *> triggers;
		list<AnomalyPlugin *>  adpis;
		list<Reasoner *>  reasoners; // remote reasoners

		QualitativeUtilization * utilization = nullptr;


		set<PerformanceIssue>   knownIssues;
		set<PerformanceIssue>   recentIssues;


		mutex                   localIssues_lock;
		// the position in the map depends on the plugin
		map<Reasoner *, set<PerformanceIssue>> localIssues;

		// for each host we store the latest observation
		map<string, pair<Timestamp, set<PerformanceIssue>> > remoteIssues;


		thread                  periodicThread;
		mutex                   recentIssues_lock;
		condition_variable      running_condition;

		ServiceServer * server = nullptr;
		vector<ServiceClient*> upstreamReasoners;

		bool terminated = false;

		uint32_t update_intervall_ms = -1;
	protected:
		ComponentOptions * AvailableOptions() {
			return new ReasonerStandardImplementationOptions();
		}

		virtual void PeriodicRun() {
			while( ! terminated ) {
				//cout << "PeriodicRun" << terminated << endl;
				auto timeout = chrono::system_clock::now() + chrono::milliseconds( update_intervall_ms );

				set<AnomalyPluginObservation> adpiObservations;
				// query recent observations of all connected plugins and integrate them into the local performance issues.
				for( auto itr = adpis.begin(); itr != adpis.end() ; itr++ ) {
					// memory management is going in our responsibility.
					unique_ptr<set<AnomalyPluginObservation>> apo = ( *itr )->queryRecentObservations();

					// Merge this set of the adpi into the global list.
					mergeObservations( adpiObservations, *apo );
					//cout << adpiObservations.size() << " " << apo->size() << endl;
				}

				// Merge remote issues:
				set<PerformanceIssue> localIssues;
				for( auto itr = this->localIssues.begin(); itr != this->localIssues.end() ; itr++ ) {
					mergeIssues( localIssues, itr->second );
				}

				// Think:
				// Update recent performance issues that are stored locally.
				// Use significiant information as stored in the observations.
				bool anomalyDetected = false;
				// TODO

				// Determine local performance issues based on recent observations and remote issues.
				// TODO

				// For testing:
				if( adpiObservations.size() > 0 ) {
					anomalyDetected = true;
				}
				if( utilization != nullptr ) {
//						StatisticObservation so = utilization->lastObservation( 4711 );
				}

				// Save recentIssues
				// TODO

				// Update knownIssues based on recentIssues
				// TODO

				// Trigger anomaly if any:
				if( anomalyDetected ) {
					for( auto itr = triggers.begin(); itr != triggers.end() ; itr++ ) {
						( *itr )->triggerResponseForAnomaly();
					}
				}

				// Forward detected performance issues to reasoners connected locally.
				for( auto itr = reasoners.begin(); itr != reasoners.end() ; itr++ ) {
					( *itr )->reportRecentIssues( this, recentIssues );
				}

				// Forward detected performance issues to remote reasoners
				for( auto itr = upstreamReasoners.begin(); itr != upstreamReasoners.end(); itr++ ){
					if( (*itr)->isConnected() ){
						// send the current status to the client.
						// itr->isend();
					}
				}

				unique_lock<mutex> lock( recentIssues_lock );
				if( terminated ) {
					break;
				}
				running_condition.wait_until( lock, timeout );
			}
			//cout << "PeriodicRun finished" << endl;
		}

		void mergeObservations( set<AnomalyPluginObservation> & issues, const set<AnomalyPluginObservation> & newIssues ) {
			for( auto itr = newIssues.begin(); itr != newIssues.end() ; itr++ ) {
				set<AnomalyPluginObservation>::iterator find = issues.find( *itr );
				const AnomalyPluginObservation & nIssue = *itr;
				// should be the default case.
				if( find == issues.end() ) {
					// append the item
					issues.insert( nIssue );
				} else {
					AnomalyPluginObservation & existingIssue = ( AnomalyPluginObservation & )( *find );
					existingIssue.occurences += nIssue.occurences;
					existingIssue.delta_time_ms += nIssue.delta_time_ms;
				}
			}
		}

		void mergeIssues( set<PerformanceIssue> & issues, const set<PerformanceIssue> & newIssues ) {
			for( auto itr = newIssues.begin(); itr != newIssues.end() ; itr++ ) {
				set<PerformanceIssue>::iterator find = issues.find( *itr );
				const PerformanceIssue & nIssue = *itr;
				// should be the default case.
				if( find == newIssues.end() ) {
					// append the item
					issues.insert( nIssue );
				} else {
					PerformanceIssue & existingIssue = ( PerformanceIssue & )( *find );
					existingIssue.occurences += nIssue.occurences;
					existingIssue.delta_time_ms += nIssue.delta_time_ms;
				}
			}
		}


	// Server implementation
	virtual void messageReceivedCB(std::shared_ptr<ServerClientMessage> msg, const char * message_data, uint64_t buffer_size){
		//shared_ptr<Activity> activity ( new Activity() );

		//uint64_t pos = 0;
		//j_serialization::deserialize(*activity, message_data, pos, buffer_size);
		msg->isendResponse(nullptr);
	}

	virtual uint64_t serializeResponseMessageLen(const ServerClientMessage * msg, const void * responseType){
		return 0;
	}

	virtual void serializeResponseMessage(const ServerClientMessage * msg, const void * responseType, char * buffer, uint64_t & pos){
	}

	// Client implementation

	virtual void messageSendCB(BareMessage * msg){ /* do nothing */ }

	virtual void messageResponseCB(BareMessage * msg, char * buffer, uint64_t buffer_size){
	}

	virtual void messageTransferErrorCB(BareMessage * msg, CommunicationError error){ 

	}

	virtual uint64_t serializeMessageLen(const void * msgObject) {
		return 0; //j_serialization::serializeLen(* (Activity*) msgObject);
	}

	virtual void serializeMessage(const void * msgObject, char * buffer, uint64_t & pos){
		//j_serialization::serialize(* (Activity*) msgObject, buffer, pos);
	}

	// client connection callback
	virtual void connectionErrorCB(ServiceClient & client, CommunicationError error){
		// remove currently pending messages
		client.dropPendingMessages();
		sleep(1);
		// try to reconnect periodically
		client.ireconnect();
	}
	
	virtual void connectionSuccessfullCB(ServiceClient & client){}

public:

	~ReasonerStandardImplementation() {
		recentIssues_lock.lock();
		terminated = true;
		running_condition.notify_one();
		recentIssues_lock.unlock();

		periodicThread.join();

		if (server){			
			delete(server);
		}

		// shutdown all client connections
		for( auto itr = upstreamReasoners.begin(); itr != upstreamReasoners.end(); itr++ ){
			delete(*itr);
		}		
	}

	virtual void init() {
		ReasonerStandardImplementationOptions & options = getOptions<ReasonerStandardImplementationOptions>();

		update_intervall_ms = options.update_intervall_ms;

		CommunicationModule * comm =  GET_INSTANCE(CommunicationModule, options.comm);
		assert(comm != nullptr);

		// spawn the server upon which we receive the remote status of different nodes.
		server = comm->startServerService(options.serviceAddress, this);


		upstreamReasoners.reserve(options.remoteReasoners.size());
		// try to connect to all clients
		for(auto itr = options.remoteReasoners.begin(); itr != options.remoteReasoners.end(); itr++){
			ServiceClient * client = comm->startClientService( *itr, this, this );
			upstreamReasoners.push_back( client );
		}

		periodicThread = thread( & ReasonerStandardImplementation::PeriodicRun, this );
	}

	virtual void reportRecentIssues( const Reasoner * reasoner, const set<PerformanceIssue> & issues ) {
		unique_lock<mutex> lock( localIssues_lock );

		// Replace last reported issues in the remote issues set.
		localIssues[( Reasoner * ) reasoner] = issues;
	}

	virtual unique_ptr<list<PerformanceIssue>> queryRecentPerformanceIssues() {
		auto stats = new list<PerformanceIssue>();
		for( auto itr = recentIssues.begin(); itr != recentIssues.end() ; itr++ ) {
			stats->push_back( *itr );
		}
		return unique_ptr<list<PerformanceIssue> >( stats );
	}

	virtual unique_ptr<list<PerformanceIssue>> queryRuntimePerformanceIssues() {
		auto stats = new list<PerformanceIssue>();
		for( auto itr = knownIssues.begin(); itr != knownIssues.end() ; itr++ ) {
			stats->push_back( *itr );
		}
		return unique_ptr<list<PerformanceIssue>>( stats );
	}

	virtual void connectAnomalyPlugin( AnomalyPlugin * plugin ) {
		assert( std::find( adpis.begin(), adpis.end(), plugin ) ==  adpis.end() );
		adpis.push_back( plugin );
	}

	virtual void connectUtilization( QualitativeUtilization * plugin ) {
		assert( utilization == nullptr );
		utilization = plugin;
	}

	virtual void connectTrigger( AnomalyTrigger * trigger ) {
		// element should not be part of the list so far.
		assert( std::find( triggers.begin(), triggers.end(), trigger ) ==  triggers.end() );
		triggers.push_back( trigger );
	}

	virtual void connectReasoner( Reasoner * reasoner ) {
		assert( std::find( reasoners.begin(), reasoners.end(), reasoner ) ==  reasoners.end() );
		reasoners.push_back( reasoner );
		localIssues[reasoner] = set<PerformanceIssue>();
	}

};

} // namespace knowledge

extern "C" {
	void * KNOWLEDGE_REASONER_INSTANCIATOR_NAME()
	{
		return new knowledge::ReasonerStandardImplementation();
	}
}
