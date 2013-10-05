#include "test-serializer.hpp"

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
