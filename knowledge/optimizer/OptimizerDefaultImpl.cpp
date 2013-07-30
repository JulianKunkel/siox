#include <knowledge/optimizer/OptimizerImplementation.hpp>

using namespace core;
using namespace knowledge;

class OptimizerDefaultImpl : Optimizer{
protected:
	ComponentOptions * AvailableOptions(){
		return new ComponentOptions();
	}
public:
	void init(){

	}

	void shutdown(){

	}
};

COMPONENT(OptimizerDefaultImpl)