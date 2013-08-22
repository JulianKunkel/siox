#include <assert.h>
#include <iostream>

#include <core/component/Component.hpp>

#define STRINGIFY_HELPER(TOKEN) #TOKEN
#define STRINGIFY(TOKEN) STRINGIFY_HELPER(TOKEN)
#define COMPILE_TIME_NAME 

int main(int argc, char const *argv[]){
	std::string compileTimeName(STRINGIFY(COMPONENT_INSTANCIATOR_NAME(foo)));
	std::string runTimeName(core::Component::component_instanciator_name("foo"));

	if(compileTimeName.compare(runTimeName)) {
		std::cout << "Error: compile time and run time names differ\n"
		        "    compile time name = \"" << compileTimeName << "\"\n"
		        "        run time name = \"" << runTimeName << "\"\n";
		abort();
	}

	std::cout << "OK\n";
	return 0;
}


