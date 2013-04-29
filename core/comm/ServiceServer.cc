#include "ServiceServer.h"


ServiceServer::ServiceServer(const std::size_t worker_pool_size)
	: signals_(io_service_),
	  worker_pool_size_(worker_pool_size)
{
	signals_.add(SIGINT);
	signals_.add(SIGTERM);
#ifdef SIGQUIT
	signals_.add(SIGQUIT);
#endif
	signals_.async_wait(boost::bind(&ServiceServer::handle_stop, this));
}


void ServiceServer::run()
{
	syslog(LOG_NOTICE, "Server::Running io_service.");

	for (std::size_t i = 0; i < worker_pool_size_; ++i) {
		boost::shared_ptr<boost::thread> worker(
			new boost::thread(boost::bind(
				&asio::io_service::run, &io_service_)));
		workers.push_back(worker);
	}
	
}


void ServiceServer::stop()
{
	handle_stop();
}


void ServiceServer::handle_stop()
{
	io_service_.stop();
	
	for (std::size_t i = 0; i < workers.size(); ++i) 
		workers[i]->join();

}


void ServiceServer::advertise(siox::MessageBuffer::MessageType message_type)
{
}


void ServiceServer::ipublish(ConnectionMessage &message)
{
}


void ServiceServer::isend_response(ConnectionMessage &message, 
				ConnectionMessage &response)
{
	
}


void ServiceServer::register_response_callback(Callback
					       &message_received_callback)
{
	response_callbacks.push_back(&message_received_callback);
}


void ServiceServer::handle_message(ConnectionMessage &msg)
{
	syslog(LOG_NOTICE, "TCP::Handling message...");
	
	boost::ptr_list<Callback>::iterator i;
	for (i = response_callbacks.begin(); i != response_callbacks.end(); ++i) {
		syslog(LOG_NOTICE, "TCP::Executing callback...");
		i->handle_message(msg);
	}
}

