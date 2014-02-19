#include <core/module/ModuleLoader.hpp>
#include <util/time.h>

#include <monitoring/activity_multiplexer/ActivityMultiplexerPlugin.hpp>
#include <monitoring/activity_multiplexer/plugins/bdbwriter/ActivityBDBWriter.hpp>

#include <monitoring/activity_multiplexer/plugins/bdbwriter/ActivityBDBWriterPluginOptions.hpp>

using namespace std;

using namespace monitoring;
using namespace core;

int main( int argc, char const * argv[] )
{
	ActivityMultiplexer * m1 = core::module_create_instance<ActivityMultiplexer>( "", "siox-monitoring-ActivityMultiplexerAsync", "monitoring_activitymultiplexer" );

	// wir registrieren das Plugin (normal geschieht dies automatisch)
	ActivityBDBWriterPlugin * ap = core::module_create_instance<ActivityBDBWriterPlugin>( "", "siox-monitoring-activityPlugin-ActivityBDBWriter", ACTIVITY_MULTIPLEXER_PLUGIN_INTERFACE );

	// init plugin
	// not necessary, but for testing...
	ActivityBDBWriterPluginOptions & op = ( ActivityBDBWriterPluginOptions & ) ap->getOptions();
	op.dirname = "db";
	op.multiplexer.componentPointer = m1;

	m1->init();
	ap->init();

	auto parentArray = vector<ActivityID> {  {.cid = {.pid = {2, 3, 4}, .id = 1}, .id = 1} };
	auto attributeArray = vector<Attribute> {{.id = 111, .value = VariableDatatype( "myData" )}, {.id = 3, . value = ( uint64_t ) 4711}};
	auto remoteCallsArray = vector<RemoteCall>();


	UniqueComponentActivityID aid = 4;
	ActivityID aaid = {.cid = {.pid = {2, 3, 4}, .id = 1}, .id = 2};
	// Cast the real activity to the serializable object class wrapper
	shared_ptr<Activity> activity ( new Activity( aid, 3, 5, aaid, parentArray, attributeArray, remoteCallsArray, NULL, 0 ) );

  	uint64_t t0 = siox_gettime();
	for(int i=0; i < 10000000; i++){
		activity->aid_.id =  i;
		m1->Log( activity);
		//ap->Notify( activity, 0 );

		uint64_t delta = siox_gettime() - t0;
		if ( delta >= 1000000000ull ){
			printf("%d in %lld s => %f/s and %f microseconds per op\n", i, delta/1000000000ull, i/(delta/1000000000.0), (delta/1000.0)/i);
			break;
		}
	}

	//sleep(1);

	delete( ap );

	delete( m1 );
	cout << "OK" << endl;
}


