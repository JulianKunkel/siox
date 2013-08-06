#include "../ProcSingleFilePlugin.hpp" //Central module header with template for parsing from statisticproviderplugins and nextTimestep
#include <stdint.h>

#include <map>

using namespace std;

class NetworkStats: public ProcSingleFilePlugin<18>{
protected:

	void timestepLine(int lineNr, vector<string> & entries){
		string name = entries[1];

		if (CurrentValues.find(name) == CurrentValues.end() ){
		cerr << "File " << filename() << "changed while accessing. New field " << name << endl;
		return;
		}

		std::array<StatisticsValue, 1> & Crrnt = CurrentValues[name];

		for(int i=0; i < 1; i++){
			Crrnt[i] = (uint64_t) atoll(entries[i+7].c_str());
		}
		// Nur für etwaige Umrechnung mit fixen Faktoren
		// Crrnt[1] = ((uint64_t) atoll(entries[2+3].c_str())) * factor;
	}


	void initLine(int lineNr, vector<string> & entries) {
		string name = entries[1];		
		CurrentValues[name] = std::array<StatisticsValue, 1>();
	}

	const string filename(){
		return "/sys/class/net/eth0/statistics/rx_packets";
	}

	map<string, std::array<StatisticsValue,1> > CurrentValues;

public:

	virtual list<StatisticsProviderDatatypes> availableMetrics(){
		auto lst = list<StatisticsProviderDatatypes>();

		for(auto iter = CurrentValues.begin(); iter != CurrentValues.end(); iter++){
			string name = iter -> first;
			uint64_t overflow_value = (uint64_t) 1 << 63;

			// Transfer Currentvalues by name to Statistics Array
			std::array<StatisticsValue, 1> & Crrnt = CurrentValues[name];

			lst.push_back({INPUT_OUTPUT, NODE, "Quantity/PacketsReceived", {{"node", LOCAL_HOSTNAME}, {"device", name}}, Crrnt[0], INCREMENTAL, "", "Field 1 -- # of packets received", overflow_value, 0});
		}
		return lst;
	}
};



PLUGIN(NetworkStats)