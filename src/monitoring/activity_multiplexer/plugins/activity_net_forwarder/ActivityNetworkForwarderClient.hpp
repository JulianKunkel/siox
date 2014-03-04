/**
 * @date 2013, 2014
 * @author Julian Kunkel, Nathanael Hübbe
 */

#ifndef INCLUDE_GUARD_MONITORING_ACTIVITY_MULTIPLEXER_PLUGINS_ACTIVITY_NET_FORWARDER_ACTIVITY_NETWORK_FORWARDER_CLIENT
#define INCLUDE_GUARD_MONITORING_ACTIVITY_MULTIPLEXER_PLUGINS_ACTIVITY_NET_FORWARDER_ACTIVITY_NETWORK_FORWARDER_CLIENT

#include <unistd.h>
#include <atomic>

#include <core/comm/CommunicationModule.hpp>
#include <core/reporting/ComponentReportInterface.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginImplementation.hpp>
#include <knowledge/reasoner/AnomalyTrigger.hpp>

class ReconnectionCallback : public core::ConnectionCallback{
	public:
		atomic<uint32_t> connectionErrors;

		ReconnectionCallback() : connectionErrors(0) {}

		void connectionErrorCB(core::ServiceClient & connection, core::CommunicationError error){
			connectionErrors++;
			sleep( 1 );

			connection.ireconnect();
		}

		void connectionSuccessfullCB(core::ServiceClient & connection){}
};

/**
 * Forward an activity from one ActivityMultiplexer to another.
 * Data is only forwarded if this client is triggered.
 */
class ActivityNetworkForwarderClient : public monitoring::ActivityMultiplexerPlugin, public core::MessageCallback, public knowledge::AnomalyTrigger, public core::ComponentReportInterface {
	public:
		void initPlugin();
		ComponentOptions * AvailableOptions();
		void start() override;

		void Notify( const shared_ptr<Activity> & element, int lost );
		void NotifyAsync( const shared_ptr<Activity> & element, int lostActivitiesCount );
		
		void clearAnomalyStatus();

		// It is expected that this function is not called concurrently.
		void triggerResponseForAnomaly(bool anomalyStillOngoing);

		virtual void messageSendCB(BareMessage * msg);
		virtual void messageResponseCB(BareMessage * msg, char * buffer, uint64_t buffer_size);
		virtual void messageTransferErrorCB(BareMessage * msg, CommunicationError error);
		virtual uint64_t serializeMessageLen(const void * msgObject);
		virtual void serializeMessage(const void * msgObject, char * buffer, uint64_t & pos);

		ComponentReport prepareReport();

		void stop() override;
		void finalize() override;

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

		ServiceClient * client;
		ReconnectionCallback connCallback;
};

#endif
