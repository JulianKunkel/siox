#include <assert.h>

#include <core/module/ModuleLoader.hpp>
#include <iostream>

#include "test-module.hpp"

using namespace std;

int main( int argc, char ** argv )
{
	try {
		HelloWorld * hw = core::module_create_instance<HelloWorld>( "", "test-module", "hello_world" );
		cout << hw->testfunc() << endl;
		delete( hw );
	} catch( core::ModuleError & e ) {
		cerr << e.what() << endl;
		exit( 1 );
	}

	try {
		HelloSaturn * hw = core::module_create_instance<HelloSaturn>( "", "test-module", "hello_world" );
		cout << hw->alternativeFunc() << endl;
		delete( hw );
		assert(false);
	} catch( core::ModuleError & e ) {
		cerr << "As expected I caught the error: " << e.what() << endl;
	}	

	return 0;
}
