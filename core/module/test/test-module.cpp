#include "test-module.hpp"

class HelloWorldImpl1 : public HelloWorld {
	public:
		virtual std::string testfunc(){
			return "Hello World!";
		}
};


// module part.
extern "C" {
	void * get_instance_hello_world()
	{
		return new HelloWorldImpl1();
	}
}
