#include <iostream>
#include <list>

#include <core/container/container-serializer.hpp>
#include <core/component/Component.hpp>
#include <core/component/ComponentReference.hpp>

using namespace core;
using namespace std;

//@serializable
class MyContainer: public Container {
	public:
		string name;
		int value;
};

//@serializable
class MyContainerChild: public MyContainer {
	public:
		string str;
		list<string> elems;
};

