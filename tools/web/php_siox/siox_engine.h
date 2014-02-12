#ifndef SIOX_ENGINE_H
#define SIOX_ENGINE_H

#include <iostream>

#include <util/autoLoadModules.hpp>
#include <util/ReporterHelper.hpp>

class SIOX_Engine {
public:
	SIOX_Engine();
	~SIOX_Engine();
	
	unsigned long get_activity_count();
	
private:
	ComponentRegistrar *registrar;
};

#endif // SIOX_ENGINE_H