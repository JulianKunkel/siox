/**
 * An ActivityMultiplexerPlugin which watches the incoming activities and 
 * creates a histogram by which the quality of further activities is judged.
 *
 * For each activity type it works in two phases:
 * 1) It learns the typical range the response time of the activities are in (min, max).
 *   After N activities have been seen the first histogram is build by splitting the observed 
 *   range into B buckets of which the first A% and last A% of the buckets are considered to be abnormal 
 *   fast/slow, the middle M% are considered to be OK while P% are just marked as fast/slow.
 *   (This information should be stored in a configuration file)
 * 2) New activities are rated based on the bucket in the histogram.
 * 3) The histogram information is output at the end.
 * 
 * @author JK
 */

#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <limits>
#include <cmath>

#include <core/reporting/ComponentReportInterface.hpp>

#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginImplementation.hpp>
#include <monitoring/system_information/SystemInformationGlobalIDManager.hpp>
#include <monitoring/ontology/OntologyDatatypes.hpp>
#include <monitoring/datatypes/Activity.hpp>

#include <knowledge/reasoner/AnomalyPlugin.hpp>

#include <monitoring/topology/Topology.hpp>

#include "HistogramAdpiOptions.hpp"


using namespace std;

using namespace core;
using namespace monitoring;
using namespace knowledge;


//#define OUTPUT(...) do { cout << "[HistogramAdpi] " << __VA_ARGS__ << "\n"; } while(0)
#define OUTPUT(...) 


struct ActivityTimeStatistics{
	uint64_t minTimeS = numeric_limits<uint64_t>::max();
	Timestamp maxTimeS = 0;
	Timestamp histogramBucketWidth = 0; // (max - min) / binSize

	uint64_t totalOperationCount = 0;

	vector<uint64_t> histogram;	
};



class HistogramAdpiPlugin: public ActivityMultiplexerPlugin, public ComponentReportInterface, public AnomalyPlugin {
	public:
		void initPlugin() override;		
		void Notify( shared_ptr<Activity> activity ) override;
		ComponentReport prepareReport() override;

		ComponentOptions * AvailableOptions() override;
	private:
		unordered_map<UniqueComponentActivityID, ActivityTimeStatistics> statistics;
		TopologyTypeId   pluginTopoTypeID;
		TopologyObjectId pluginTopoObjectID;

		TopologyAttributeId bucketMinAttributID;
		TopologyAttributeId bucketMaxAttributID;
		Topology * topology;
};



ComponentOptions * HistogramAdpiPlugin::AvailableOptions() {
  return new HistogramAdpiOptions();
}


void HistogramAdpiPlugin::initPlugin() {
	assert( facade != nullptr );

	HistogramAdpiOptions & o = getOptions<HistogramAdpiOptions>();
	ActivityPluginDereferencing * f = GET_INSTANCE(ActivityPluginDereferencing, o.dereferenceFacade);
	topology =  f->topology();

	assert(topology);

	TopologyType dataType = topology->registerType("data");
	TopologyType adpiType = topology->registerType("ADPIPlugin");

	pluginTopoTypeID = dataType.id();

	TopologyObject myData = topology->registerObjectByPath( "AMUXPlugin" TOPO_TYPE_SEP "ADPIPlugin" );
	pluginTopoObjectID = myData.id();
	
	TopologyAttribute bucketMinAttribute = topology->registerAttribute( adpiType.id(), "min", VariableDatatype::Type::UINT64 );
	bucketMinAttributID = bucketMinAttribute.id();

	TopologyAttribute bucketMaxAttribute = topology->registerAttribute( adpiType.id(), "max", VariableDatatype::Type::UINT64 );
	bucketMaxAttributID = bucketMaxAttribute.id();
}

static string convertAIDToString(UniqueComponentActivityID aid){
	stringstream s;
	s << aid;
	return s.str();
}

