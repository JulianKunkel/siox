#include <core/autoconfigurator/ConfigurationProvider.hpp>
#include <core/container/container-serializer.hpp>

#include <fstream>
#include <iostream>
#include <sstream>
#include <map>

#include <assert.h>

using namespace core;
using namespace std;

class FileConfigurationProvider : public ConfigurationProvider{
public:

	void connect(string & configuration_filename){
		if(configuration_filename == ""){
			// TODO use/try the default configuration files
		}
		ifstream fin(configuration_filename);
		assert (fin);

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

	std::string getConfiguration(string & type, string & matchingRules){
		string what = type + " " + matchingRules;
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
