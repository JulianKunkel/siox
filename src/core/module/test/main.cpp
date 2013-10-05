#include <assert.h>

#include <core/module/ModuleLoader.hpp>
#include <iostream>

#include "test-module.hpp"

using namespace std;

#define STRINGIFY_HELPER(TOKEN) #TOKEN
#define STRINGIFY(TOKEN) STRINGIFY_HELPER(TOKEN)

int main( int argc, char ** argv )
{
	std::string compileTimeName( STRINGIFY( MODULE_INSTANCIATOR_NAME( foo ) ) );
	std::string runTimeName( core::ModuleInterface::instanciator_name( "foo" ) );

	if( compileTimeName.compare( runTimeName ) ) {
		std::cout << "Error: compile time and run time names differ\n"
		          "    compile time name = \"" << compileTimeName << "\"\n"
		          "        run time name = \"" << runTimeName << "\"\n";
		exit( 1 );
	}

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
		assert( false );
	} catch( core::ModuleError & e ) {
		cerr << "As expected I caught the error: " << e.what() << endl;
	}

	return 0;
}
