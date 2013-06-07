#include "Configuration.h"

Configuration::Configuration(const std::string &filename)
{
#ifndef NDEBUG
	logger->log(Logger::DEBUG, "Opening configuration file %s.", 
		    filename.c_str());
#endif	
	try {
		std::ifstream fin(filename);
		YAML::Parser parser(fin);
		
		YAML::Node doc;
		parser.GetNextDocument(doc);

		for (unsigned i = 0; i < doc.size(); i++) {
			
			Module module;
			doc[i] >> module;
			
			modules_.insert(std::pair<std::string, Module>(module.name, module));
		}
		
	} catch (YAML::ParserException &e) {
		logger->log(Logger::ERR, e.what());
	}
}


Module Configuration::get_module_conf(const std::string &modname)
{
	return modules_[modname];
}

