#include <mutex>

#include <monitoring/activity_multiplexer/ActivityMultiplexer.hpp>
#include <monitoring/activity_multiplexer/plugins/activity_net_forwarder/ActivityBinarySerializable.hpp>
#include <knowledge/reasoner/Reasoner.hpp>

#include "ActivityNetworkForwarderClient.hpp"
#include "ActivityNetworkForwarderOptions.hpp"

using namespace core;
using namespace monitoring;
using namespace knowledge;

#define max_pending_ops 10000

ComponentReport ActivityNetworkForwarderClient::prepareReport(){
	ComponentReport report;

	report.addEntry( new GroupEntry( "activitiesSend" ), ReportEntry( ReportEntry::Type::APPLICATION_INFO, VariableDatatype( activitiesSendCount ) ));
	report.addEntry( new GroupEntry( "activitiesReceptionConfirmed" ), ReportEntry( ReportEntry::Type::APPLICATION_INFO, VariableDatatype( activitiesReceptionConfirmed ) ));
	report.addEntry( new GroupEntry( "activitySendErrors" ), ReportEntry( ReportEntry::Type::APPLICATION_INFO, VariableDatatype( activitiesErrorCount ) ));
	report.addEntry( new GroupEntry( "activitiesDroppedDueToOverflow" ), ReportEntry( ReportEntry::Type::APPLICATION_INFO, VariableDatatype( droppedActivities.load() ) ));
	report.addEntry( new GroupEntry( "activitiesAsyncDroppedDueToOverflowWhileAnomaly" ), ReportEntry( ReportEntry::Type::APPLICATION_INFO, VariableDatatype( droppedActivitiesDuringAnomaly.load() ) ));
	report.addEntry( new GroupEntry( "connectionErrors" ), ReportEntry( ReportEntry::Type::APPLICATION_INFO, VariableDatatype( connCallback.connectionErrors.load() ) ));
	
	return report;
}

void ActivityNetworkForwarderClient::Notify( const shared_ptr<Activity> & element, int lost ) {
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
void ActivityNetworkForwarderClient::NotifyAsync( const shared_ptr<Activity> & element, int lostActivitiesCount ) {
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

void ActivityNetworkForwarderClient::clearAnomalyStatus(){
	anomalyStatus = false;
}

// It is expected that this function is not called concurrently.
void ActivityNetworkForwarderClient::triggerResponseForAnomaly(bool anomalyStillOngoing){
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
ComponentOptions * ActivityNetworkForwarderClient::AvailableOptions(){
	return new ActivityNetworkForwarderClientOptions();
}

void ActivityNetworkForwarderClient::stop() {
	delete(client);

	ActivityMultiplexerPlugin::stop();

	// free pending messages
	sendPos = 0;
	curPos = 0;
}

void ActivityNetworkForwarderClient::start() {
	ActivityNetworkForwarderClientOptions & options = getOptions<ActivityNetworkForwarderClientOptions>();
	CommunicationModule * comm =  GET_INSTANCE(CommunicationModule, options.comm);

	client = comm->startClientService(options.targetAddress, & connCallback, this);

	ActivityMultiplexerPlugin::start();
}


/**
 * Forwarder setup:
 * Register this to a already created multiplexer
 * (a ActivityMultiplexerPlugin always also creates a Multiplexer).
 * Create a second ActivityMultiplexer instance that serves as an
 * out going end of the Forwarder.
 */
void ActivityNetworkForwarderClient::initPlugin() {
	pendingActivities = 0;
	droppedActivities = 0;
	droppedActivitiesDuringAnomaly = 0;

	ActivityNetworkForwarderClientOptions & options = getOptions<ActivityNetworkForwarderClientOptions>();

	Reasoner * reasoner =  GET_INSTANCE(Reasoner, options.reasoner);
	this->ringBufferSize = options.ringBufferSize;
	this->forwardAllActivities = options.forwardAllActivities;

	this->ringBuffer.resize(ringBufferSize);

	if( multiplexer ) {
		multiplexer->registerCatchall( this, static_cast<ActivityMultiplexer::Callback>( &ActivityNetworkForwarderClient::Notify ), false );
		multiplexer->registerCatchall( this, static_cast<ActivityMultiplexer::Callback>( &ActivityNetworkForwarderClient::NotifyAsync ), true );
	}

	if ( reasoner != nullptr ){
		reasoner->connectTrigger(this);
	}
}

void ActivityNetworkForwarderClient::messageSendCB(BareMessage * msg){
	activitiesSendCount++;
}

void ActivityNetworkForwarderClient::messageResponseCB(BareMessage * msg, char * buffer, uint64_t buffer_size){
	activitiesReceptionConfirmed++;
	pendingActivities--;
}

void ActivityNetworkForwarderClient::messageTransferErrorCB(BareMessage * msg, CommunicationError error){ 
	activitiesErrorCount++;
	pendingActivities--;
}

uint64_t ActivityNetworkForwarderClient::serializeMessageLen(const void * msgObject) {
	return j_serialization::serializeLen(* (Activity*) msgObject);
}

void ActivityNetworkForwarderClient::serializeMessage(const void * msgObject, char * buffer, uint64_t & pos){
	j_serialization::serialize(* (Activity*) msgObject, buffer, pos);
}

void ActivityNetworkForwarderClient::finalize() {
	if( multiplexer ) {
		multiplexer->unregisterCatchall( this, false );
		multiplexer->unregisterCatchall( this, true );
	}
	ActivityMultiplexerPlugin::finalize();
}


extern "C" {
	void * MONITORING_ACTIVITY_MULTIPLEXER_PLUGIN_INSTANCIATOR_NAME()
	{
		return new ActivityNetworkForwarderClient();
	}
}
