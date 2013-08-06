#include "test-serialize-modules.hpp"
#include <core/component/ComponentReferenceSerializable.hpp>


using namespace core;


void MyChildModule::init(){
}

ComponentOptions * MyChildModule::AvailableOptions(){
	return new MyChildModuleOptions();
}

void MyParentModule::init(){
}

ComponentOptions * MyParentModule::AvailableOptions(){
	return new MyParentModuleOptions();
}

CREATE_SERIALIZEABLE_CLS(MyChildModuleOptions)
CREATE_SERIALIZEABLE_CLS(MyParentModuleOptions)


extern "C"{\
void * get_instance_parent() { return new MyParentModule(); }\
}


extern "C"{\
void * get_instance_child() { return new MyChildModule(); }\
}
