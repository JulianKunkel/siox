#include <iostream>
#include <fstream>
#include <list>
#include <mutex>

#include "libpq-fe.h"

#include <monitoring/datatypes/Activity.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginImplementation.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerListener.hpp>
#include <monitoring/activity_multiplexer/plugins/postgres_writer/PostgreSQLQuerier.hpp>

#include "ActivityPostgreSQLWriterOptions.hpp"

#include <core/component/ActivitySerializableText.cpp>

using namespace std;
using namespace monitoring;
using namespace core;

// It is important that the first parent class is of type ActivityMultiplexerPlugin
class PostgreSQLWriterPlugin : public ActivityMultiplexerPlugin {
private:
	PostgreSQLQuerier *querier_;
	PGconn *dbconn_;
public:

	void Notify( const shared_ptr<Activity> & activity, int lost ) {
		querier_->insert_activity(*activity);
	}

	ComponentOptions *AvailableOptions() {
		return new PostgreSQLWriterPluginOptions();
	}

	void initPlugin() override {		
		multiplexer->registerCatchall( this, static_cast<ActivityMultiplexer::Callback>( &PostgreSQLWriterPlugin::Notify ), false );
	}

	void finalize() override {
		multiplexer->unregisterCatchall( this, false );
		ActivityMultiplexerPlugin::finalize();
	}

	void start() override{
		PostgreSQLWriterPluginOptions &o = getOptions<PostgreSQLWriterPluginOptions>();
		
		dbconn_ = PQconnectdb(o.dbinfo.c_str());
		
		if (PQstatus(dbconn_) != CONNECTION_OK) {
			
			std::cerr << "Connection to database failed: " << PQerrorMessage(dbconn_) << std::endl;
			
		}

		querier_ = new PostgreSQLQuerier(*dbconn_);
	}

	void stop() override{
		PQfinish(dbconn_);
		delete(querier_);
	}
};

extern "C" {
	void *MONITORING_ACTIVITY_MULTIPLEXER_PLUGIN_INSTANCIATOR_NAME()
	{
		return new PostgreSQLWriterPlugin();
	}
}
