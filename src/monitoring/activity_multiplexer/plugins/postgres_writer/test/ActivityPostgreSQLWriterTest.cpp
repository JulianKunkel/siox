#include <core/module/ModuleLoader.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerPlugin.hpp>
#include <util/Rnd.hpp>

#include "../ActivityPostgreSQLWriterOptions.hpp"

#define SIOX_DB_INFO "host = '127.0.0.1' port = '5432' user = 'postgres' password = 'siox.db.X916' dbname = 'siox'"

using namespace std;

using namespace monitoring;
using namespace core;

int main(int argc, char const *argv[])
{
	ActivityMultiplexer *m1 = core::module_create_instance<ActivityMultiplexer>("", "siox-monitoring-ActivityMultiplexerAsync", "monitoring_activitymultiplexer");
	ActivityMultiplexerPlugin *ap = core::module_create_instance<ActivityMultiplexerPlugin>("", "siox-monitoring-activityPlugin-ActivityPostgreSQLWriter", ACTIVITY_MULTIPLEXER_PLUGIN_INTERFACE);

	PostgreSQLWriterPluginOptions &op = (PostgreSQLWriterPluginOptions &) ap->getOptions();
	op.dbinfo = SIOX_DB_INFO;
	op.multiplexer.componentPointer = m1;

	m1->init();
	ap->init();
	m1->start();
	ap->start();

 
	srand(time(NULL));
	Activity *act = rnd::activity();
	m1->Log( shared_ptr<Activity>(act));

	m1->stop();
	ap->stop();

	delete(ap);
	delete(m1);
	
	return 0;
}


