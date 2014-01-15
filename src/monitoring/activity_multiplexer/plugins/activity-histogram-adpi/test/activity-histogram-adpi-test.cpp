#include <unistd.h>

#include <core/module/ModuleLoader.hpp>

#include <monitoring/activity_multiplexer/ActivityMultiplexerPlugin.hpp>
#include <core/reporting/RuntimeReporter.hpp>
#include <core/component/ComponentRegistrar.hpp>
#include <knowledge/activity_plugin/ActivityPluginDereferencing.hpp>


#include <monitoring/system_information/SystemInformationGlobalIDManager.hpp>

#include <monitoring/system_information/modules/filebased-system-information/FileBasedSystemInformationOptions.hpp>
#include <knowledge/activity_plugin/DereferencingFacadeOptions.hpp>


#include "../HistogramAdpiOptions.hpp"
#include <monitoring/topology/Topology.hpp>

#include <monitoring/topology/RamTopologyOptions.hpp>

using namespace core;
using namespace monitoring;

using namespace std;

using namespace core;
using namespace monitoring;
using namespace knowledge;

int main( int argc, char const * argv[] )
{
	ActivityMultiplexerPlugin * adpi = core::module_create_instance<ActivityMultiplexerPlugin>( "", "siox-monitoring-activityPlugin-histogramAdpi", ACTIVITY_MULTIPLEXER_PLUGIN_INTERFACE );

	ActivityPluginDereferencing * facade = core::module_create_instance<ActivityPluginDereferencing>( "", "siox-knowledge-DereferencingFacade", ACTIVITY_DEREFERENCING_FACADE_INTERFACE );

	SystemInformationGlobalIDManager * systemInfo = core::module_create_instance<SystemInformationGlobalIDManager>( "", "siox-monitoring-FileBasedSystemInformation", SYSTEMINFORMATION_GLOBALID_MANAGER_INTERFACE );

	Topology* topology = module_create_instance<Topology>( "", "siox-monitoring-RamTopology", MONITORING_TOPOLOGY_INTERFACE );
	topology->getOptions<RamTopologyOptions>();
	topology->init();

	
	systemInfo->getOptions<FileBasedSystemInformationOptions>().filename = "system-info-test.txt";
	systemInfo->init( );

	facade->getOptions<DereferencingFacadeOptions>().system_information_manager = systemInfo;
	facade->getOptions<DereferencingFacadeOptions>().topology = topology;
	facade->init();

	UniqueInterfaceID uid = systemInfo->register_interfaceID( "MPI", "MPICH2" );
	UniqueComponentActivityID ucaid1 = systemInfo->register_activityID( uid, "open" );
	UniqueComponentActivityID ucaid2 = systemInfo->register_activityID( uid, "write" );

	UniqueComponentActivityID ucaid3 = systemInfo->register_activityID( uid, "close" );

   // register a topology entry with pre-filled data for ucaid3 to test if we read it properly
   {
   stringstream s;
   s << "AMUXPlugin:ADPIPlugin/data:" << ucaid3;

  	TopologyType dataType = topology->registerType("ADPIPlugin");
  	TopologyObject ucaid3data = topology->registerObjectByPath( s.str() );
	
	TopologyAttribute bucketMinAttribute = topology->registerAttribute( dataType.id(), "min", VariableDatatype::Type::UINT64 );
	TopologyAttribute bucketMaxAttribute = topology->registerAttribute( dataType.id(), "max", VariableDatatype::Type::UINT64 );

	topology->setAttribute( ucaid3data.id(), bucketMinAttribute.id(), (uint64_t) 2000 );
	topology->setAttribute( ucaid3data.id(), bucketMaxAttribute.id(), (uint64_t) 4000 );
	}


	auto & options = adpi->getOptions<HistogramAdpiOptions>();
	options.learnCount = 3;
	options.dereferenceFacade = facade;

	options.buckets = 10;
	options.extremeBucketCount = 1; 
	options.slowFastBucketCount = 3; 

	adpi->init();


	auto parentArray = vector<ActivityID> {  {.cid = {.pid = {2, 3, 4}, .id = 1}, .id = 1} };
	auto attributeArray = vector<Attribute>();
	attributeArray.push_back( {.id = 1, .value = ( uint64_t ) 4711} );
	attributeArray.push_back( {.id = 2, .value = VariableDatatype( "julian/data.nc4" )} );
	attributeArray.push_back( {.id = 3, .value = ( uint32_t ) 1} );

	auto remoteCallsArray = vector<RemoteCall>();

	const ComponentID cid = {.pid = {2, 3, 4}, .id = 1};
	const ActivityID aaid = {.cid = cid, .id = 2};
	
	adpi->Notify( shared_ptr<Activity>( new Activity( ucaid1, 0, 100, aaid, parentArray, attributeArray, remoteCallsArray, NULL, 0 ) ) );

	adpi->Notify( shared_ptr<Activity>( new Activity( ucaid1, 0, 1100, aaid, parentArray, attributeArray, remoteCallsArray, NULL, 0 ) ) );

	adpi->Notify( shared_ptr<Activity>( new Activity( ucaid1, 0, 550, aaid, parentArray, attributeArray, remoteCallsArray, NULL, 0 ) ) );

	// learn phase is finished now for ucaid1.

	// try with ucaid2
	adpi->Notify( shared_ptr<Activity>( new Activity( ucaid2, 0, 1000, aaid, parentArray, attributeArray, remoteCallsArray, NULL, 0 ) ) );


	// try to rate a few activities:	
	adpi->Notify( shared_ptr<Activity>( new Activity( ucaid1, 0, 2000, aaid, parentArray, attributeArray, remoteCallsArray, NULL, 0 ) ) );

	{
	unique_ptr<unordered_map<ComponentID, AnomalyPluginHealthStatistic>> aphsMap = dynamic_cast<AnomalyPlugin*>(adpi)->queryRecentObservations();

	assert( aphsMap->size() == 1 );

	AnomalyPluginHealthStatistic & aph = (*aphsMap)[cid];
	assert( aph.cid == cid );
	
	assert( aph.positiveIssues.size() == 0 );
	assert( aph.negativeIssues.size() == 0 );

	assert( aph.occurrences[HealthState::ABNORMAL_SLOW ] == 1 );
	}


	adpi->Notify( shared_ptr<Activity>( new Activity( ucaid1, 0, 10, aaid, parentArray, attributeArray, remoteCallsArray, NULL, 0 ) ) );
	{
	unique_ptr<unordered_map<ComponentID, AnomalyPluginHealthStatistic>> aphsMap = dynamic_cast<AnomalyPlugin*>(adpi)->queryRecentObservations();

	assert( aphsMap->size() == 1 );

	AnomalyPluginHealthStatistic & aph = (*aphsMap)[cid];
	assert( aph.cid == cid );
	
	assert( aph.positiveIssues.size() == 0 );
	assert( aph.negativeIssues.size() == 0 );

	assert( aph.occurrences[HealthState::ABNORMAL_FAST ] == 1 );
	}

	adpi->Notify( shared_ptr<Activity>( new Activity( ucaid1, 0, 400, aaid, parentArray, attributeArray, remoteCallsArray, NULL, 0 ) ) );
	{
	unique_ptr<unordered_map<ComponentID, AnomalyPluginHealthStatistic>> aphsMap = dynamic_cast<AnomalyPlugin*>(adpi)->queryRecentObservations();

	assert( aphsMap->size() == 1 );

	AnomalyPluginHealthStatistic & aph = (*aphsMap)[cid];
	assert( aph.cid == cid );
	assert( aph.positiveIssues.size() == 0 );
	assert( aph.negativeIssues.size() == 0 );
	assert( aph.occurrences[HealthState::OK ] == 1 );
	}

	adpi->Notify( shared_ptr<Activity>( new Activity( ucaid1, 0, 999, aaid, parentArray, attributeArray, remoteCallsArray, NULL, 0 ) ) );
	{
	unique_ptr<unordered_map<ComponentID, AnomalyPluginHealthStatistic>> aphsMap = dynamic_cast<AnomalyPlugin*>(adpi)->queryRecentObservations();

	assert( aphsMap->size() == 1 );

	AnomalyPluginHealthStatistic & aph = (*aphsMap)[cid];
	assert( aph.cid == cid );
	assert( aph.positiveIssues.size() == 0 );
	assert( aph.negativeIssues.size() == 0 );
	assert( aph.occurrences[HealthState::SLOW ] == 1 );
	}



	RuntimeReporter * reporter = core::module_create_instance<RuntimeReporter>( "", "siox-core-reporting-ConsoleReporter", CORE_REPORTER_INTERFACE );

	list< pair<RegisteredComponent*, ComponentReport> > lst;

	RegisteredComponent * me = new RegisteredComponent( 1, "test", "histogramADPI", adpi );

	lst.push_back( { me, dynamic_cast<ComponentReportInterface*>(adpi)->prepareReport()} ); 

	reporter->processFinalReport( lst );

	// dump the current topology relation list
	
	TopologyObject stored1;
	TopologyObject stored2;
	TopologyObject stored3;
	{
	TopologyType typ = topology->lookupTypeByName("data");
	assert(typ);

	stringstream path;
	path << "AMUXPlugin:ADPIPlugin/data:" << ucaid1;
	stored1 = topology->lookupObjectByPath( path.str() );
	assert(stored1 != nullptr);
	}
	{
	stringstream path;
	path << "AMUXPlugin:ADPIPlugin/data:" << ucaid2;
	stored2 = topology->lookupObjectByPath( path.str() );
	assert(stored2 == nullptr);
	}

	// test if the injected data for ucaid3 has been set properly
	{
	stringstream path;
	path << "AMUXPlugin:ADPIPlugin/data:" << ucaid3;
	stored3 = topology->lookupObjectByPath( path.str() );
	assert(stored3 != nullptr);
	}

	adpi->Notify( shared_ptr<Activity>( new Activity( ucaid3, 0, 1000, aaid, parentArray, attributeArray, remoteCallsArray, NULL, 0 ) ) );

	{
	unique_ptr<unordered_map<ComponentID, AnomalyPluginHealthStatistic>> aphsMap = dynamic_cast<AnomalyPlugin*>(adpi)->queryRecentObservations();

	assert( aphsMap->size() == 1 );

	AnomalyPluginHealthStatistic & aph = (*aphsMap)[cid];
	assert( aph.cid == cid );
	
	assert( aph.positiveIssues.size() == 0 );
	assert( aph.negativeIssues.size() == 0 );

	assert( aph.occurrences[HealthState::ABNORMAL_FAST ] == 1 );
	}



	cout << "OK" << endl;

	delete( systemInfo );
	delete( facade );
	delete( adpi );
	delete( reporter );
	delete( topology );

	return 0;
}


