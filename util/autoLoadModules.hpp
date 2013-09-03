#ifndef UTIL_AUTO_LOAD_MODULES_H
#define UTIL_AUTO_LOAD_MODULES_H

#include <string>

#include <core/component/Component.hpp>
#include <core/autoconfigurator/AutoConfigurator.hpp>

using namespace std;
using namespace core;

namespace util{
	vector<Component *> LoadConfiguration(AutoConfigurator ** outConfigurator, ComponentRegistrar * registrar);

	string getHostname();
}


#endif