#include <util/referenceCounting.hpp>

int allocCount = 0;

class Foo : public ReferencedObject {
	public:
		int foo;
		Foo() {
			allocCount++;
		}
		~Foo() {
			allocCount--;
		}
};

class Bar : public Foo {
	public:
		int bar;
};

int main( int argc, char const * argv[] ) throw() {
	{
		Release<Bar> myBar(new Bar());
		assert(allocCount == 1);
		Retain<Bar> retainedBar = myBar;
		Retain<Foo> test1 = myBar;
		Retain<Foo> test2 = retainedBar;
		test1 = myBar;
		test2 = retainedBar;
		test1 = new Foo();
		test1->release();
		assert(allocCount == 2);
		test1 = new Bar();
		test1->release();
		assert(allocCount == 2);
		test2->foo = 3;
		assert(myBar->foo == 3);
		assert(test2 == myBar);
	}
	assert(allocCount == 0);
}

