#include <core/module/ModuleLoader.hpp>
#include <iostream>

#include "test-module.hpp"

using namespace std;

int main(int argc, char ** argv){
	try{
		HelloWorld * hw = core::module_create_instance<HelloWorld>("", "test-module", "hello_world");
		cout << hw->testfunc() << endl;
		delete(hw);
	}catch(core::ModuleError& e){
		cerr << e.what() << endl;
		exit(1);
	}

	return 0;
}
