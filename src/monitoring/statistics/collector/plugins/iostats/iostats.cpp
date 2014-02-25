#include "../ProcSingleFilePlugin.hpp"
#include <stdint.h>
#include <stdio.h>

#include <map>

static bool isPrefixed( string prefix, string aString );

enum DeviceType {
	DEVICE_TYPE_MEMORY,
	DEVICE_TYPE_PHYSICAL,
	DEVICE_TYPE_VIRTUAL,
	DEVICE_TYPE_COUNT
};

struct StorageDevice{
	std::array<StatisticsValue, 11> currentValues;
	DeviceType type;

	StorageDevice(){
		for( size_t i = 11; i--; ) currentValues[i] = (uint64_t) 0;
	}
};

class IOstats: public ProcSingleFilePlugin<15> {
	public:
		virtual vector<StatisticsProviderDatatypes> availableMetrics() throw();

	private:
		void timestepStarts() override;
		void timestepLine( int lineNr,  vector<string> & entries )  override;
		void initLine( int lineNr, vector<string> & entries )  override;
		const string filename()  override;

		map<string, StorageDevice> knownDevices;

		StorageDevice aggregated[DeviceType::DEVICE_TYPE_COUNT];
};

void IOstats::timestepStarts(){
	for(int i=0; i < DEVICE_TYPE_COUNT; i++){
		auto & cur = aggregated[i].currentValues;
		for( size_t i = 11; i--; ) cur[i] = (uint64_t) 0;
	}
}

void IOstats::timestepLine( int lineNr,  vector<string> & entries ) {
	string name = entries[2];

	if( knownDevices.find( name ) == knownDevices.end() ) {
		cerr << "File " << filename() << " changed while accessing. New field " << name << endl;
		return;
	}
	StorageDevice & dev = knownDevices[name];

	auto & cur = dev.currentValues;

	for( int i = 0; i < 2; i++ ) {
		cur[i] = ( uint64_t ) atoll( entries[i + 3].c_str() );
	}
	cur[2] = ( ( uint64_t ) atoll( entries[2 + 3].c_str() ) ) * 512;

	for( int i = 3; i < 6; i++ ) {
		cur[i] = ( uint64_t ) atoll( entries[i + 3].c_str() );
	}
	cur[6] = ( ( uint64_t ) atoll( entries[6 + 3].c_str() ) ) * 512;

	for( int i = 7; i < 11; i++ ) {
		cur[i] = ( uint64_t ) atoll( entries[i + 3].c_str() );
	}

	auto & ag = aggregated[dev.type].currentValues;
	for( size_t i = 11; i--; ) ag[i] += cur[i];
}

void IOstats::initLine( int lineNr, vector<string> & entries ) {
	string name = entries[2];
	knownDevices[name] = StorageDevice();
}

const string IOstats::filename() {
	return "/proc/diskstats";
}

static void registerDevice(const string & topologyPath, vector<StatisticsProviderDatatypes> & r, StorageDevice & sd){
	 std::array<StatisticsValue, 11> & cur = sd.currentValues;
		r.push_back( {"quantity/block/reads", topologyPath, cur[0], INCREMENTAL, "", "Field 1 -- # of reads issued"} );
		r.push_back( {"quantity/block/reads/merged", topologyPath, cur[1], INCREMENTAL, "", "Field 2 -- # of reads merged"} );
		r.push_back( {"quantity/block/dataRead", topologyPath, cur[2], INCREMENTAL, "Bytes", "Data read based on Field 3 -- # of sectors read"} );
		r.push_back( {"time/block/reads", topologyPath, cur[3], INCREMENTAL, "ms", "Field 4 -- # of milliseconds spent reading"} );

		r.push_back( {"quantity/block/writes", topologyPath, cur[4], INCREMENTAL, "", "Field 5 -- # of writes completed"} );
		r.push_back( {"quantity/block/writes/merged", topologyPath, cur[5], INCREMENTAL, "", "Field 6 -- # of writes merged"} );
		r.push_back( {"quantity/block/dataWritten", topologyPath, cur[6], INCREMENTAL, "Bytes", "Data written based on Field 7 -- # of sectors written"} );
		r.push_back( {"time/block/writes", topologyPath, cur[7], INCREMENTAL, "ms", "Field 8 -- # of milliseconds spent writing"} );

		r.push_back( {"quantity/block/pendingIOs", topologyPath, cur[8], SAMPLED, "", "Field 9 -- # of I/Os currently in progress"} );
		r.push_back( {"time/block/access", topologyPath, cur[9], INCREMENTAL, "ms", "Field 10 -- # of milliseconds spent doing I/Os"} );
		r.push_back( {"time/block/weighted", topologyPath, cur[10], INCREMENTAL, "ms", "Field 11 -- weighted # of milliseconds spent doing I/Os"} );
}


