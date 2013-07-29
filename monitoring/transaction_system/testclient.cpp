#include "ServiceClient.hpp"
#include "SioxLogger.hpp"

#define SIOX_TS_ADDR  "tcp://localhost:7000"

Logger *logger;

int main(int argc, char *argv[])
{
	ServiceClient *client = new ServiceClient(SIOX_TS_ADDR);
	client->run();

	boost::shared_ptr<siox::MessageBuffer> mp(new siox::MessageBuffer());
	mp->set_action(siox::MessageBuffer::Activity);
	mp->set_type(8);
	mp->set_unid(40);
	mp->set_aid(50);

	boost::shared_ptr<ConnectionMessage> msg_ptr(new ConnectionMessage(mp));

	client->isend(msg_ptr);
	
	sleep(1);
	
	client->stop();
	
	return 0;
}