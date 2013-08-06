#include <iostream>
#include <list>

#include <knowledge/optimizer/Optimizer.hpp>
#include <core/module/module-loader.hpp>

using namespace std;
using namespace knowledge;


int main(int argc, char const *argv[]){
	Optimizer * m = core::module_create_instance<Optimizer>("", "OptimizerDefaultImpl", KNOWLEDGE_OPTIMIZER_INTERFACE);

	m->init();

	delete(m);

	return 0;
}



