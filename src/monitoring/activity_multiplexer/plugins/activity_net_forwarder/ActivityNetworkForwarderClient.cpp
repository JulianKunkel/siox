#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginImplementation.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexer.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerListener.hpp>

#include <core/comm/CommunicationModule.hpp>
#include <monitoring/activity_multiplexer/plugins/activity_net_forwarder/ActivityBinarySerializable.hpp>

#include "ActivityNetworkForwarderOptions.hpp"

using namespace core;
using namespace monitoring;

/**
 * Forward an activity from one ActivityMultiplexer to another.
 *
 */
class ActivityNetworkForwarderClient: public ActivityMultiplexerPlugin, MessageCallback{
	public:
	/**
	 * Implements ActivityMultiplexerListener::Notify, passes activity to out.
	 */
	virtual void NotifyAsync( int lostActivitiesCount, Activity * element ) {
		client->isend(element);
	}

	/**
	 * Dummy implementation for ActivityNetworkForwarder Options.
	 */
	ComponentOptions * AvailableOptions() {
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
		ActivityNetworkForwarderClientOptions & options = getOptions<ActivityNetworkForwarderClientOptions>();
		CommunicationModule * comm =  GET_INSTANCE(CommunicationModule, options.comm);
		client = comm->startClientService(options.targetAddress, & connCallback, this);
	}

	virtual void messageSendCB(BareMessage * msg){ /* do nothing */ }

	virtual void messageResponseCB(BareMessage * msg, char * buffer, uint64_t buffer_size){
	}

	virtual void messageTransferErrorCB(BareMessage * msg, CommunicationError error){  }

	virtual uint64_t serializeMessageLen(const void * msgObject) {
		return j_serialization::serializeLen(* (Activity*) msgObject);
	}

	virtual void serializeMessage(const void * msgObject, char * buffer, uint64_t & pos){
		j_serialization::serialize(* (Activity*) msgObject, buffer, pos);
	}

	private:
		ServiceClient * client;
		ConnectionCallback connCallback;
};

extern "C" {
	void * MONITORING_ACTIVITY_MULTIPLEXER_PLUGIN_INSTANCIATOR_NAME()
	{
		return new ActivityNetworkForwarderClient();
	}
}
