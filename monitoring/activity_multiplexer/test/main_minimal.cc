#include "../ActivityMultiplexer_Impl1.hpp"
#include "../ActivityMultiplexerListener_Impl1.hpp"


int main(int argc, char const *argv[])
{

	ActivityMultiplexer * m1 = new ActivityMultiplexer_Impl1();

	// create a few listeners for init testing
	ActivityMultiplexerListener * l0 = new ActivityMultiplexerListenerSync_Impl1();
	ActivityMultiplexerListener * l2 = new ActivityMultiplexerListenerAsync_Impl1(); // for async

	// register listeners to m1
	m1->registerListener(l0); // not async
	m1->registerListener(l1);

	// log an activity
	m1->Log(new Activity());

	// unregister a listner
	m1->unregisterListener(l1);

	// expect only one listener to ne notified
	m1->Log(new Activity());



	return 0;
}
