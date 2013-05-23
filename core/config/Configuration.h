#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <fstream>
#include <iostream>
#include <string>
#include <yaml-cpp/yaml.h>

#include "SioxLogger.h"

struct Module {
	std::string name;
	std::string interface;
};

inline void operator >> (const YAML::Node &node, Module &module) {
	node["module"] >> module.name;
	node["interface"] >> module.interface;
};

class Configuration {
	
public:
	
	Configuration(const std::string &filename);
	
};

#endif
