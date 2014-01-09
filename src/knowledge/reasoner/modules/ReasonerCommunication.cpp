#include <unistd.h> // for sleep()

#include <knowledge/reasoner/modules/ReasonerCommunication.hpp>

#include <knowledge/reasoner/modules/ReasoningDatatypesSerializable.hpp>


namespace knowledge{
ReasonerCommunication::~ReasonerCommunication(){
	if (server){			
		delete(server);
	}
	if ( upstreamReasoner != nullptr ){
		delete(upstreamReasoner);
	}
}


void ReasonerCommunication::init(ReasonerCommunicationOptions & options){
	CommunicationModule * comm =  GET_INSTANCE(CommunicationModule, options.comm);
	assert(comm != nullptr);

	this->reasonerID = options.reasonerID;

	// spawn the server upon which we receive the remote status of different nodes.
	server = comm->startServerService(options.serviceAddress, this);

	if ( options.upstreamReasoner != "" ){
		upstreamReasoner = comm->startClientService( options.upstreamReasoner, this, this );
	}
}

void ReasonerCommunication::pushNodeStateUpstream(shared_ptr<NodeHealth> & health, Timestamp time){
	assert( upstreamReasoner );
	ReasonerMessageData data;

	data.reasonerID = this->reasonerID;
	data.timestamp = time;
	data.containedData = ReasonerMessageDataType::NODE;
	data.messagePayload = &* health;

	upstreamReasoner->isend( & data, (const void*) ReasonerMessageDataType::SYSTEM );
}

void ReasonerCommunication::pushSystemStateUpstream(shared_ptr<SystemHealth> & health, Timestamp time){
	assert( upstreamReasoner );
	ReasonerMessageData data;

	data.reasonerID = this->reasonerID;
	data.timestamp = time;
	data.containedData = ReasonerMessageDataType::SYSTEM;
	data.messagePayload = &* health;

	upstreamReasoner->isend( & data, (const void*) ReasonerMessageDataType::NONE );
}

void ReasonerCommunication::pushProcessStateUpstream(shared_ptr<ProcessHealth> & health, Timestamp time){
	assert( upstreamReasoner );
	ReasonerMessageData data;

	data.reasonerID = this->reasonerID;
	data.timestamp = time;
	data.containedData = ReasonerMessageDataType::PROCESS;
	data.messagePayload = &* health;

	upstreamReasoner->isend( & data, (const void*) ReasonerMessageDataType::NODE );
}


// Server implementation
void ReasonerCommunication::messageReceivedCB(std::shared_ptr<ServerClientMessage> msg, const char * message_data, uint64_t buffer_size){

	ReasonerMessageDataType expectedResponse = parseReceivedData(message_data, buffer_size);

	//cout << "messageReceivedCB: " << (int) expectedResponse << endl;


	// always send a response
	ReasonerMessageData rmsd;

	// TODO check against our role if we can provide the requested data
	rmsd.containedData = expectedResponse;
	rmsd.reasonerID = this->reasonerID;

	switch (expectedResponse){
	case (ReasonerMessageDataType::PROCESS) : {
		shared_ptr<ProcessHealth> payload = cb.getProcessHealth();
		rmsd.messagePayload = &* payload;
		msg->isendResponse(& rmsd);
		break;
	}
	case (ReasonerMessageDataType::NODE) : {
		shared_ptr<NodeHealth> payload = cb.getNodeHealth();
		rmsd.messagePayload = &* payload;
		msg->isendResponse(& rmsd);		
		break;
	}
	case (ReasonerMessageDataType::SYSTEM) : {	
		shared_ptr<SystemHealth> payload = cb.getSystemHealth();
		rmsd.messagePayload = &* payload;
		msg->isendResponse(& rmsd);				
		break;
	}
	default:{
		rmsd.messagePayload = nullptr;
		msg->isendResponse(& rmsd);
	}
	}
}

uint64_t ReasonerCommunication::serializeResponseMessageLen(const ServerClientMessage * msg, const void * responseType)
{
	return serializeMessageLen(responseType);
}

void ReasonerCommunication::serializeResponseMessage(const ServerClientMessage * msg, const void * responseType, char * buffer, uint64_t & pos){
	serializeMessage(responseType, buffer, pos);
}



ReasonerMessageDataType ReasonerCommunication::parseReceivedData(const char * buffer, uint64_t buffer_size){
	ReasonerMessageData data; 
	uint64_t pos = 0;

	j_serialization::deserialize(data, buffer, pos, buffer_size);

	cout << "parseReceivedData: " << (int) data.containedData << endl;


	if (data.containedData == ReasonerMessageDataType::NONE){
		return ReasonerMessageDataType::NONE;
	}

	ReasonerMessageDataType expectedResponse;

	ReasonerMessageReceived rmr( data.timestamp, data.reasonerID );

	switch( data.containedData ){
	case (ReasonerMessageDataType::PROCESS) : {
		ProcessHealth health;
		j_serialization::deserialize( health, buffer, pos, buffer_size );

		// invoke the callback
		cb.receivedReasonerProcessHealth(rmr, health); 

		expectedResponse = ReasonerMessageDataType::NODE;
		break;
	}
	case (ReasonerMessageDataType::NODE) : {
		NodeHealth health;
		j_serialization::deserialize( health, buffer, pos, buffer_size );
		// invoke the callback
		cb.receivedReasonerNodeHealth(rmr, health); 

		expectedResponse = ReasonerMessageDataType::SYSTEM;
		break;
	}
	case (ReasonerMessageDataType::SYSTEM) : {
		SystemHealth health;
		j_serialization::deserialize( health, buffer, pos, buffer_size );
		// invoke the callback
		cb.receivedReasonerSystemHealth(rmr, health); 
		
		expectedResponse = ReasonerMessageDataType::NONE;
		break;
	}
	case (ReasonerMessageDataType::NONE) :{
	}
	}

	return expectedResponse;
}

void ReasonerCommunication::messageResponseCB(BareMessage * msg, char * buffer, uint64_t buffer_size){
	if ( msg->user_ptr == (size_t) ReasonerMessageDataType::NONE ){
		return;
	}
	parseReceivedData(buffer, buffer_size);
}

void ReasonerCommunication::messageTransferErrorCB(BareMessage * msg, CommunicationError error){ 

}

void ReasonerCommunication::connectionSuccessfullCB(ServiceClient & client){
	//cout << "CONNECTED" << endl;
}

uint64_t ReasonerCommunication::serializeMessageLen(const void * msgObject){	
	ReasonerMessageData & data = *(ReasonerMessageData *) msgObject;

	if (data.containedData == ReasonerMessageDataType::NONE){
		return 0;
	}

	uint64_t serLen = j_serialization::serializeLen(data);

	switch( data.containedData ){
	case (ReasonerMessageDataType::NONE) : {
		break;
	}
	case (ReasonerMessageDataType::PROCESS) : {
		serLen += j_serialization::serializeLen(*(ProcessHealth*) data.messagePayload );
		break;
	}
	case (ReasonerMessageDataType::NODE) : {
		serLen += j_serialization::serializeLen(*(NodeHealth*) data.messagePayload );
		break;
	}
	case (ReasonerMessageDataType::SYSTEM) : {
		serLen += j_serialization::serializeLen( *(SystemHealth*) data.messagePayload );
		break;
	}
	}		

	return serLen;
}

void ReasonerCommunication::serializeMessage(const void * msgObject, char * buffer, uint64_t & pos){	
	//cout << "serializeMessage" << endl;

	ReasonerMessageData & data = *(ReasonerMessageData *) msgObject;
	if (data.containedData == ReasonerMessageDataType::NONE){
		return;
	}

	j_serialization::serialize(data, buffer, pos);
	switch( data.containedData ){
	case (ReasonerMessageDataType::NONE) : {
		break;
	}
	case (ReasonerMessageDataType::PROCESS) : {
		j_serialization::serialize(*(ProcessHealth*) data.messagePayload, buffer, pos );
		break;
	}
	case (ReasonerMessageDataType::NODE) : {
		j_serialization::serialize(*(NodeHealth*) data.messagePayload, buffer, pos );
		break;
	}
	case (ReasonerMessageDataType::SYSTEM) : {
		j_serialization::serialize( *(SystemHealth*) data.messagePayload, buffer, pos );
		break;
	}
	}
}

// client connection callback
void ReasonerCommunication::connectionErrorCB(ServiceClient & client, CommunicationError error){
	// remove currently pending messages
	client.dropPendingMessages();
	sleep(1);
	// try to reconnect periodically
	client.ireconnect();
}
}

