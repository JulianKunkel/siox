#include <iostream>

#include <core/container/container-serializer.hpp>
#include <core/container/container-macros.hpp>

using namespace core;

class MyContainer: public Container{
public:
	string name;
	int value;

	SERIALIZE(MEMBER(name) MEMBER(value) )
};


class MyContainerChild: public MyContainer{
public:
	string child;
	list<string> elems;
	
	SERIALIZE(MEMBER(child) MEMBER(elems) PARENT_CLASS(MyContainer) )
};


int main(){
	ContainerSerializer<MyContainerChild> * cs = new ContainerSerializer<MyContainerChild>();
	MyContainerChild * test = new MyContainerChild();
        test->name = "testname";
	test->value = 24;
	test->elems.push_front("elem1");
	test->elems.push_front("elem2");
	
 	string val = cs->serialize(*test);
	cout << val << endl;
	
	MyContainerChild parsedContainer;
 	cs->parse(parsedContainer, val);
	cout << parsedContainer.name << endl;
}
