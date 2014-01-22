/**
 * @author Nathanael Hübbe
 * @date 2014
 */

#include <monitoring/statistics/collector/StatisticsProviderPluginImplementation.hpp>
#include <util/ExceptionHandling.hpp>

#include <fstream>
#include <vector>
#include <dirent.h>

namespace {

	using namespace std;
	using namespace monitoring;

	class NetworkStatisticsPlugin : public StatisticsProviderPlugin {
		public:
			virtual void init( StatisticsProviderPluginOptions & options );

			virtual void nextTimestep() throw();
			virtual vector<StatisticsProviderDatatypes> availableMetrics() throw();
		private:
			class InterfaceData {
				public:
					StatisticsValue bandwidthBytes = ( uint64_t )0;	//the theoretical link speed
					StatisticsValue bytesSent = ( uint64_t )0, bytesRecieved = ( uint64_t )0;
					StatisticsValue packetsSent = ( uint64_t )0, packetsRecieved = ( uint64_t )0;
					string interfaceName, speedPath, bytesSentPath, bytesRecievedPath, packetsSentPath, packetsRecievedPath;

					InterfaceData( string interfaceName );
					void nextTimestep() throw();
					void addMetricsToList( vector<StatisticsProviderDatatypes>* metricsList ) throw();
			};

			vector<InterfaceData> interfaces;
	};


	NetworkStatisticsPlugin::InterfaceData::InterfaceData( string interfaceName ) :
		interfaceName( interfaceName ),
		speedPath( "/sys/class/net/" + interfaceName + "/speed" ),
		bytesSentPath( "/sys/class/net/" + interfaceName + "/statistics/tx_bytes" ),
		bytesRecievedPath( "/sys/class/net/" + interfaceName + "/statistics/rx_bytes" ),
		packetsSentPath( "/sys/class/net/" + interfaceName + "/statistics/tx_packets" ),
		packetsRecievedPath( "/sys/class/net/" + interfaceName + "/statistics/rx_packets" )
	{}


	uint64_t fetchFileValue( const string& path ) throw() {
		ifstream file( path );
		//Unfortunately, I found no exception free way to check whether opening the file actually succeeded.
		//On my system even `file.good() && file.is_open()` would succeed for the nonexistant file `/sys/class/net/lo/speed`.
		//No idea why, but if someone knows better, please feel free to insert a sensible check.
		uint64_t result = 0;
		IGNORE_EXCEPTIONS( file >> result; );
		return result;
	}


	void NetworkStatisticsPlugin::InterfaceData::nextTimestep() throw() {
		bandwidthBytes = 1000*1000/8*fetchFileValue( speedPath );	//the file gives the link speed in MBits, we use Bytes
		bytesSent = fetchFileValue( bytesSentPath );
		bytesRecieved = fetchFileValue( bytesRecievedPath );
		packetsSent = fetchFileValue( packetsSentPath );
		packetsRecieved = fetchFileValue( packetsRecievedPath );
	}


	void NetworkStatisticsPlugin::InterfaceData::addMetricsToList( vector<StatisticsProviderDatatypes>* metricsList ) throw() {
		uint64_t overflow_value = ( uint64_t )-1;
		string topologyPath = "@localhost/networkInterface:" + interfaceName;
		#define addGaugeMetric( variableName, unitString, descriptionString ) do { \
			metricsList->push_back( {NETWORK, DEVICE, "quantity/" #variableName, topologyPath, variableName, GAUGE, unitString, descriptionString, 0, 0} ); \
		} while(0)
		#define addIncrementalMetric( variableName, unitString, descriptionString ) do { \
			metricsList->push_back( {NETWORK, DEVICE, "quantity/" #variableName, topologyPath, variableName, INCREMENTAL, unitString, descriptionString, overflow_value, 0} ); \
		} while(0)
		addGaugeMetric( bandwidthBytes, "B/s", "theoretical link speed" );
		addIncrementalMetric( bytesSent, "B", "total bytes sent over this network interface" );
		addIncrementalMetric( bytesRecieved, "B", "total bytes recieved over this network interface" );
		addIncrementalMetric( packetsSent, "", "total packets sent over this network interface" );
		addIncrementalMetric( packetsRecieved, "", "total packets recieved over this network interface" );
		
	}


	void NetworkStatisticsPlugin::init( StatisticsProviderPluginOptions& options ) {
		if( DIR* directory = opendir( "/sys/class/net" ) ) {
			union {	//this is for portability
				struct dirent64 data;
				char spacer[offsetof ( struct dirent64, d_name ) + NAME_MAX + 1];
			} enlargedDirent;
			struct dirent64* curEntry = NULL;
			while( !readdir64_r( directory, &enlargedDirent.data, &curEntry ) && curEntry ) {
				if( curEntry->d_name[0] != '.' ) {	//we need to skip the "." and ".." entries
					string interfaceName( curEntry->d_name, _D_EXACT_NAMLEN( curEntry ) );
					interfaces.emplace_back( interfaceName );
				}
			}
			closedir( directory );
		}
	}


	void NetworkStatisticsPlugin::nextTimestep() throw() {
		for( size_t i = interfaces.size(); i--; ) interfaces[i].nextTimestep();
	}


	vector<StatisticsProviderDatatypes> NetworkStatisticsPlugin::availableMetrics() throw() {
		vector<StatisticsProviderDatatypes> result;
		for( size_t i = interfaces.size(); i--; ) interfaces[i].addMetricsToList( &result );
		return result;
	}
}


extern "C" {
	void * MONITORING_STATISTICS_PLUGIN_INSTANCIATOR_NAME() {
		return new NetworkStatisticsPlugin();
	}
}
