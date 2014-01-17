#include "../ProcSingleFilePlugin.hpp" //Central module header with template for parsing from statisticproviderplugins and nextTimestep
#include <stdint.h>

#include <map>

using namespace std;

class NetworkStats: public ProcSingleFilePlugin<18> {
	protected:
		map<string, map<StatisticsValue, string> > values;

		void timestepLine( int lineNr, vector<string> & entries ) {
			string name = "PacketsReceived";
			string name2 = "PacketsSent";
			string name3 = "BytesReceived";
			string name4 = "BytesSent";

			if( CurrentValues.find( name ) == CurrentValues.end() ) {
				cerr << "File " << filename() << "changed while accessing. New field " << name << endl;
				return;
			}

			std::array<StatisticsValue, 1> & Crrnt = CurrentValues[name];

			for( int i = 0; i < 1; i++ ) {
				Crrnt[i] = ( uint64_t ) atoll( entries[i].c_str() );
			}

			std::array<StatisticsValue, 1> & Crrnt2 = CurrentValues[name2];

			for( int i = 0; i < 1; i++ ) {
				Crrnt2[i] = ( uint64_t ) atoll( entries[i].c_str() );
			}

			std::array<StatisticsValue, 1> & Crrnt3 = CurrentValues[name3];

			for( int i = 0; i < 1; i++ ) {
				Crrnt3[i] = ( uint64_t ) atoll( entries[i].c_str() );
			}

			std::array<StatisticsValue, 1> & Crrnt4 = CurrentValues[name4];

			for( int i = 0; i < 1; i++ ) {
				Crrnt4[i] = ( uint64_t ) atoll( entries[i].c_str() );
			}


			// Nur für etwaige Umrechnung mit fixen Faktoren
			// Crrnt[1] = ((uint64_t) atoll(entries[2+3].c_str())) * factor;
		}




		void initLine( int lineNr, vector<string> & entries ) {

			string name = "PacketsReceived";
			CurrentValues[name] = std::array<StatisticsValue, 1>();

			string name2 = "PacketsSent";
			CurrentValues[name2] = std::array<StatisticsValue, 1>();

			string name3 = "BytesReceived";
			CurrentValues[name3] = std::array<StatisticsValue, 1>();

			string name4 = "BytesSent";
			CurrentValues[name4] = std::array<StatisticsValue, 1>();


		}

		const string filename() {
			return "/sys/class/net/eth0/statistics/rx_packets";
		}

		const string filename2() {
			return "/sys/class/net/eth0/statistics/tx_packets";
		}

		const string filename3() {
			return "/sys/class/net/eth0/statistics/rx_bytes";
		}

		const string filename4() {
			return "/sys/class/net/eth0/statistics/tx_bytes";
		}



		map<string, std::array<StatisticsValue, 1> > CurrentValues;

	public:

		virtual vector<StatisticsProviderDatatypes> availableMetrics() {
			vector<StatisticsProviderDatatypes> lst;

			for( auto iter = CurrentValues.begin(); iter != CurrentValues.end(); iter++ ) {
				///@todo TODO: The following four lines do not make any sense, as do some other parts of this file. Rework this file.
				string name = iter -> first;
				string name2 = iter -> first;
				string name3 = iter -> first;
				string name4 = iter -> first;
				uint64_t overflow_value = ( uint64_t ) 1 << 63;


				// Transfer Currentvalues by name to Statistics Array
				std::array<StatisticsValue, 1> & Crrnt = CurrentValues[name];
				lst.push_back( {INPUT_OUTPUT, NODE, "Quantity/PacketsReceived", "@localhost", Crrnt[0], INCREMENTAL, "", "Field 1 -- # of packets received", overflow_value, 0} );
				std::array<StatisticsValue, 1> & Crrnt2 = CurrentValues[name2];
				lst.push_back( {INPUT_OUTPUT, NODE, "Quantity/PacketsSent", "@localhost", Crrnt2[0], INCREMENTAL, "", "Field 1 -- # of packets sent", overflow_value, 0} );
				std::array<StatisticsValue, 1> & Crrnt3 = CurrentValues[name3];
				lst.push_back( {INPUT_OUTPUT, NODE, "Quantity/BytesReceived", "@localhost", Crrnt3[0], INCREMENTAL, "", "Field 1 -- # of bytes received", overflow_value, 0} );
				std::array<StatisticsValue, 1> & Crrnt4 = CurrentValues[name4];
				lst.push_back( {INPUT_OUTPUT, NODE, "Quantity/BytesSent", "@localhost", Crrnt4[0], INCREMENTAL, "", "Field 1 -- # of bytes sent", overflow_value, 0} );

			}
			return lst;
		}
};

extern "C" {
	void * MONITORING_STATISTICS_PLUGIN_INSTANCIATOR_NAME()
	{
		return new NetworkStats();
	}
}
