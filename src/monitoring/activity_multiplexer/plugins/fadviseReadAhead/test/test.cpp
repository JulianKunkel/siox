#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <core/module/ModuleLoader.hpp>

#include <monitoring/activity_multiplexer/ActivityMultiplexerPlugin.hpp>
#include <core/reporting/RuntimeReporter.hpp>
#include <core/component/ComponentRegistrar.hpp>
#include <knowledge/activity_plugin/ActivityPluginDereferencing.hpp>
#include <monitoring/system_information/SystemInformationGlobalIDManager.hpp>

#include <monitoring/system_information/modules/filebased-system-information/FileBasedSystemInformationOptions.hpp>
#include <knowledge/activity_plugin/DereferencingFacadeOptions.hpp>


#include <monitoring/activity_multiplexer/plugins/fadviseReadAhead/fadviseReadAheadOptions.hpp>
#include <monitoring/topology/Topology.hpp>
#include <monitoring/ontology/Ontology.hpp>
#include <monitoring/topology/RamTopologyOptions.hpp>

using namespace core;
using namespace monitoring;

using namespace std;

using namespace core;
using namespace monitoring;
using namespace knowledge;



int main( int argc, char const * argv[] )
{
	ActivityMultiplexerPlugin * plugin = core::module_create_instance<ActivityMultiplexerPlugin>( "", "siox-monitoring-activityPlugin-fadviseReadAhead", ACTIVITY_MULTIPLEXER_PLUGIN_INTERFACE );

	ActivityPluginDereferencing * facade = core::module_create_instance<ActivityPluginDereferencing>( "", "siox-knowledge-DereferencingFacade", ACTIVITY_DEREFERENCING_FACADE_INTERFACE );

	SystemInformationGlobalIDManager * systemInfo = core::module_create_instance<SystemInformationGlobalIDManager>( "", "siox-monitoring-FileBasedSystemInformation", SYSTEMINFORMATION_GLOBALID_MANAGER_INTERFACE );

	Ontology * ontology = core::module_create_instance<Ontology>( "", "siox-monitoring-FileOntology", ONTOLOGY_INTERFACE );

	Topology* topology = module_create_instance<Topology>( "", "siox-monitoring-RamTopology", MONITORING_TOPOLOGY_INTERFACE );
	topology->getOptions<RamTopologyOptions>();
	topology->init();

	
	systemInfo->getOptions<FileBasedSystemInformationOptions>().filename = "system-info-test.txt";
	systemInfo->init( );

	facade->getOptions<DereferencingFacadeOptions>().system_information_manager = systemInfo;
	facade->getOptions<DereferencingFacadeOptions>().topology = topology;
	facade->getOptions<DereferencingFacadeOptions>().ontology = ontology;
	facade->init();

	// register types needed for test

	UniqueInterfaceID uid = systemInfo->register_interfaceID( "POSIX", "" );
	UniqueComponentActivityID ucaidOpen = systemInfo->register_activityID( uid, "open" );
	UniqueComponentActivityID ucaidWrite = systemInfo->register_activityID( uid, "write" );
	UniqueComponentActivityID ucaidRead = systemInfo->register_activityID( uid, "read" );
	UniqueComponentActivityID ucaidPRead = systemInfo->register_activityID( uid, "pread" );
	UniqueComponentActivityID ucaidClose = systemInfo->register_activityID( uid, "close" );


	OntologyAttributeID fhAttr = ontology->register_attribute( "POSIX", "descriptor/filehandle" , VariableDatatype::Type::UINT32 ).aID;
	OntologyAttributeID filenameAttr = ontology->register_attribute( "POSIX", "descriptor/filename", VariableDatatype::Type::STRING ).aID;
	OntologyAttributeID fileposAttr = ontology->register_attribute( "POSIX", "file/position" , VariableDatatype::Type::UINT64 ).aID;
	OntologyAttributeID bytesReadAttr = ontology->register_attribute( "POSIX", "quantity/BytesRead", VariableDatatype::Type::UINT64 ).aID;
	OntologyAttributeID bytesWrittenAttr = ontology->register_attribute( "POSIX", "quantity/BytesWritten", VariableDatatype::Type::UINT64 ).aID;


	auto & options = plugin->getOptions<FadviseReadAheadPluginOptions>();
	options.dereferenceFacade = facade;
	plugin->init();

	const ComponentID cid = {.pid = {2, 3, 4}, .id = 1};
	const ActivityID aaidOpen = {.cid = cid, .id = 2};
	const ActivityID aaidOthers = {.cid = cid, .id = 3};

	Timestamp time = 0;
	
	// open a real file, otherwise seek and fadvise() break
	uint32_t fd = open("testfile", O_CREAT | O_TRUNC | O_RDWR, S_IRWXU);

	assert(fd != 0);

	// write some data
	void * buff = malloc(10000);
	write(fd, buff, 10000);
	free(buff);

	plugin->Notify( shared_ptr<Activity>( new Activity( ucaidOpen, 0, time++, aaidOpen, {}, {{.id = filenameAttr, .value = VariableDatatype( "julian/data.nc4" )}}, {}, NULL, 0 ) ) );

	// Read 6 times 100 bytes from position 0, 200, 400, ... 1000

	// try with ucaidPRead
	plugin->Notify( shared_ptr<Activity>( new Activity( ucaidPRead, 0, time++, aaidOthers, {aaidOpen}, {{.id = bytesReadAttr, .value = (uint64_t) 100}, {.id = fileposAttr, .value = (uint64_t) 0}, {.id = fhAttr, .value = fd }}, {}, NULL, 0 ) ) );

	// try with ucaidPRead
	plugin->Notify( shared_ptr<Activity>( new Activity( ucaidPRead, 0, time++, aaidOthers, {aaidOpen}, {{.id = bytesReadAttr, .value = (uint64_t) 100}, {.id = fileposAttr, .value = (uint64_t) 200}, {.id = fhAttr, .value = fd }}, {}, NULL, 0 ) ) );

	// try with ucaidPRead
	plugin->Notify( shared_ptr<Activity>( new Activity( ucaidPRead, 0, time++, aaidOthers, {aaidOpen}, {{.id = bytesReadAttr, .value = (uint64_t) 100}, {.id = fileposAttr, .value = (uint64_t) 400}, {.id = fhAttr, .value = fd }}, {}, NULL, 0 ) ) );

	// try with ucaidPRead
	plugin->Notify( shared_ptr<Activity>( new Activity( ucaidPRead, 0, time++, aaidOthers, {aaidOpen}, {{.id = bytesReadAttr, .value = (uint64_t) 100}, {.id = fileposAttr, .value = (uint64_t) 600}, {.id = fhAttr, .value = fd }}, {}, NULL, 0 ) ) );

	// try with ucaidPRead
	plugin->Notify( shared_ptr<Activity>( new Activity( ucaidPRead, 0, time++, aaidOthers, {aaidOpen}, {{.id = bytesReadAttr, .value = (uint64_t) 100}, {.id = fileposAttr, .value = (uint64_t) 800}, {.id = fhAttr, .value = fd }}, {}, NULL, 0 ) ) );

	// try with ucaidPRead
	plugin->Notify( shared_ptr<Activity>( new Activity( ucaidPRead, 0, time++, aaidOthers, {aaidOpen}, {{.id = bytesReadAttr, .value = (uint64_t) 100}, {.id = fileposAttr, .value = (uint64_t) 1000}, {.id = fhAttr, .value = fd }}, {}, NULL, 0 ) ) );

	// try with ucaidPRead
	plugin->Notify( shared_ptr<Activity>( new Activity( ucaidPRead, 0, time++, aaidOthers, {aaidOpen}, {{.id = bytesReadAttr, .value = (uint64_t) 100}, {.id = fileposAttr, .value = (uint64_t) 1200}, {.id = fhAttr, .value = fd }}, {}, NULL, 0 ) ) );


	// see if a write resets the counter
	plugin->Notify( shared_ptr<Activity>( new Activity( ucaidWrite, 0, time++, aaidOthers, {aaidOpen}, {{.id = bytesWrittenAttr, .value = (uint64_t) 1000}, {.id = fhAttr, .value = fd }}, {}, NULL, 0 ) ) );

	// go to position 0
	plugin->Notify( shared_ptr<Activity>( new Activity( ucaidPRead, 0, time++, aaidOthers, {aaidOpen}, {{.id = bytesReadAttr, .value = (uint64_t) 100}, {.id = fileposAttr, .value = (uint64_t) 0}}, {}, NULL, 0 ) ) );

	// check if we use lseek properly
	lseek(fd, 0, SEEK_CUR);

	// try with ucaidRead
	plugin->Notify( shared_ptr<Activity>( new Activity( ucaidRead, 0, time++, aaidOthers, {aaidOpen}, {{.id = bytesReadAttr, .value = (uint64_t) 1000}, {.id = fhAttr, .value = fd }}, {}, NULL, 0 ) ) );


	// try with ucaidClose
	plugin->Notify( shared_ptr<Activity>( new Activity( ucaidClose, 0, time++, aaidOthers, {aaidOpen}, {}, {}, NULL, 0 ) ) );

	RuntimeReporter * reporter = core::module_create_instance<RuntimeReporter>( "", "siox-core-reporting-ConsoleReporter", CORE_REPORTER_INTERFACE );

	list< pair<RegisteredComponent*, ComponentReport> > lst;

	RegisteredComponent * me = new RegisteredComponent( 1, "test", "fadviseTest", plugin );

	lst.push_back( { me, dynamic_cast<ComponentReportInterface*>(plugin)->prepareReport()} ); 

	reporter->processFinalReport( lst );


	cout << "OK" << endl;

	delete( systemInfo );
	delete( facade );
	delete( plugin );
	delete( ontology );
	delete( reporter );
	delete( topology );

	close(fd);

	return 0;
}


