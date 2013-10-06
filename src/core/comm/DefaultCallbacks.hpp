#include <unistd.h>

#include <core/comm/CommunicationModule.hpp>

using namespace std;

namespace core{

//@serializable
struct DefaultConnectionCallbackOptions{
	int retries = 3; 
	int waitTime_ms = 100;
};

class DefaultConnectionCallbackErrorCB{
public:
	void connectionErrorCB(CommunicationError error);
};

class DefaultConnectionCallback: public ConnectionCallback{
public:
	DefaultConnectionCallback(const DefaultConnectionCallbackOptions & options, DefaultConnectionCallbackErrorCB & cb) : options(options), cb(cb){}

	void connectionErrorCB(ServiceClient & connection, CommunicationError error){
		if(retries == options.retries){
			// we discard all pending messages and notify the service which probably will stop operation.
			cb.connectionErrorCB(error);
			return;
		}

		retries++;

		usleep(options.waitTime_ms * 1000l);

		connection.ireconnect();
	}

	void connectionSuccessfullCB(ServiceClient & connection){
		retries = 0;
	}
private:
	int retries = 0;
	DefaultConnectionCallbackOptions options;
	DefaultConnectionCallbackErrorCB & cb;
};

class DefaultReConnectionCallback: public ConnectionCallback{
public:
	void connectionErrorCB(ServiceClient & connection, CommunicationError error){
		// wait 100 ms
		usleep(100 * 1000l);

		connection.ireconnect();
	}

	void connectionSuccessfullCB(ServiceClient & connection){
	}
};

}