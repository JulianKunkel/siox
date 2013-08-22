#include <iostream>

#include <core/container/container-serializer.hpp>
#include <core/component/component-macros.hpp>

using namespace core;
using namespace std;

class MyContainer: public Container {
	public:
		string name;
		int value;

		SERIALIZE( MEMBER( name ) MEMBER( value ) PARENT_CLASS( Container ) )
};
CREATE_SERIALIZEABLE_CLS( MyContainer )



class MyContainerChild: public MyContainer {
	public:
		string str;
		list<string> elems;

		SERIALIZE( MEMBER( str ) MEMBER( elems ) PARENT_CLASS( MyContainer ) )
};
CREATE_SERIALIZEABLE_CLS( MyContainerChild )


int main()
{
	ContainerSerializer * cs = new ContainerSerializer();
	MyContainerChild * test = new MyContainerChild();
	test->name = "testname";
	test->value = 24;
	test->elems.push_front( "elem1" );
	test->elems.push_front( "elem2" );

	string val = cs->serialize( test );
	cout << val << endl;

	MyContainerChild * parsedContainer;
	parsedContainer = dynamic_cast<MyContainerChild *>( cs->parse( val ) );
	cout << parsedContainer->name << endl;
}
