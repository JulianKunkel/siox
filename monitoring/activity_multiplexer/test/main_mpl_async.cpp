
#include <stdlib.h> 
#include <time.h> 

#include <iostream>
#include <sstream>
#include <vector>

#include <boost/thread.hpp>
#include <boost/date_time.hpp>

#include "../ActivityMultiplexer_Impl1.hpp"
#include "../ActivityMultiplexerListener_Impl1.hpp"


ActivityMultiplexer * m1 = new ActivityMultiplexer_Impl1();

// populations
const int num_listeners = 5;
const int num_a_producers = 5;
const int num_l_producers = 5;
const int num_l_consumers = 5;

// behavior adjustments
const int producer_delay_to_produce = 0.3;
const int max_activities_per_producer = 3;

// storage
std::vector<ActivityMultiplexerListener*> ls; // the only global as l_producer/l_consumers use it


// thread behaivors
void a_producer() {
    boost::posix_time::seconds delay(producer_delay_to_produce);  
	for (int i = 0; i < max_activities_per_producer; ++i)
	{
		m1->Log(new Activity());
		boost::this_thread::sleep(delay);  
	}
}

void l_producer() {
	boost::posix_time::seconds delay(producer_delay_to_produce);  
	for (int i = 0; i < max_activities_per_producer; ++i)
	{
		//ActivityMultiplexerListener * listener = new ActivityMultiplexerListener_Impl1();
		//m1->registerListener(listener)
		boost::this_thread::sleep(delay);  
	}
}

void l_consumer() {
	boost::posix_time::seconds delay(producer_delay_to_produce);  
	for (int i = 0; i < max_activities_per_producer; ++i)
	{
		//ActivityMultiplexerListener * listener = m1->getRandomListener();
		//m1->registerListener(listener)
		boost::this_thread::sleep(delay);  
	}
}


int main(int argc, char const *argv[])
{
	srand (time(NULL));

	// create finite number of listeners
	for(int i = 0; i < num_listeners; ++i)
	{
		//ls.push_back(new ActivityMultiplexerNotifier_Impl1());
	}

	// create a few listeners
	ActivityMultiplexerListener * l1 = new ActivityMultiplexerListener_Impl1();
	ActivityMultiplexerListener * l2 = new ActivityMultiplexerListener_Impl1();
	ActivityMultiplexerListener * l3 = new ActivityMultiplexerListener_Impl1();
	ActivityMultiplexerListener * l4 = new ActivityMultiplexerListener_Impl1(); // for async
	ActivityMultiplexerListener * l5 = new ActivityMultiplexerListener_Impl1(); // for async
	ActivityMultiplexerListener * l6 = new ActivityMultiplexerListener_Impl1(); // for async

	// is the interface working?
//	l2->Notify(new Activity());
//	l3->Notify(new Activity());

	// introduce a multiplexer

	// log an activity
	m1->Log(new Activity());

	// register sync our listeners to m1
	m1->registerListener(l1, false); // not async
	m1->registerListener(l2, false); // not async
	m1->registerListener(l3, false); // not async

	// register async listeners
	m1->registerListener(l4, true); // async
	m1->registerListener(l5, true); // async
	m1->registerListener(l6, true); // async

	// log another activity & expect activity in listeners	
	m1->Log(new Activity());

	// unregister a listner
	m1->unregisterListener(l2, false);

	// expect 2 to get passed
	m1->Log(new Activity());
	m1->Log(new Activity());
	m1->Log(new Activity());
	m1->Log(new Activity());

	std::cout << std::endl;

	// keep track of every kind individually
	std::vector<boost::thread> a_producers;
	std::vector<boost::thread> l_producers;
	std::vector<boost::thread> l_consumers;
	
	// boost threading experiment
    std::cout << "main: starting threads" << std::endl;  
	std::cout << std::endl;


	// create the threads
	for(int i = 0; i < num_a_producers; ++i)
		a_producers.push_back(boost::thread(a_producer));

	for(int i = 0; i < num_l_producers; ++i)
		l_producers.push_back(boost::thread(l_producer));

	for(int i = 0; i < num_l_consumers; ++i)
		l_consumers.push_back(boost::thread(l_consumer));

    std::cout << "main: waiting for threads" << std::endl;  

	for(auto& t : a_producers)
		t.join();

	for(auto& t : l_producers)
		t.join();

	for(auto& t : l_consumers)
		t.join();

    std::cout << "main: done" << std::endl;
	

	return 0;
}
