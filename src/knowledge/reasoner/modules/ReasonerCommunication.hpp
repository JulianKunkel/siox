#ifndef KNOWLEDGE_REASONER_COMMUNICATION_HPP
#define KNOWLEDGE_REASONER_COMMUNICATION_HPP

#include <core/comm/CommunicationModule.hpp>

#include <knowledge/reasoner/modules/ReasonerCommunicationMessage.hpp>
#include <knowledge/reasoner/modules/ReasonerStandardImplementationOptions.hpp>

using namespace std;
using namespace core;

namespace knowledge {

struct ReasonerMessageReceived{
		ReasonerMessageDataType containedData;
		
		Timestamp timestamp;

		string reasonerID; // identifier of the reasoner sending this message

		union {
			NodeHealth * n;
			ProcessHealth * p;
			SystemHealth * s;
		} u;

		ReasonerMessageReceived() : containedData(ReasonerMessageDataType::NONE) {  }

		ReasonerMessageReceived( ReasonerMessageDataType containedData, Timestamp timestamp, string reasonerID ) : containedData(containedData), timestamp(timestamp), reasonerID(reasonerID) {}

		 ReasonerMessageReceived & operator= (ReasonerMessageReceived & other){
		 	// Assignment operator should only be used for testing...
		 	u.n = other.u.n;
		 	containedData = other.containedData;
		 	timestamp = other.timestamp;
		 	reasonerID = other.reasonerID;
		 	
		 	other.containedData = ReasonerMessageDataType::NONE;
		 	return *this;
		 }

		~ReasonerMessageReceived(){
			switch(containedData){
			case (ReasonerMessageDataType::PROCESS):
				delete(u.p);
				break;
			case (ReasonerMessageDataType::NODE):
				delete(u.n);
				break;
			case (ReasonerMessageDataType::SYSTEM):
				delete(u.s);
				break;
			default:{

			}
			}
		}
};

struct ReasoningDataReceivedCB{
	virtual void receivedReasonerMessage(ReasonerMessageReceived & data) = 0;

	virtual shared_ptr<NodeHealth> getNodeHealth() = 0;
	virtual shared_ptr<SystemHealth> getSystemHealth() = 0;
	virtual shared_ptr<ProcessHealth> getProcessHealth() = 0;
};

class ReasonerCommunication : ServerCallback, MessageCallback, ConnectionCallback{
public:
	void init(ReasonerCommunicationOptions & options);

	void pushNodeStateUpstream(shared_ptr<NodeHealth> & health, Timestamp time);
	void pushSystemStateUpstream(shared_ptr<SystemHealth> & health, Timestamp time);
	void pushProcessStateUpstream(shared_ptr<ProcessHealth> & health, Timestamp time);

	ReasonerCommunication(ReasoningDataReceivedCB & cb): cb(cb){}

	~ReasonerCommunication();

private:
	ReasoningDataReceivedCB & cb;
	string reasonerID;

	ReasonerMessageDataType parseReceivedData(const char * buffer, uint64_t buffer_size);

	// Server implementation
	virtual void messageReceivedCB(std::shared_ptr<ServerClientMessage> msg, const char * message_data, uint64_t buffer_size) override;

	virtual uint64_t serializeResponseMessageLen(const ServerClientMessage * msg, const void * responseType) override;

	virtual void serializeResponseMessage(const ServerClientMessage * msg, const void * responseType, char * buffer, uint64_t & pos) override;

	virtual void messageSendCB(BareMessage * msg) override{ /* do nothing */ }

	virtual void messageResponseCB(BareMessage * msg, char * buffer, uint64_t buffer_size) override;

	virtual void messageTransferErrorCB(BareMessage * msg, CommunicationError error) override;

	virtual uint64_t serializeMessageLen(const void * msgObject) override;

	virtual void serializeMessage(const void * msgObject, char * buffer, uint64_t & pos) override;

	// client connection callback
	virtual void connectionErrorCB(ServiceClient & client, CommunicationError error) override;
	
	virtual void connectionSuccessfullCB(ServiceClient & client) override;

	ServiceServer * server = nullptr;
	ServiceClient * upstreamReasoner = nullptr;
};
}

#endif
