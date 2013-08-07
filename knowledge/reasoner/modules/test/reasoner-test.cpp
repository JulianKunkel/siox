#include <assert.h>
#include <iostream>
#include <list>

#include <knowledge/reasoner/Reasoner.hpp>
#include <core/module/module-loader.hpp>


using namespace std;
using namespace knowledge;


int main(int argc, char const *argv[]){

	// Obtain a FileOntology instance from module loader
	Reasoner * r = core::module_create_instance<Reasoner>("", "ReasonerStandardImplementation", KNOWLEDGE_REASONER_INTERFACE);
	assert( r != nullptr);

	r->init();

	delete(r);

	return 0;
}


