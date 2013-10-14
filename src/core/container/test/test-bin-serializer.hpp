#include <inttypes.h>
#include <vector>
#include <string>
#include <list>

using namespace std;

namespace test{

//@serializable
struct TestC2{
	int32_t a;
};

//@serializable
struct TestContainer{
	int32_t a;
	int32_t b;
	string myVar;
	long double f;

	vector<TestC2> v;

	list<int32_t> listInt;

	int32_t * pa;

	TestC2 c2;
};

}
