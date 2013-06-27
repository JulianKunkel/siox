#include <core/autoconfigurator/ConfigurationProvider.hpp>

#include <iostream>

using namespace core;
using namespace std;

class FileConfigurationProvider : public ConfigurationProvider{
public:

	void connect(string configuration_filename){
		if(configuration_filename == ""){
			// TODO use/try the default configuration files
		}
		cout << "parsing from: " << configuration_filename << endl;		
	}

	std::string getConfiguration(std::string type, std::map<std::string, std::string> & optional_matchings_priority){
		return "";
	}

private:

};



COMPONENT(FileConfigurationProvider)
