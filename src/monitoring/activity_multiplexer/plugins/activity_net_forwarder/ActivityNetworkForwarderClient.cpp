#include <unistd.h>

#include <mutex>
#include <atomic>

#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginImplementation.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexer.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerListener.hpp>


#include <core/comm/CommunicationModule.hpp>
#include <monitoring/activity_multiplexer/plugins/activity_net_forwarder/ActivityBinarySerializable.hpp>

#include <knowledge/reasoner/AnomalyTrigger.hpp>
#include <knowledge/reasoner/Reasoner.hpp>
#include <core/reporting/ComponentReportInterface.hpp>
	
#include "ActivityNetworkForwarderOptions.hpp"

using namespace core;
using namespace monitoring;
using namespace knowledge;

#define max_pending_ops 1000

class ReconnectionCallback : public ConnectionCallback{
public:
	atomic<uint32_t> connectionErrors;

	ReconnectionCallback(){
		connectionErrors = 0;
	}

	void connectionErrorCB(ServiceClient & connection, CommunicationError error){
		connectionErrors++;		
		sleep( 1 );

		connection.ireconnect();
	}

	void connectionSuccessfullCB(ServiceClient & connection){
	}
};

/**
 * Forward an activity from one ActivityMultiplexer to another.
 * Data is only forwarded if this client is triggered.
 */
class ActivityNetworkForwarderClient: public ActivityMultiplexerPlugin, public MessageCallback, public AnomalyTrigger, public ComponentReportInterface{
private:
	// the ringBuffer
	vector<shared_ptr<Activity>> ringBuffer;
	uint curPos = 0;
	uint sendPos = 0;
	uint ringBufferSize;

	bool forwardAllActivities;
	bool anomalyStatus = false;

	mutex ringBuffMutex;

	atomic<uint32_t> pendingActivities;
	atomic<uint64_t> droppedActivities;
	atomic<uint64_t> droppedActivitiesDuringAnomaly;

	uint64_t activitiesSendCount = 0;
	uint64_t activitiesReceptionConfirmed = 0;
	uint64_t activitiesErrorCount = 0;

public:

	ComponentReport prepareReport(){
		ComponentReport report;

		report.addEntry( new GroupEntry( "activitiesSend" ), ReportEntry( ReportEntry::Type::APPLICATION_INFO, VariableDatatype( activitiesSendCount ) ));
		report.addEntry( new GroupEntry( "activitiesReceptionConfirmed" ), ReportEntry( ReportEntry::Type::APPLICATION_INFO, VariableDatatype( activitiesReceptionConfirmed ) ));
		report.addEntry( new GroupEntry( "activitySendErrors" ), ReportEntry( ReportEntry::Type::APPLICATION_INFO, VariableDatatype( activitiesErrorCount ) ));
		report.addEntry( new GroupEntry( "activitiesDroppedDueToOverflow" ), ReportEntry( ReportEntry::Type::APPLICATION_INFO, VariableDatatype( droppedActivities.load() ) ));
		report.addEntry( new GroupEntry( "activitiesAsyncDroppedDueToOverflowWhileAnomaly" ), ReportEntry( ReportEntry::Type::APPLICATION_INFO, VariableDatatype( droppedActivitiesDuringAnomaly.load() ) ));		
		report.addEntry( new GroupEntry( "connectionErrors" ), ReportEntry( ReportEntry::Type::APPLICATION_INFO, VariableDatatype( connCallback.connectionErrors.load() ) ));		
		
		return report;
	}

	void Notify( const shared_ptr<Activity> & element ) override {
		if ( forwardAllActivities ){

			if ( pendingActivities.load() < max_pending_ops ){
				pendingActivities++;
				client->isend(&*element);
			}else{
				droppedActivities++;
			}
		}		
	}

