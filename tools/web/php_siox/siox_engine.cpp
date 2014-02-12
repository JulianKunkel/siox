#include "siox_engine.h"

SIOX_Engine::SIOX_Engine() 
{
	registrar = new ComponentRegistrar();
	AutoConfigurator *configurator = nullptr;
	vector<Component *> loadedComponents = util::LoadConfiguration(&configurator, registrar);
}

SIOX_Engine::~SIOX_Engine() 
{
	util::invokeAllReporters(registrar);
	registrar->shutdown();
}

void SIOX_Engine::test()
{
	std::cout << "Test" << std::endl;
}
