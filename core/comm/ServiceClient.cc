#include "ServiceClient.h"

ServiceClient::ServiceClient(const std::string &endpoint_uri, 
			     std::size_t worker_pool_size)
   : io_service_(),
     worker_pool_size_(worker_pool_size)
{
#ifndef NDEBUG
	syslog(LOG_NOTICE, "Creating service client at %s.", 
	       endpoint_uri.c_str());
#endif
	try {
		connection_ = 
			ConnectionFactory::create_connection(io_service_,
							     endpoint_uri);
	} catch(ConnectionFactoryException &e) {
		throw(new ServiceClientException(e.what()));
	}

}

void ServiceClient::run()
{
	for (std::size_t i = 0; i < worker_pool_size_; ++i) {
		boost::shared_ptr<boost::thread> worker(
			new boost::thread(boost::bind(
				&asio::io_service::run, &io_service_)));
		workers.push_back(worker);
	}
}


void ServiceClient::stop()
{
#ifndef NDEBUG
	syslog(LOG_NOTICE, "Waiting for client workers to finish."); 
#endif
	io_service_.stop();
	
	for (std::size_t i = 0; i < workers.size(); ++i) 
		workers[i]->join();

}


void ServiceClient::register_error_callback(Callback &err_cb)
{
	error_callbacks.push_back(&err_cb);
}


void ServiceClient::clear_error_callbacks()
{
	error_callbacks.clear();
}


void ServiceClient::register_response_callback(Callback &rsp_cb)
{
	response_callbacks.push_back(&rsp_cb);
}


void ServiceClient::clear_response_callbacks()
{
	response_callbacks.clear();
}


void ServiceClient::isend(const ConnectionMessage &msg)
{
	connection_->isend(msg);
}


void ServiceClient::handle_response(ConnectionMessage &msg)
{
#ifndef NDEBUG
	syslog(LOG_NOTICE, "Handling message response.");
#endif
	boost::ptr_list<Callback>::iterator i;
	for (i = response_callbacks.begin(); i != response_callbacks.end(); ++i) {
#ifndef NDEBUG
		syslog(LOG_NOTICE, "Executing message response callback.");
#endif
		i->handle_message(msg);
	}
}


void ServiceClient::handle_message(ConnectionMessage &msg)
{
}
