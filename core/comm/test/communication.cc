#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Main
#include <boost/test/unit_test.hpp>
#include <boost/thread.hpp>

#include <iostream>

#include "siox.pb.h"
#include "Callback.h"
#include "ServerFactory.h"
#include "ServiceClient.h"

namespace asio = boost::asio;

ConnectionMessage *m;

// The purpose of this callback is just to extract the message received by the
// service in order to compare it to the one that was sent.
class TestCallback : public Callback {
public:
	void handle_message(ConnectionMessage &msg) const
	{
		syslog(LOG_NOTICE, "TestCallback handle_message");
		set_message(msg);
	}
	
	void set_message(ConnectionMessage &msg) const
	{
		BOOST_TEST_MESSAGE("Setting ConnectionMessage");
		m = &msg;
	}
	
};


void init_syslog()
{
	setlogmask(LOG_UPTO(LOG_NOTICE));
	openlog("CommTest", LOG_CONS | LOG_NDELAY | LOG_NOWAIT | LOG_PERROR | 
		LOG_PID, LOG_USER);
};


BOOST_AUTO_TEST_CASE(ipc_communication)
{
	init_syslog();
	
	std::string ipc_socket_path("ipc:///tmp/siox.socket");
	ServiceServer *server = ServerFactory::create_server(ipc_socket_path);
	server->run();

	TestCallback test_cb;
	
	server->register_message_callback(test_cb);
	
	boost::shared_ptr<siox::MessageBuffer> mp(new siox::MessageBuffer());
	mp->set_action(siox::MessageBuffer::Advertise);
	mp->set_type(2);
	mp->set_unid(10);
	mp->set_aid(20);

	boost::shared_ptr<ConnectionMessage> msg_ptr(new ConnectionMessage(mp));

	ServiceClient *client = new ServiceClient(ipc_socket_path);
	client->run();
	
	client->isend(msg_ptr);

	sleep(1);
	
	BOOST_CHECK_EQUAL(mp->unid(), m->get_msg()->unid());
	
	client->register_response_callback(test_cb);
	
	mp->set_unid(210);
	server->ipublish(msg_ptr);

	sleep(1);

	BOOST_CHECK_EQUAL(210, m->get_msg()->unid());
	
	server->advertise(77);
	sleep(1);
	
	BOOST_CHECK_EQUAL(77, m->get_msg()->type());

	client->stop();
	server->stop();
}


BOOST_AUTO_TEST_CASE(tcp_communication)
{
	init_syslog();
	
	std::string tcp_socket_addr("tcp://localhost:6677");
	ServiceServer *server = ServerFactory::create_server(tcp_socket_addr);
	server->run();

	TestCallback test_cb;
	
	server->register_message_callback(test_cb);
	
	boost::shared_ptr<siox::MessageBuffer> mp(new siox::MessageBuffer());
	mp->set_action(siox::MessageBuffer::Advertise);
	mp->set_type(2);
	mp->set_unid(40);
	mp->set_aid(50);

	boost::shared_ptr<ConnectionMessage> msg_ptr(new ConnectionMessage(mp));
	
	ServiceClient *client = new ServiceClient(tcp_socket_addr);
	client->run();
	
	client->isend(msg_ptr);
 
	sleep(1);
	
	BOOST_CHECK_EQUAL(mp->unid(), m->get_msg()->unid());
	
	client->register_response_callback(test_cb);
	
	mp->set_unid(110);
	server->ipublish(msg_ptr);

	sleep(1);

	BOOST_CHECK_EQUAL(110, m->get_msg()->unid());
	
	server->advertise(66);
	sleep(1);
	
	BOOST_CHECK_EQUAL(66, m->get_msg()->type());
	
	server->stop();
	client->stop();

}

