#include "../ProcSingleFilePlugin.hpp"
#include <stdint.h>
#include <stdio.h>

#include <map>

static bool isPrefixed( string prefix, string aString );
static string deviceName2topologyPath( string device );

class IOstats: public ProcSingleFilePlugin<15> {
	public:
		virtual vector<StatisticsProviderDatatypes> availableMetrics();

	private:
		void timestepLine( int lineNr,  vector<string> & entries );
		void initLine( int lineNr, vector<string> & entries );
		const string filename();

		map<string, std::array<StatisticsValue, 11> > currentValues;
};



void IOstats::timestepLine( int lineNr,  vector<string> & entries ) {
	string name = entries[2];

	if( currentValues.find( name ) == currentValues.end() ) {
		cerr << "File " << filename() << " changed while accessing. New field " << name << endl;
		return;
	}

	std::array<StatisticsValue, 11> & cur = currentValues[name];

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
}

void IOstats::initLine( int lineNr, vector<string> & entries ) {
	string name = entries[2];
	currentValues[name] = std::array<StatisticsValue, 11>();
	for( size_t i = 11; i--; ) currentValues[name][i] = ( uint64_t )0;
}

const string IOstats::filename() {
	return "/proc/diskstats";
}

vector<StatisticsProviderDatatypes> IOstats::availableMetrics() {
	vector<StatisticsProviderDatatypes> result;
	// walk through all known devices:

	// TODO: determine available DEVICES using /sys/block/
	// Could be combined using the library for detecting hardware in the KB?
	// Right now we consider DEVICES to be NODE metrics, although this is wrong.
	// It is NODE level for virtual devices and real DEVICES for existing hardware

	for( auto iterator = currentValues.begin(); iterator != currentValues.end(); iterator++ ) {
		string name = iterator->first;
		string topologyPath = deviceName2topologyPath( name );
		//cout << name << endl;
		//Add the 11 metrics
		uint64_t overflow_value = ( uint64_t ) 1 << 63; //TODO CHECK ME, we expect 64 Bit...

		std::array<StatisticsValue, 11> & cur = currentValues[name];
		result.push_back( {INPUT_OUTPUT, NODE, "quantity/block/reads", topologyPath, cur[0], INCREMENTAL, "", "Field 1 -- # of reads issued", overflow_value, 0} );
		result.push_back( {INPUT_OUTPUT, NODE, "quantity/block/reads/merged", topologyPath, cur[1], INCREMENTAL, "", "Field 2 -- # of reads merged", overflow_value, 0} );
		result.push_back( {INPUT_OUTPUT, NODE, "quantity/block/dataRead", topologyPath, cur[2], INCREMENTAL, "Bytes", "Data read based on Field 3 -- # of sectors read", overflow_value, 0} );
		result.push_back( {INPUT_OUTPUT, NODE, "time/block/reads", topologyPath, cur[3], INCREMENTAL, "ms", "Field 4 -- # of milliseconds spent reading", overflow_value, 0} );

		result.push_back( {INPUT_OUTPUT, NODE, "quantity/block/writes", topologyPath, cur[4], INCREMENTAL, "", "Field 5 -- # of writes completed", overflow_value, 0} );
		result.push_back( {INPUT_OUTPUT, NODE, "quantity/block/writes/merged", topologyPath, cur[5], INCREMENTAL, "", "Field 6 -- # of writes merged", overflow_value, 0} );
		result.push_back( {INPUT_OUTPUT, NODE, "quantity/block/dataWritten", topologyPath, cur[6], INCREMENTAL, "Bytes", "Data written based on Field 7 -- # of sectors written", overflow_value, 0} );
		result.push_back( {INPUT_OUTPUT, NODE, "time/block/writes", topologyPath, cur[7], INCREMENTAL, "ms", "Field 8 -- # of milliseconds spent writing", overflow_value, 0} );

		result.push_back( {INPUT_OUTPUT, NODE, "quantity/block/pendingIOs", topologyPath, cur[8], SAMPLED, "", "Field 9 -- # of I/Os currently in progress", 0, 0} );
		result.push_back( {INPUT_OUTPUT, NODE, "time/block/access", topologyPath, cur[9], INCREMENTAL, "ms", "Field 10 -- # of milliseconds spent doing I/Os", overflow_value, 0} );
		result.push_back( {INPUT_OUTPUT, NODE, "time/block/weighted", topologyPath, cur[10], INCREMENTAL, "ms", "Field 11 -- weighted # of milliseconds spent doing I/Os", overflow_value, 0} );
	}

	return result;
}

static bool isPrefixed( string prefix, string aString ) {
	if( prefix.size() > aString.size() ) return false;
	for( size_t i = prefix.size(); i--; ) if( prefix[i] != aString[i] ) return false;
	return true;
}

static string deviceName2topologyPath( string device ) {
	///@todo TODO: Currently we handle only "sd" and "hd" devices and partitions. Extend to all block devices listed in https://www.kernel.org/doc/Documentation/devices.txt .
	if( isPrefixed( "sd", device ) || isPrefixed( "hd", device ) ) {
		//We have an IDE or SCSI device, possibly with a partition number.
		assert( device.size() >= 3 && device[2] >= 'a' && device[2] <= 'z' );
		std::ostringstream path;
		path << "@localhost/" << device[0] << "d:" << device[2] << ":block-device";
		if( device.size() == 3 ) return path.str();	//It's a whole device
		//It's a partition
		for( size_t i = device.size(); i-->3; ) assert( device[i] >= '0' && device[i] <= '9' );
		path << "/partition:" << &device.c_str()[3];
		return path.str();
	}

	//Handle devices of unknown format
	const char* prefix = "@localhost/";
	const char* postfix = ":master";
	char path[strlen( prefix ) + device.size() + strlen( postfix ) + 1];
	//find the start of the device number
	size_t deviceNumberStart = device.size();
	for(; deviceNumberStart-->1 && device[deviceNumberStart] <= '9' && device[deviceNumberStart] >= '0'; ) ;
	deviceNumberStart++;
	//put the resulting path together
	if( deviceNumberStart == device.size() ) {
		//No device number, this is a master device
		//sprintf() is safe due to the way the array size was calculated.
		sprintf( path, "%s%s%s", prefix, device.c_str(), postfix );
	} else {
		char* curResultChar = path;
		for( size_t i = 0; prefix[i]; i++ ) *curResultChar++ = prefix[i];
		for( size_t i = 0; i < deviceNumberStart; i++ ) *curResultChar++ = device[i];
		*curResultChar++ = ':';
		for( size_t i = deviceNumberStart; i < device.size(); i++ ) *curResultChar++ = device[i];
		*curResultChar++ = 0;
	}
	return string( path );
}

extern "C" {
	void * MONITORING_STATISTICS_PLUGIN_INSTANCIATOR_NAME()
	{
		return new IOstats();
	}
}
