#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <utility>
#include <yaml-cpp/yaml.h>

#include "SioxLogger.hpp"

struct Module {
	std::string name;
	std::string interface;
	std::list<std::string> options;
	std::list<Module> modules;
};


inline void operator >> (const YAML::Node &node, Module &module) 
{
	node["module"] >> module.name;
	node["interface"] >> module.interface;
	
#ifndef NDEBUG
	logger->log(Logger::DEBUG, "Loaded configuration for module %s with interface %s.",
		    module.name.c_str(), module.interface.c_str());
#endif

	if (const YAML::Node *options = node.FindValue("options")) {

		if (options[0].Type() == YAML::NodeType::Sequence) {
		
			const YAML::Node &opt_vals = options[0];
			for (unsigned i = 0; i < opt_vals.size(); i++) {
				
				std::string value;
				opt_vals[i] >> value;
				module.options.push_back(value);
#ifndef NDEBUG
				logger->log(Logger::DEBUG, "Option: %s.", 
					    value.c_str());
#endif
			}
			
		}

	}
	
	if (const YAML::Node *modules = node.FindValue("modules")) 
		
		if (modules[0].Type() == YAML::NodeType::Sequence) {
			
			const YAML::Node &mod_vals = modules[0];
			for (unsigned i = 0; i < mod_vals.size(); i++) {
				Module m;
				mod_vals[i] >> m;
				module.modules.push_back(m);
			}
		}
};


class Configuration {
	
public:
	Configuration(const std::string &filename);
	Module get_module_conf(const std::string &modname);
	
private:
	std::map<std::string, Module> modules_;
};

#endif
