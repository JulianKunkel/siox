#ifndef KNOWLEDGE_REASONER_COMMUNICATION_HPP
#define KNOWLEDGE_REASONER_COMMUNICATION_HPP

#include <core/comm/CommunicationModule.hpp>

#include <knowledge/reasoner/modules/ReasonerCommunicationMessage.hpp>
#include <knowledge/reasoner/modules/ReasonerStandardImplementationOptions.hpp>

using namespace std;

namespace knowledge {

struct ReasonerMessageReceived{
		string reasonerID; // identifier of the reasoner sending this message

		ReasonerMessageReceived( string reasonerID ): reasonerID(reasonerID) {}
};

struct ReasoningDataReceivedCB{
	virtual void receivedReasonerProcessHealth(ReasonerMessageReceived & data, ProcessHealth & health) = 0;
	virtual void receivedReasonerNodeHealth(ReasonerMessageReceived & data, NodeHealth & health) = 0;
	virtual void receivedReasonerSystemHealth(ReasonerMessageReceived & data, SystemHealth & health) = 0;

	virtual shared_ptr<NodeHealth> getNodeHealth() = 0;
	virtual shared_ptr<SystemHealth> getSystemHealth() = 0;
	virtual shared_ptr<ProcessHealth> getProcessHealth() = 0;
};

class ReasonerCommunication : core::ServerCallback, core::MessageCallback, core::ConnectionCallback{
public:
	void init(ReasonerCommunicationOptions & options);

	void pushNodeStateUpstream(shared_ptr<NodeHealth> & health);
	void pushSystemStateUpstream(shared_ptr<SystemHealth> & health);
	void pushProcessStateUpstream(shared_ptr<ProcessHealth> & health);

	ReasonerCommunication(ReasoningDataReceivedCB & cb): cb(cb){}

	~ReasonerCommunication();

private:
	ReasoningDataReceivedCB & cb;
	string reasonerID;

	ReasonerMessageDataType parseReceivedData(const char * buffer, uint64_t buffer_size);

	// Server implementation
	virtual void messageReceivedCB(std::shared_ptr<core::ServerClientMessage> msg, const char * message_data, uint64_t buffer_size) override;

	virtual uint64_t serializeResponseMessageLen(const core::ServerClientMessage * msg, const void * responseType) override;

	virtual void serializeResponseMessage(const core::ServerClientMessage * msg, const void * responseType, char * buffer, uint64_t & pos) override;

	virtual void messageSendCB(core::BareMessage * msg) override{ /* do nothing */ }

	virtual void messageResponseCB(core::BareMessage * msg, char * buffer, uint64_t buffer_size) override;

	virtual void messageTransferErrorCB(core::BareMessage * msg, core::CommunicationError error) override;

	virtual uint64_t serializeMessageLen(const void * msgObject) override;

	virtual void serializeMessage(const void * msgObject, char * buffer, uint64_t & pos) override;

	// client connection callback
	virtual void connectionErrorCB(core::ServiceClient & client, core::CommunicationError error) override;

	virtual void connectionSuccessfullCB(core::ServiceClient & client) override;

	core::ServiceServer * server = nullptr;
	core::ServiceClient * upstreamReasoner = nullptr;
};
}

#endif

