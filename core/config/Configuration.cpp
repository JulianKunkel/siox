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
#ifndef NDEBUG
			logger->log(Logger::DEBUG, "Loaded configuration for module %s with interface %s.",
				    module.name.c_str(), module.interface.c_str());
#endif
		}
		
	} catch (YAML::ParserException &e) {
		logger->log(Logger::ERR, e.what());
	}
}