void HistogramAdpiPlugin::Notify( shared_ptr<Activity> activity ) {
	const HistogramAdpiOptions & o = getOptions<HistogramAdpiOptions>();
	auto itr = statistics.find(activity->ucaid_);	
	if ( itr == statistics.end() ){
		statistics[activity->ucaid_] = {};
		itr = statistics.find(activity->ucaid_);

		ActivityTimeStatistics & mats = itr->second;

		// try to load the data object from a persistent representation		
		const string name = convertAIDToString( activity->ucaid_ );
		TopologyRelation tr = topology->lookupRelation( pluginTopoObjectID, pluginTopoTypeID, name );
		if (tr != nullptr){
			TopologyObjectId storedObjectID = tr.child();

			// try to get the attributes
			TopologyValue min = topology->getAttribute( storedObjectID, bucketMinAttributID );
			if (min != nullptr){
				TopologyValue max = topology->getAttribute( storedObjectID, bucketMaxAttributID );
				// cout << "reading values : " << min.value() << " - " << max.value() << endl;			
				mats.minTimeS = min.value().uint64();
				mats.maxTimeS = max.value().uint64();

				// we have to create the bins			
				mats.histogramBucketWidth = ( mats.maxTimeS - mats.minTimeS ) / o.buckets;

				mats.histogram.resize( o.buckets );				
			}
		}

	}
	ActivityTimeStatistics & ats = itr->second;
	Timestamp duration = activity->duration();

	// are we still in learning mode?
	ats.totalOperationCount++;

	if ( ! ats.histogram.empty() ){
		int bucket;
		// we finished learning, determine the bin.
		if ( duration < ats.minTimeS ){
			bucket = 0;
		}else if ( duration < ats.maxTimeS ){
			bucket = (uint64_t) ceil((duration - ats.minTimeS) / ats.histogramBucketWidth);
		}else{ // large than the max
			bucket = o.buckets - 1;
		}

		ats.histogram[bucket]++;

		// based on the bucket number we choose the goodness.
		HealthState goodness;
		if ( bucket < o.extremeBucketCount ){
			goodness = HealthState::ABNORMAL_FAST;
		}else if ( bucket < o.slowFastBucketCount ){
			goodness = HealthState::FAST;
		}else if ( bucket < ( o.buckets - o.slowFastBucketCount) ){
			goodness = HealthState::OK;
		}else if ( bucket < ( o.buckets - o.extremeBucketCount) ){
			goodness = HealthState::SLOW;
		}else{
			goodness = HealthState::ABNORMAL_SLOW;
		}

		OUTPUT(duration << " " << (duration - ats.minTimeS) << " " << bucket << " " << ats.histogramBucketWidth << " " << o.slowFastBucketCount << " " << goodness);

		addObservation( activity->aid_.cid, goodness, "", duration );

	}else{
		// we are learning
		ats.minTimeS = ats.minTimeS < duration ? ats.minTimeS : duration;
		ats.maxTimeS = ats.maxTimeS > duration ? ats.maxTimeS : duration;		

		if ( ats.totalOperationCount == o.learnCount && ( ats.maxTimeS != ats.minTimeS ) ){

			// we have to create the bins			
			ats.histogramBucketWidth = ( ats.maxTimeS - ats.minTimeS ) / o.buckets;

			// assert( ats.histogramBucketWidth > 0 )
			ats.histogram.resize( o.buckets );

			const string name = convertAIDToString( activity->ucaid_ );
			TopologyObject child = topology->registerObject( pluginTopoObjectID, pluginTopoTypeID, name, pluginTopoTypeID );
			TopologyObjectId storedObjectID = child.id();

			topology->setAttribute( storedObjectID, bucketMaxAttributID, ats.maxTimeS );
			topology->setAttribute( storedObjectID, bucketMinAttributID, ats.minTimeS );
		}
	}
}



ComponentReport HistogramAdpiPlugin::prepareReport()
{
	ComponentReport result;

	SystemInformationGlobalIDManager * sysInfo = facade->get_system_information();
	assert( sysInfo != nullptr );

	// todo lookup via systeminformation plugin
	for( auto itr = statistics.begin(); itr != statistics.end(); itr++ ){
		ActivityTimeStatistics & ats = itr->second;
		
		GroupEntry * ge;

		try{
			UniqueInterfaceID uiid = sysInfo->lookup_interface_of_activity( itr->first );
			ge = new GroupEntry( sysInfo->lookup_interface_name( uiid ) );
			ge = new GroupEntry( sysInfo->lookup_activity_name( itr->first), ge );
		}catch(NotFoundError & e){
			stringstream name;
			name << itr->first;
			ge = new GroupEntry( name.str() );
		}	

		result.addEntry( new GroupEntry( "OpCount", ge ), ReportEntry( ReportEntry::Type::APPLICATION_INFO, VariableDatatype( ats.totalOperationCount ) ));
		result.addEntry( new GroupEntry( "HistBucketWidth", ge ), ReportEntry( ReportEntry::Type::APPLICATION_INFO, VariableDatatype( ats.histogramBucketWidth ) ));
		result.addEntry( new GroupEntry( "MinTime", ge ), ReportEntry( ReportEntry::Type::APPLICATION_PERFORMANCE, VariableDatatype( ats.minTimeS ) ));
		result.addEntry( new GroupEntry( "MaxTime", ge ), ReportEntry( ReportEntry::Type::APPLICATION_PERFORMANCE, VariableDatatype( ats.maxTimeS ) ));

		// NOTE the MPI aggregation does not make any sense as the buckets intervals are likely to be different for each process. But we will output them as string.

		if ( ats.histogram.size() > 0 ){
			stringstream stream;

			uint64_t cur = 0;
			for ( uint i = 0 ; i < ats.histogram.size(); i++ ){
				stream << cur << "=" << ats.histogram[i] << " ";
				cur = (ats.histogramBucketWidth * i + ats.minTimeS);
			}
			stream << cur;

			result.addEntry( new GroupEntry( "Buckets", ge ), ReportEntry( ReportEntry::Type::APPLICATION_PERFORMANCE, VariableDatatype( stream.str() ) ));
		}
	}

	return result;
}


extern "C" {
	void * MONITORING_ACTIVITY_MULTIPLEXER_PLUGIN_INSTANCIATOR_NAME()
	{
		return new HistogramAdpiPlugin();
	}
}
