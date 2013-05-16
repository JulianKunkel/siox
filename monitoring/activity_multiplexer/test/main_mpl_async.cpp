#include <iostream>
#include <sstream>
#include <vector>

#include <boost/thread.hpp>
#include <boost/date_time.hpp>

#include "../ActivityMultiplexer_Impl1.hpp"
#include "../ActivityMultiplexerListener_Impl1.hpp"


ActivityMultiplexer * m1 = new ActivityMultiplexer_Impl1();

/*
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
} */

const int num_a_producers = 5;
const int num_l_producers = 5;
const int num_l_consumers = 5;

const int producer_delay_to_produce = 0.3;
const int max_activities_per_producer = 5;

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

	// keep track of every kind individually
	std::vector<boost::thread> a_producers;
	std::vector<boost::thread> l_producers;
	std::vector<boost::thread> l_consumers;
	
	// boost threading experiment
    std::cout << "main: starting threads" << std::endl;  

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
