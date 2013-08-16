#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Main
#include <boost/test/unit_test.hpp>
#include <boost/thread.hpp>

#include <iostream>

#include <core/comm/Callback.hpp>
#include <core/comm/ServerFactory.hpp>
#include <core/comm/ServiceClient.hpp>
#include <core/logger/SioxLogger.hpp>

namespace asio = boost::asio;

ConnectionMessage *m;
boost::uint64_t unid_sum;

Logger *logger;

// The purpose of this callback is just to extract the message received by the
// service in order to compare it to the one that was sent.
class TestCallback : public Callback {
public:
	void handle_message(ConnectionMessage &msg) const
	{
		logger->log(Logger::DEBUG, "TestCallback handle_message");
		set_message(msg);
	}
	
	void set_message(ConnectionMessage &msg) const
	{
		BOOST_TEST_MESSAGE("Setting ConnectionMessage");
		m = &msg;
	}
	
};

class AddCallback : public Callback {
public:
	AddCallback() {
		unid_sum = 0;
	};
	
	void handle_message(ConnectionMessage &msg) const
	{
		logger->log(Logger::DEBUG, "AddCallback handle_message");
		unid_sum += msg.get_msg()->unid();
	}
};


BOOST_AUTO_TEST_CASE(ipc_communication)
{
	if (!logger)
		logger = new Logger();
	
	std::string ipc_socket_path("ipc:///tmp/siox.socket");
	ServiceServer *server = ServerFactory::create_server(ipc_socket_path);
	server->run();

	TestCallback test_cb;
	server->register_message_callback(test_cb);
	
	boost::shared_ptr<buffers::MessageBuffer> mp(new buffers::MessageBuffer());
	mp->set_action(buffers::MessageBuffer::Activity);
	mp->set_type(2);
	mp->set_unid(10);

	boost::shared_ptr<ConnectionMessage> msg_ptr(new ConnectionMessage(mp));

	ServiceClient *client = new ServiceClient(ipc_socket_path);
	client->run();
	
	client->isend(msg_ptr);

	sleep(1);
	
	BOOST_CHECK_EQUAL(mp->unid(), m->get_msg()->unid());
	
	mp->set_action(siox::MessageBuffer::Subscribe);
	mp->set_type(300);
	client->isend(msg_ptr);
	
	sleep(1);
	
	client->register_response_callback(test_cb);
	
	mp->set_unid(33);
	server->ipublish(msg_ptr);
	
	sleep(1);

	BOOST_CHECK_EQUAL(33, m->get_msg()->unid());

	server->advertise(666);
	
	sleep(1);
 
	BOOST_CHECK_EQUAL(666, m->get_msg()->type());

	client->stop();
	server->stop();
}


BOOST_AUTO_TEST_CASE(tcp_communication)
{
	if (!logger)
		logger = new Logger();

	std::string tcp_socket_addr("tcp://localhost:6677");
	ServiceServer *server = ServerFactory::create_server(tcp_socket_addr);
	server->run();

	TestCallback test_cb;
	
	server->register_message_callback(test_cb);
	
	boost::shared_ptr<siox::MessageBuffer> mp(new siox::MessageBuffer());
	mp->set_action(siox::MessageBuffer::Advertise);
	mp->set_type(8);
	mp->set_unid(40);
	mp->set_aid(50);

	boost::shared_ptr<ConnectionMessage> msg_ptr(new ConnectionMessage(mp));
	
	ServiceClient *client = new ServiceClient(tcp_socket_addr);
	client->run();
	
	client->isend(msg_ptr);
 
	sleep(1);
	
	BOOST_CHECK_EQUAL(mp->unid(), m->get_msg()->unid());
	
	mp->set_action(siox::MessageBuffer::Subscribe);
	mp->set_type(300);
	client->isend(msg_ptr);

	sleep(1);
	
	client->register_response_callback(test_cb);
	
	mp->set_unid(99);
	server->ipublish(msg_ptr);
	
	sleep(1);

	BOOST_CHECK_EQUAL(99, m->get_msg()->unid());

	server->advertise(777);
	
	sleep(1);
 
	BOOST_CHECK_EQUAL(777, m->get_msg()->type());
	
	server->stop();
	client->stop();

}

boost::uint64_t multisend(int n, ServiceClient &c)
{
	for (int i = 0; i < n; ++i) {
		
		boost::shared_ptr<siox::MessageBuffer> mp(new siox::MessageBuffer());
		mp->set_action(siox::MessageBuffer::Activity);
		mp->set_type(10);
		mp->set_unid(i);
		
		boost::shared_ptr<ConnectionMessage> msg_ptr(new ConnectionMessage(mp));
		
		c.isend(msg_ptr);
	}
	
	return n*(n-1)/2;
}


BOOST_AUTO_TEST_CASE(tcp_multisend_single)
{
	if (!logger)
		logger = new Logger();

	std::string tcp_socket_addr("tcp://localhost:6678");
	ServiceServer *server = ServerFactory::create_server(tcp_socket_addr);
	server->run();

	AddCallback acb;
	server->register_message_callback(acb);

	ServiceClient *client = new ServiceClient(tcp_socket_addr);
	client->run();
	
	boost::uint64_t sum = multisend(1000, *client);
	
	sleep(1);
	
	BOOST_CHECK_EQUAL(sum, unid_sum);
	
	server->stop();
	client->stop();
}


BOOST_AUTO_TEST_CASE(ipc_multisend_single)
{
	if (!logger)
		logger = new Logger();

	std::string socket_addr("ipc:///tmp/socket");
	ServiceServer *server = ServerFactory::create_server(socket_addr);
	server->run();

	AddCallback acb;
	server->register_message_callback(acb);

	ServiceClient *client = new ServiceClient(socket_addr);
	client->run();
	
	boost::uint64_t sum = multisend(1000, *client);
	
	sleep(1);
	
	BOOST_CHECK_EQUAL(sum, unid_sum);
	
	server->stop();
	client->stop();
}
