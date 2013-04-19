#include "ServiceClient.h"

class IPCServiceClientTest : public ServiceClient {
public:
	IPCServiceClientTest(const std::string &endpoint_uri)
	   : ServiceClient(endpoint_uri)
	{
	}
};


int main(int argc, char *argv[]) 
{
	IPCServiceClientTest sc("ipc:///tmp/sioxsocket");
	
	return 0;
}