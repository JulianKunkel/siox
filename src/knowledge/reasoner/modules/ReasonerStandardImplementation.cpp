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

#include <knowledge/reasoner/modules/ReasonerStandardImplementationOptions.hpp>
#include <knowledge/reasoner/modules/ReasoningDatatypesSerializable.hpp>

using namespace std;
using namespace core;

namespace knowledge {

/*
 Data is pushed into a set of recent PerformanceIssues.
 A background thread fetches these issues and abstracts/refines them into knownIssues.

 The reasoner deduces the local state from local issues taking remote issues into account.
 The deduced information is communicated with remote reasoners.
 There are four roles:
 - process: this reasoner will only aggregate information for the process and forward them.
 - node: this reasoner will aggreagte all informations from process reasoners. It forwards the node state upstream and the global information downstream.
 - system: this reasoner will aggreate all node informations and deduct the system state e.g. for file-systems and forward this information to all connected reasoners.

 */
class ReasonerStandardImplementation : public Reasoner, ServerCallback, MessageCallback, ConnectionCallback {
	private:
		list<AnomalyTrigger *> triggers;
		list<AnomalyPlugin *>  adpis;
		list<Reasoner *>  reasoners; // remote reasoners

		QualitativeUtilization * utilization = nullptr;


		shared_ptr<HealthStatistics>   runtimeStatistics;


		// for each host (by ID) we store the latest observation
		unordered_map<string, pair<Timestamp, set<HealthStatistics>> > remoteIssues;


		thread                  periodicThread;
		mutex                   recentIssues_lock;
		condition_variable      running_condition;

		ServiceServer * server = nullptr;
		vector<ServiceClient*> downstreamReasoners;
		ServiceClient * upstreamReasoner = nullptr;

		bool terminated = false;

		uint32_t update_intervall_ms = -1;
		
	protected:
		ComponentOptions * AvailableOptions() {
			return new ReasonerStandardImplementationOptions();
		}

		virtual void PeriodicRun() {
			bool persistingAnomaly = false;
			while( ! terminated ) {
				//cout << "PeriodicRun" << terminated << endl;
				auto timeout = chrono::system_clock::now() + chrono::milliseconds( update_intervall_ms );

				unique_ptr<unordered_map<ComponentID, AnomalyPluginHealthStatistic>>  adpiObservations;
				// query recent observations of all connected plugins and integrate them into the local performance issues.
				for( auto itr = adpis.begin(); itr != adpis.end() ; itr++ ) {
					// memory management is going in our responsibility.
					unique_ptr<unordered_map<ComponentID, AnomalyPluginHealthStatistic>>  apo = ( *itr )->queryRecentObservations();

					// Merge this set of the adpi into the global list.
					//mergeObservations( adpiObservations, *apo );
					//cout << adpiObservations.size() << " " << apo->size() << endl;
				}

				// Think:
				// Update recent performance issues that are stored locally.
				// Use significiant information as stored in the observations.
				bool anomalyDetected = false;
				// TODO

				// Determine local performance issues based on recent observations and remote issues.
				// TODO

				// For testing:
				if( adpiObservations->size() > 0 ) {
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
				if( anomalyDetected && ! persistingAnomaly ) {
					for( auto itr = triggers.begin(); itr != triggers.end() ; itr++ ) {
						( *itr )->triggerResponseForAnomaly(false);
					}
				}
				// TODO sometimes we'd like to have persistent Anomalies ... persistingAnomaly = anomalyDetected;

				// Forward detected performance issues to remote reasoners
				for( auto itr = downstreamReasoners.begin(); itr != downstreamReasoners.end(); itr++ ){
					if( (*itr)->isConnected() ){
						// send the current status to the client.
						//itr->isend();
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
		for( auto itr = downstreamReasoners.begin(); itr != downstreamReasoners.end(); itr++ ){
			delete(*itr);
		}		
		if ( upstreamReasoner != nullptr ){
			delete(upstreamReasoner);
		}
	}

	virtual void init() {
		ReasonerStandardImplementationOptions & options = getOptions<ReasonerStandardImplementationOptions>();

		update_intervall_ms = options.update_intervall_ms;

		CommunicationModule * comm =  GET_INSTANCE(CommunicationModule, options.comm);
		assert(comm != nullptr);

		// spawn the server upon which we receive the remote status of different nodes.
		server = comm->startServerService(options.serviceAddress, this);


		downstreamReasoners.reserve(options.downstreamReasoners.size());
		// try to connect to all clients
		for(auto itr = options.downstreamReasoners.begin(); itr != options.downstreamReasoners.end(); itr++){
			ServiceClient * client = comm->startClientService( *itr, this, this );
			downstreamReasoners.push_back( client );
		}

		if ( options.upstreamReasoner != "" ){
			upstreamReasoner = comm->startClientService( options.upstreamReasoner, this, this );
		}

		periodicThread = thread( & ReasonerStandardImplementation::PeriodicRun, this );
	}

	virtual shared_ptr<HealthStatistics> queryRuntimePerformanceIssues() {
		return nullptr;
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
};

} // namespace knowledge

extern "C" {
	void * KNOWLEDGE_REASONER_INSTANCIATOR_NAME()
	{
		return new knowledge::ReasonerStandardImplementation();
	}
}
