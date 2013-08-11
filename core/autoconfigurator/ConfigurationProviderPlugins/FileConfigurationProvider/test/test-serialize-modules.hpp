#include <string>

#include <core/component/Component.hpp>
#include <core/component/component-macros.hpp>


using namespace std;
using namespace core;

class MyChildModuleOptions : public ComponentOptions{
public:
	string name = "ChildOpt1";
	int value = 3;

	SERIALIZE_CONTAINER(MEMBER(name) MEMBER(value) )
};


class MyParentModuleOptions : public ComponentOptions{
public:
	string pname = "Test";
	int pvalue = -1;
	ComponentReference childInterface;

	SERIALIZE_CONTAINER(MEMBER(pname) MEMBER(pvalue) MEMBER(childInterface))
};

class MyParentModule : public Component{
public:
	ComponentOptions * AvailableOptions();
	void init();
};

class MyChildModule : public Component{
public:
	ComponentOptions * AvailableOptions();
	void init();
};