static string deviceName2topologyPath( string device, enum DeviceType & outType ) {
	///@todo TODO: Currently we handle only "sd" and "hd" devices and partitions. Extend to all block devices listed in https://www.kernel.org/doc/Documentation/devices.txt .
	outType = DEVICE_TYPE_VIRTUAL;

	if( isPrefixed( "sd", device ) || isPrefixed( "hd", device ) ) {
		//We have an IDE or SCSI device, possibly with a partition number.
		assert( device.size() >= 3 && device[2] >= 'a' && device[2] <= 'z' );
		std::ostringstream path;
		path << "@localhost/" << device[0] << "d:" << device[2] << ":block-device";
		if( device.size() == 3 ) return path.str();	//It's a whole device
		//It's a partition
		for( size_t i = device.size(); i-->3; ) assert( device[i] >= '0' && device[i] <= '9' );
		path << "/partition:" << &device.c_str()[3];

		outType = DEVICE_TYPE_PHYSICAL;
		return path.str();
	}

	if ( isPrefixed( "dm-", device ) ){
		device = "dm" + device.substr(3);
		outType = DEVICE_TYPE_VIRTUAL;
	}

	if ( isPrefixed( "ram", device ) ){
		outType = DEVICE_TYPE_MEMORY;
	}

	//Handle devices of unknown format
	const char* prefix = "@localhost/";
	const char* postfix = ":master";
	char path[strlen( prefix ) + device.size() + strlen( postfix ) + 1];
	//find the start of the device number
	size_t deviceNumberStart = device.size();
	for(; deviceNumberStart-->1 && device[deviceNumberStart] <= '9' && device[deviceNumberStart] >= '0'; ) ;
	deviceNumberStart++;
	//put the ring path together
	if( deviceNumberStart == device.size() ) {
		//No device number, this is a master device
		//sprintf() is safe due to the way the array size was calculated.
		sprintf( path, "%s%s%s", prefix, device.c_str(), postfix );
	} else {
		char* currChar = path;
		for( size_t i = 0; prefix[i]; i++ ) *currChar++ = prefix[i];
		for( size_t i = 0; i < deviceNumberStart; i++ ) *currChar++ = device[i];
		*currChar++ = ':';
		for( size_t i = deviceNumberStart; i < device.size(); i++ ) *currChar++ = device[i];
		*currChar++ = 0;
	}
	return string( path );
}

vector<StatisticsProviderDatatypes> IOstats::availableMetrics() throw() {
	vector<StatisticsProviderDatatypes> r;
	// walk through all known devices:

	// TODO: determine available DEVICES using /sys/block/
	// Could be combined using the library for detecting hardware in the KB?
	// Right now we consider DEVICES to be NODE metrics, although this is wrong.
	// It is NODE level for virtual devices and real DEVICES for existing hardware

	for( auto iterator = knownDevices.begin(); iterator != knownDevices.end(); iterator++ ) {
		string name = iterator->first;
		enum DeviceType type;
		string topologyPath = deviceName2topologyPath( name, type );

		knownDevices[name].type = type;

		//cout << name << endl;
		registerDevice(topologyPath, r,  knownDevices[name] );
	}

	registerDevice("@localhost", r, aggregated[DEVICE_TYPE_PHYSICAL]);
	registerDevice("@localhost/device:virtual:device", r, aggregated[DEVICE_TYPE_VIRTUAL]);
	registerDevice("@localhost/device:memory:device", r, aggregated[DEVICE_TYPE_MEMORY]);

	return r;
}

static bool isPrefixed( string prefix, string aString ) {
	if( prefix.size() > aString.size() ) return false;
	for( size_t i = prefix.size(); i--; ) if( prefix[i] != aString[i] ) return false;
	return true;
}


extern "C" {
	void * MONITORING_STATISTICS_PLUGIN_INSTANCIATOR_NAME()
	{
		return new IOstats();
	}
}
