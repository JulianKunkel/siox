#include <iostream>
#include <fstream>
#include <list>
#include <thread>
#include <mutex>
#include <libpq-fe.h>

#include <core/component/ActivitySerializableText.cpp>
#include <core/persist/SetupPersistentStructures.hpp>


#include <monitoring/datatypes/Activity.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginImplementation.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerListener.hpp>
#include <monitoring/activity_multiplexer/plugins/postgres_writer/PostgreSQLQuerier.hpp>

#include "ActivityPostgreSQLWriterOptions.hpp"

using namespace std;
using namespace monitoring;
using namespace core;

// It is important that the first parent class is of type ActivityMultiplexerPlugin
class PostgreSQLWriterPlugin : public ActivityMultiplexerPlugin, public SetupPersistentStructures  {
private:
	PostgreSQLQuerier *querier_;
	PGconn *dbconn_;
	mutex mtx_;
public:

	int preparePersistentStructuresIfNecessary() override{
		bool ret = true;
		// TODO
		return ret;
	}

	int cleanPersistentStructures() override{
		bool ret = true;
		// TODO
		return ret;
	}

	void Notify(const shared_ptr<Activity> & activity, int lost) {
		mtx_.lock();
		querier_->insert_activity(*activity);
		mtx_.unlock();
	}

	ComponentOptions *AvailableOptions() {
		return new PostgreSQLWriterPluginOptions();
	}

	void initPlugin() override {		
		std::cout << "PostgreSQL Plug-in initializing..." << std::endl;
		multiplexer->registerCatchall(this, static_cast<ActivityMultiplexer::Callback>( &PostgreSQLWriterPlugin::Notify ), false);
	}

	void finalize() override {
		std::cout << "PostgreSQL Plug-in finalizing..." << std::endl;
		multiplexer->unregisterCatchall(this, false);
		ActivityMultiplexerPlugin::finalize();
	}

	void start() override {
		std::cout << "PostgreSQL Plug-in starting..." << std::endl;

		PostgreSQLWriterPluginOptions &o = getOptions<PostgreSQLWriterPluginOptions>();
		
		dbconn_ = PQconnectdb(o.dbinfo.c_str());
		
		if (PQstatus(dbconn_) != CONNECTION_OK) {
			std::cerr << "Connection to database failed: " << PQerrorMessage(dbconn_) << std::endl;
		}

		querier_ = new PostgreSQLQuerier(*dbconn_);
	}

	void stop() override {
		std::cout << "PostgreSQL Plug-in stopping..." << std::endl;
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
