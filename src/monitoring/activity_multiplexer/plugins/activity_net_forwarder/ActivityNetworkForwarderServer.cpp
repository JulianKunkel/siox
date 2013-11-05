#include <monitoring/activity_multiplexer/ActivityMultiplexer.hpp>

#include <core/comm/CommunicationModule.hpp>
#include <core/comm/NetworkService.hpp>

#include <monitoring/activity_multiplexer/plugins/activity_net_forwarder/ActivityBinarySerializable.hpp>

#include "ActivityNetworkForwarderOptions.hpp"


using namespace core;
using namespace monitoring;

/**
 * Forward an activity from one ActivityMultiplexer to another.
 *
 */
class ActivityNetworkServerForwarder: public NetworkService, public ServerCallback {
	public:


	ComponentOptions * AvailableOptions() {
		return new ActivityNetworkForwarderServerOptions();
	}

	void init() {
		ActivityNetworkForwarderServerOptions & options = getOptions<ActivityNetworkForwarderServerOptions>();
		CommunicationModule * comm =  GET_INSTANCE(CommunicationModule, options.comm);
		target =  GET_INSTANCE(ActivityMultiplexer, options.target_multiplexer);

		assert(target != nullptr);
		assert(comm != nullptr);

		server = comm->startServerService(options.serviceAddress, this);
	}

	~ActivityNetworkServerForwarder(){
		delete(server);
	}


	virtual void messageReceivedCB(std::shared_ptr<ServerClientMessage> msg, const char * message_data, uint64_t buffer_size){
		shared_ptr<Activity> activity ( new Activity() );

		uint64_t pos = 0;
		j_serialization::deserialize(*activity, message_data, pos, buffer_size);
		target->Log(activity);

		msg->isendResponse(nullptr);
	}

	virtual uint64_t serializeResponseMessageLen(const ServerClientMessage * msg, const void * responseType){
		return 0;
	}

	virtual void serializeResponseMessage(const ServerClientMessage * msg, const void * responseType, char * buffer, uint64_t & pos){
	}


	private:
		ActivityMultiplexer * target = nullptr;
		ServiceServer * server;
};

extern "C" {
	void * NETWORK_SERVICE_INSTANCIATOR_NAME()
	{
		return new ActivityNetworkServerForwarder();
	}
}