	/**
	 * Implements ActivityMultiplexerListener::Notify, passes activity to out.
	 */
	void NotifyAsync( int lostActivitiesCount, const shared_ptr<Activity> & element ) override {	
		if ( forwardAllActivities ) return;

		if ( anomalyStatus ){
			if ( pendingActivities.load() < max_pending_ops ){
				pendingActivities++;
				client->isend(&*element);
			}else{
				droppedActivitiesDuringAnomaly++;
			}
		}else{
			lock_guard<mutex> lock(ringBuffMutex);

			//cout << "Adding " << curPos << " " << element->ucaid_ << " " << sendPos << endl;		

			if ( curPos == sendPos && ringBuffer[curPos] != nullptr ) {
				sendPos = (sendPos + 1) % ringBufferSize;
			}

			ringBuffer[ curPos ] = element;
			curPos = (curPos + 1) % ringBufferSize;
		}
	}
	
	void clearAnomalyStatus(){
		anomalyStatus = false;
	}

	// It is expected that this function is not called concurrently.
	void triggerResponseForAnomaly(bool anomalyStillOngoing){
		// if we overtook the buffer, then everything must be send...		
		// Send the current ringbuffer away iff the activities have not been send so far.
		
		lock_guard<mutex> lock(ringBuffMutex);
		anomalyStatus = anomalyStillOngoing;

		//cout << sendPos << " " << curPos << endl;

		for(uint pos = sendPos ; pos < (ringBufferSize + sendPos) ; pos ++){
			const int realPos = pos % ringBufferSize;

			//cout << realPos << " " << ringBuffer.size() << endl;

			// small opportunity for a race condition if the buffer is full now...
			shared_ptr<Activity> data = ringBuffer[realPos];
			ringBuffer[realPos] = nullptr;

			if ( data == nullptr ){
				this->sendPos = realPos;
				return;
			}

			//cout << "sending " << realPos << " " << data->ucaid_ << endl;
			if ( pendingActivities.load() < max_pending_ops ){
				pendingActivities++;
				client->isend(&*data);
			}			
		}
		this->sendPos = curPos;
	}

	/**
	 * Dummy implementation for ActivityNetworkForwarder Options.
	 */
	ComponentOptions * AvailableOptions(){
		return new ActivityNetworkForwarderClientOptions();
	}

	/**
	 * Forwarder setup:
	 * Register this to a already created multiplexer
	 * (a ActivityMultiplexerPlugin always also creates a Multiplexer).
	 * Create a second ActivityMultiplexer instance that serves as an
	 * out going end of the Forwarder.
	 */
	void initPlugin() {
		pendingActivities = 0;
		droppedActivities = 0;
		droppedActivitiesDuringAnomaly = 0;

		ActivityNetworkForwarderClientOptions & options = getOptions<ActivityNetworkForwarderClientOptions>();
		CommunicationModule * comm =  GET_INSTANCE(CommunicationModule, options.comm);
		client = comm->startClientService(options.targetAddress, & connCallback, this);

		Reasoner * reasoner =  GET_INSTANCE(Reasoner, options.reasoner);
		this->ringBufferSize = options.ringBufferSize;
		this->forwardAllActivities = options.forwardAllActivities;

		this->ringBuffer.resize(ringBufferSize);

		if ( reasoner != nullptr ){
			reasoner->connectTrigger(this);
		}		
	}

	virtual void messageSendCB(BareMessage * msg){ 
		activitiesSendCount++;
	}

	virtual void messageResponseCB(BareMessage * msg, char * buffer, uint64_t buffer_size){
		activitiesReceptionConfirmed++;
		pendingActivities--;
	}

	virtual void messageTransferErrorCB(BareMessage * msg, CommunicationError error){  
		activitiesErrorCount++;
		pendingActivities--;
	}

	virtual uint64_t serializeMessageLen(const void * msgObject) {
		return j_serialization::serializeLen(* (Activity*) msgObject);
	}

	virtual void serializeMessage(const void * msgObject, char * buffer, uint64_t & pos){
		j_serialization::serialize(* (Activity*) msgObject, buffer, pos);
	}

	~ActivityNetworkForwarderClient(){
		delete(client);
	}

	private:
		ServiceClient * client;
		ReconnectionCallback connCallback;
};

extern "C" {
	void * MONITORING_ACTIVITY_MULTIPLEXER_PLUGIN_INSTANCIATOR_NAME()
	{
		return new ActivityNetworkForwarderClient();
	}
}
