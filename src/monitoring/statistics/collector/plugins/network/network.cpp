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
			void init( StatisticsProviderPluginOptions * options )  override;

			void nextTimestep() throw()  override;
			vector<StatisticsProviderDatatypes> availableMetrics() throw() override;
		private:
			class InterfaceData {
				public:
					StatisticsValue bandwidthBytes = ( uint64_t )0;	//the theoretical link speed
					StatisticsValue bytesSent = ( uint64_t )0, bytesRecieved = ( uint64_t )0;
					StatisticsValue packetsSent = ( uint64_t )0, packetsRecieved = ( uint64_t )0;
					string interfaceName, speedPath, bytesSentPath, bytesRecievedPath, packetsSentPath, packetsRecievedPath;

					InterfaceData(){}
					InterfaceData( string interfaceName );
					void nextTimestep() throw();
					void addMetricsToList( vector<StatisticsProviderDatatypes>* metricsList ) throw();
			};

			vector<InterfaceData> interfaces;

			// aggregated information for the full node
			InterfaceData nodeDate;
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
		string topologyPath = "@localhost/networkInterface:" + interfaceName;
		if ( interfaceName == "" ){ // the global name
			topologyPath = "@localhost/network:kernel";
		}else if( interfaceName.size() >= 4 && string( interfaceName, 0, 3 ) == "eth" ) {
			topologyPath = "@localhost/eth:" + string( interfaceName, 3, interfaceName.size() - 3 );
		}
		#define addGaugeMetric( variableName, unitString, descriptionString ) do { \
			metricsList->push_back( {"quantity/network/" #variableName, topologyPath, variableName, GAUGE, unitString, descriptionString} ); \
		} while(0)
		#define addIncrementalMetric( variableName, unitString, descriptionString ) do { \
			metricsList->push_back( {"quantity/network/" #variableName, topologyPath, variableName, INCREMENTAL, unitString, descriptionString} ); \
		} while(0)
		addGaugeMetric( bandwidthBytes, "B/s", "theoretical link speed" );
		addIncrementalMetric( bytesSent, "B", "total bytes sent over this network interface" );
		addIncrementalMetric( bytesRecieved, "B", "total bytes recieved over this network interface" );
		addIncrementalMetric( packetsSent, "", "total packets sent over this network interface" );
		addIncrementalMetric( packetsRecieved, "", "total packets recieved over this network interface" );
		
	}


	void NetworkStatisticsPlugin::init( StatisticsProviderPluginOptions * options ) {
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
		nodeDate.bandwidthBytes = (uint64_t) 0;
		nodeDate.bytesSent = (uint64_t) 0;
		nodeDate.packetsSent = (uint64_t) 0;
		nodeDate.bytesRecieved = (uint64_t) 0;
		nodeDate.packetsRecieved = (uint64_t) 0;

		for( size_t i = interfaces.size(); i--; ){
			interfaces[i].nextTimestep();

			nodeDate.bandwidthBytes += interfaces[i].bandwidthBytes;
			nodeDate.bytesSent += interfaces[i].bytesSent;
			nodeDate.packetsSent += interfaces[i].packetsSent;
			nodeDate.bytesRecieved += interfaces[i].bytesRecieved;
			nodeDate.packetsRecieved += interfaces[i].packetsRecieved;
		}
	}


	vector<StatisticsProviderDatatypes> NetworkStatisticsPlugin::availableMetrics() throw() {
		vector<StatisticsProviderDatatypes> result;
		for( size_t i = interfaces.size(); i--; ) interfaces[i].addMetricsToList( &result );
		nodeDate.addMetricsToList( & result );		
		return result;
	}
}


extern "C" {
	void * MONITORING_STATISTICS_PLUGIN_INSTANCIATOR_NAME() {
		return new NetworkStatisticsPlugin();
	}
}
