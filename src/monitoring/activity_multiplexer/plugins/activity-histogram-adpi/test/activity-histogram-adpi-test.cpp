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

using namespace std;

using namespace core;
using namespace monitoring;
using namespace knowledge;

int main( int argc, char const * argv[] )
{
	ActivityMultiplexerPlugin * adpi = core::module_create_instance<ActivityMultiplexerPlugin>( "", "siox-monitoring-activityPlugin-histogramAdpi", ACTIVITY_MULTIPLEXER_PLUGIN_INTERFACE );

	ActivityPluginDereferencing * facade = core::module_create_instance<ActivityPluginDereferencing>( "", "siox-knowledge-DereferencingFacade", ACTIVITY_DEREFERENCING_FACADE_INTERFACE );

	SystemInformationGlobalIDManager * systemInfo = core::module_create_instance<SystemInformationGlobalIDManager>( "", "siox-monitoring-FileBasedSystemInformation", SYSTEMINFORMATION_GLOBALID_MANAGER_INTERFACE );
	
	systemInfo->getOptions<FileBasedSystemInformationOptions>().filename = "system-info-test.txt";
	systemInfo->init( );

	facade->getOptions<DereferencingFacadeOptions>().system_information_manager = systemInfo;
	facade->init();

	UniqueInterfaceID uid = systemInfo->register_interfaceID( "MPI", "MPICH2" );
	UniqueComponentActivityID ucaid1 = systemInfo->register_activityID( uid, "open" );
	UniqueComponentActivityID ucaid2 = systemInfo->register_activityID( uid, "write" );


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


	cout << "OK" << endl;

	delete( systemInfo );
	delete( facade );
	delete( adpi );
	delete( reporter );

	return 0;
}

