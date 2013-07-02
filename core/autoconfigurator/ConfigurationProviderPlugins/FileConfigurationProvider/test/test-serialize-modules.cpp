#include "test-serialize-modules.hpp"

using namespace core;


CREATE_SERIALIZEABLE_CLS(MyChildModuleOptions)
CREATE_SERIALIZEABLE_CLS(MyParentModuleOptions)



void MyChildModule::init(ComponentOptions * options){
	assert(options);
	this->options = (MyChildModuleOptions*) options;	
}

ComponentOptions * MyChildModule::get_options(){
	return new MyChildModuleOptions();
}

void MyChildModule::shutdown(){
	cout << "Shutting down MyChildModule" << endl;
}



void MyParentModule::init(ComponentOptions * options){
	assert(options);
	this->options = (MyParentModuleOptions*) options;
}

ComponentOptions * MyParentModule::get_options(){
	return new MyParentModuleOptions();
}

void MyParentModule::shutdown(){
	cout << "Shutting down MyParentModule" << endl;
}

extern "C"{\
void * get_instance_parent() { return new MyParentModule(); }\
}


extern "C"{\
void * get_instance_child() { return new MyChildModule(); }\
}
