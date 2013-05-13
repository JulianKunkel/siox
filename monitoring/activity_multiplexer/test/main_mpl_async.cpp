#include <iostream>
#include <boost/thread.hpp>
#include <boost/date_time.hpp>

#include "ActivityMultiplexer_Impl1.hpp"
#include "ActivityMultiplexerListener_Impl1.hpp"

void work()  
{ 
	static int id = 0;
	id++;

	int cid = id;

    boost::posix_time::seconds workTime(3);  
    std::cout << "Worker-" << cid << ": running" << std::endl;  
    // Pretend to do something useful...  
    boost::this_thread::sleep(workTime);  
    std::cout << "Worker-"<< cid <<": finished" << std::endl;  
} 

int main(int argc, char const *argv[])
{

	// create a few listeners
	ActivityMultiplexerListener * l1 = new ActivityMultiplexerListener_Impl1();
	ActivityMultiplexerListener * l2 = new ActivityMultiplexerListener_Impl1();
	ActivityMultiplexerListener * l3 = new ActivityMultiplexerListener_Impl1();
	ActivityMultiplexerListener * l4 = new ActivityMultiplexerListener_Impl1(); // for async
	ActivityMultiplexerListener * l5 = new ActivityMultiplexerListener_Impl1(); // for async
	ActivityMultiplexerListener * l6 = new ActivityMultiplexerListener_Impl1(); // for async

	// is the interface working?
//	l1->Notify(new Activity());
//	l2->Notify(new Activity());
//	l3->Notify(new Activity());

	// introduce a multiplexer
	ActivityMultiplexer * m1 = new ActivityMultiplexer_Impl1();

	// log an activity
	m1->Log(new Activity());

	// register sync our listeners to m1
	m1->registerListener(l1, false); // not async
	m1->registerListener(l2, false); // not async
	m1->registerListener(l3, false); // not async

	// register async listeners
	m1->registerListener(l4, true); // not async
	m1->registerListener(l5, true); // not async
	m1->registerListener(l6, true); // not async

	// log another activity & expect activity in listeners	
	m1->Log(new Activity());

	// unregister a listner
	m1->unregisterListener(l2, false);

	// expect 2 to get passed
	m1->Log(new Activity());
	m1->Log(new Activity());
	m1->Log(new Activity());
	m1->Log(new Activity());
	m1->Log(new Activity());


	std::cout << std::endl;

	// boost threading experiment
    std::cout << "main: startup" << std::endl;  
    boost::thread t1(work);  
    boost::thread t2(work);  
    std::cout << "main: waiting for thread" << std::endl;  
    t1.join();  
    t2.join();  
    std::cout << "main: done" << std::endl;  


	return 0;
}
