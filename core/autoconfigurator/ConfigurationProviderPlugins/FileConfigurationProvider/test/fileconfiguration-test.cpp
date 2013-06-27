#include <assert.h>
#include <iostream>

#include <core/autoconfigurator/AutoConfigurator.hpp>
#include <core/component/Component.hpp>
#include <core/component/component-macros.hpp>

using namespace core;

/**
 * Test objects to create/init from configuration file.
 */
class MyChildModuleOptions : public ComponentOptions{
public:
	string name;
	int value;

	SERIALIZE_CONTAINER(MEMBER(name) MEMBER(value) )
};
CREATE_SERIALIZEABLE_CLS(MyChildModuleOptions)

class MyChildModule : public Component{
public:	
	void init(ComponentOptions * options){
		MyChildModuleOptions * o = (MyChildModuleOptions*) options;
		cout << "Child" << o->name << " " << o->value << endl;

		delete(o);
	}

	ComponentOptions * get_options(){
		return new MyChildModuleOptions();
	}

	void shutdown(){

	}
};

class MyParentModuleOptions : public ComponentOptions{
public:
	string pname;
	int pvalue;

	Module<MyChildModule> submodule;

	MyParentModuleOptions(){

	}

	SERIALIZE_CONTAINER(MEMBER(pname) MEMBER(pvalue) MEMBER(submodule) )
};
CREATE_SERIALIZEABLE_CLS(MyParentModuleOptions)

class MyParentModule : public Component{
public:
	void init(ComponentOptions * options){
		MyParentModuleOptions * o = (MyParentModuleOptions*) options;
		cout << "Parent " << o->pname << " " << o->pvalue << " " << o->submodule.instance << endl;

		// free the options afterwards
		delete(o);
	}

	ComponentOptions * get_options(){
		return new MyParentModuleOptions();
	}

	void shutdown(){

	}
};




int main(){
	AutoConfigurator * a = new AutoConfigurator("FileConfigurationProvider", "", "test.config");


	MyChildModule child;
	MyParentModule parent;

	map<string, string> optional;

	a->LoadConfiguration(parent, "daemon", optional);

	cout << a->DumpEmptyConfiguration(parent) << endl;

	cout << a->DumpEmptyConfiguration(child) << endl;

	return 0;
}