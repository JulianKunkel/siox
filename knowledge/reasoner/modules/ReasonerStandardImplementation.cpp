#include <knowledge/reasoner/ReasonerImplementation.hpp>

#include <core/component/Component.hpp>

using namespace core;

namespace knowledge{

class ReasonerStandardImplementation : public Reasoner{
private:

	

protected:

	ComponentOptions * AvailableOptions(){
		return new ComponentOptions();
	}


public:
	virtual void init(){

	}
};

} // namespace knowledge


COMPONENT(knowledge::ReasonerStandardImplementation)
