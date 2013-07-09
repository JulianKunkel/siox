#include <core/autoconfigurator/ConfigurationProviderImplementation.hpp>
#include <core/container/container-serializer.hpp>

#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <boost/algorithm/string.hpp>

#include <assert.h>

using namespace core;
using namespace std;


class FileConfigurationProvider : public ConfigurationProvider{
public:

	void connect(string & configuration_filename){
		// try for existance of potentially multiple files multiple configuration_filename are separated by ":" or tab.

		vector<string> filelist;
		boost::split(filelist, configuration_filename, boost::is_any_of(":\t"));

		ifstream fin;
		for(auto it = filelist.begin(); it != filelist.end(); ++it) {
			fin.open(configuration_filename);
			if(fin.good()){
				break;
			}
		}
		if(! fin.good()){
			throw "Error configuration in files is not readable";
		}

		string line;
		stringstream data;
		string configSection;
		while(getline(fin, line)){
			if(line.length() < 3){
				continue;
			}
			if(line.compare(0, 7, "#CONFIG") == 0){
				parseData(configSection, data.str());
				data.str("");
				configSection = line;
			}else{
				data << line << endl;
			}
		}
		parseData(configSection, data.str());

		fin.close();
	}

	string getConfiguration(string & type, string & matchingRules){
		string what = type + (matchingRules.length() > 0 ? " " + matchingRules : "");
		return configurationSections[what];
	}

private:
	// this map contains the key <type, all optional info concatenated using "/"> and value: configuration
	map<string, string> configurationSections;

	void parseData(string & configSection, string && data){
		if(configSection.length() < 4){
			return;
		}
		configSection = configSection.substr(8); // strip: #CONFIG 
		configurationSections[configSection] = data;
	}
};

		// auto itr = configSection.begin();
		
		// string key_str;
		// stringstream current;

		// bool masked = false;
		// bool key = true;
		// string
		// while(itr != configSection.end()){
		// 	char c = *itr;
		// 	if (key){
		// 		if( c == ' '){

		// 		}else if( c == '=' ){
		// 			key_str = current.str();
		// 			current.str("");
		// 			itr++; // should be a "
		// 			key = false;
		// 		}else{
		// 			current << c;
		// 		}
		// 	}else{
		// 		if( c == '"'  && ! masked){
		// 			string value = current.str();
		// 			// cout << key_str << ":" << value << endl;

		// 			current.str("");
		// 			key = true;
		// 		}else if(c == '\\' && ! masked){
		// 			masked = true;
		// 		}else{
		// 			masked = false;
		// 			current << c;
		// 		}
		// 	}
		// 	itr++;
		// }

COMPONENT(FileConfigurationProvider)

