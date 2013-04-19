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
	std::vector<boost::shared_ptr<boost::thread> > workers;
	
	for (std::size_t i = 0; i < worker_pool_size_; ++i) {
		boost::shared_ptr<boost::thread> worker(
			new boost::thread(boost::bind(
				&asio::io_service::run, &io_service_)));
		workers.push_back(worker);
	}
	
	for (std::size_t i = 0; i < workers.size(); ++i) 
		workers[i]->join();

}


void ServiceServer::handle_stop()
{
	io_service_.stop();
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


void ServiceServer::register_response_callback(siox::MessageBuffer::MessageType type, 
					    Callback &message_received_callback)
{
	
}

