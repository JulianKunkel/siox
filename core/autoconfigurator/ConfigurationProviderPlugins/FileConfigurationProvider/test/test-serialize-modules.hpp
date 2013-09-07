#include <string>

#include <core/component/Component.hpp>
#include <core/component/ComponentReference.hpp>

using namespace std;
using namespace core;

//@serializable
class MyChildModuleOptions : public ComponentOptions {
	public:
		string name = "ChildOpt1";
		int value = 3;
};


//@serializable
class MyParentModuleOptions : public ComponentOptions {
	public:
		string pname = "Test";
		int pvalue = -1;
		ComponentReference childInterface;
};

class MyParentModule : public Component {
	public:
		ComponentOptions * AvailableOptions();
		void init();
};

class MyChildModule : public Component {
	public:
		ComponentOptions * AvailableOptions();
		void init();
};

