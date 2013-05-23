
#include <stdlib.h> 
#include <time.h> 

#include <iostream>
#include <sstream>
#include <vector>

#include <thread>
#include <chrono>

#include "../ActivityMultiplexer_Impl1.hpp"
#include "../ActivityMultiplexerListener_Impl1.hpp"


ActivityMultiplexer * m1 = new ActivityMultiplexer_Impl1();

// population settings
const int num_listeners = 5;
const int num_a_producers = 70;
const int num_l_producers = 5;
const int num_l_consumers = 5;

// behavior adjustments
int producer_delay_to_produce = 30;  // ms, used by l and a producers
const int lconsumer_delay_to_consume = 50;
const int max_activities_per_producer = 10; // total max before termination 

// thread behaivors
void a_producer() {
	for (int i = 0; i < max_activities_per_producer; ++i)
	{
		m1->Log(new Activity());
		std::this_thread::sleep_for(std::chrono::milliseconds(producer_delay_to_produce));
	}
}

// activity consumer is part of the multiplexer (notifier to be specific spawns a thread)


void l_producer() {
	for (int i = 0; i < max_activities_per_producer; ++i)
	{
		//ActivityMultiplexerListener * listener = new ActivityMultiplexerListener_Impl1();
		//m1->registerListener(listener)
		std::this_thread::sleep_for(std::chrono::milliseconds(producer_delay_to_produce));
	}
}

void l_consumer() {
	for (int i = 0; i < max_activities_per_producer; ++i)
	{
		//ActivityMultiplexerListener * listener = m1->getRandomListener();
		//m1->registerListener(listener)
		dynamic_cast<ActivityMultiplexer_Impl1*>(m1)->unregisterRandom();
		std::this_thread::sleep_for(std::chrono::milliseconds(lconsumer_delay_to_consume));
	}
}


int main(int argc, char const *argv[])
{
	srand (time(NULL));

	// create a few listeners for init testing
	ActivityMultiplexerListener * l0 = new ActivityMultiplexerListenerSync_Impl1();
	ActivityMultiplexerListener * l1 = new ActivityMultiplexerListenerSync_Impl1();
	ActivityMultiplexerListener * l2 = new ActivityMultiplexerListenerSync_Impl1();
	ActivityMultiplexerListener * l3 = new ActivityMultiplexerListenerAsync_Impl1(); // for async
	ActivityMultiplexerListener * l4 = new ActivityMultiplexerListenerAsync_Impl1(); // for async
	ActivityMultiplexerListener * l5 = new ActivityMultiplexerListenerAsync_Impl1(); // for async

	// basic init
	std::cout << "Init and register 6 listeners test notify. \n" << std::endl;

	// log an activity
	m1->Log(new Activity());

	// register sync our listeners to m1
	m1->registerListener(l0); // not async
	m1->registerListener(l1); // not async
	m1->registerListener(l2); // not async

	// register async listeners
	m1->registerListener(l3); // async
	m1->registerListener(l4); // async
	m1->registerListener(l5); // async

	// log another activity & expect activity in listeners	
	m1->Log(new Activity());

	// unregister a listner
	std::cout << "\nUnregister l1, and test notify again. \n" << std::endl;
	m1->unregisterListener(l1);

	// expect 2 to get passed
	m1->Log(new Activity());
	m1->Log(new Activity());

	std::cout << std::endl;




	std::cout << "Start producing Activities by threads" << std::endl;
    std::cout << "main: starting threads" << std::endl;  
	std::cout << std::endl;

	// keep track of every kind individually
	std::vector<std::thread> a_producers;
	std::vector<std::thread> l_producers;
	std::vector<std::thread> l_consumers;

	// create the threads
	for(int i = 0; i < num_a_producers; ++i)
		a_producers.push_back(std::thread(a_producer)); // activity producers

	for(int i = 0; i < num_l_producers; ++i)
	{
		// registers listeners
		l_producers.push_back(std::thread(l_producer)); 
	}

	for(int i = 0; i < num_l_consumers; ++i)
	{
		// unregisters listeners
		l_consumers.push_back(std::thread(l_consumer));
	}

    std::cout << "\nmain: waiting for threads\n\n";  


	// emitt activities
	for(auto& t : a_producers)
		t.join();

	// randomly add listeners
	for(auto& t : l_producers)
		t.join();

	// randomly remove listeners
	for(auto& t : l_consumers)
		t.join();

    std::cout << std::endl << "main: done" << std::endl;
	


	// give detached thread a chance to catch up
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));


	return 0;
}
