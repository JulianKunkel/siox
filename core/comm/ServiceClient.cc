#include "ServiceClient.h"

ServiceClient::ServiceClient(const std::string &endpoint_uri)
   : io_service_()
{
	syslog(LOG_NOTICE, "Client::Creating service client at %s.", endpoint_uri.c_str());
	connection_ = ConnectionFactory::create_connection(io_service_, 
							   endpoint_uri);

}

void ServiceClient::run()
{
	syslog(LOG_NOTICE, "Client::Running io_service.");
	io_service_.run();
	syslog(LOG_NOTICE, "Client::io_service has been stopped or has nothing to do.");
	return;
	
// 	std::vector<boost::shared_ptr<boost::thread> > workers;
	
// 	for (std::size_t i = 0; i < worker_pool_size_; ++i) {
// 		boost::shared_ptr<boost::thread> worker(
// 			new boost::thread(boost::bind(
// 				&asio::io_service::run, &io_service_)));
// 		workers.push_back(worker);
// 	}
// 	
// 	for (std::size_t i = 0; i < workers.size(); ++i) 
// 		workers[i]->join();

}


void ServiceClient::stop()
{
	handle_stop();
}


void ServiceClient::handle_stop()
{
	io_service_.stop();
}


// void ServiceClient::register_error_callback(Callback err_cb)
// {
// 	error_callbacks.push_back(err_cb);
// }
// 
// 
// void ServiceClient::unregister_error_callback(Callback err_cb)
// {
// }
// 
// 
// void ServiceClient::clear_error_callbacks()
// {
// 	error_callbacks.clear();
// }
// 
// 
// void ServiceClient::register_response_callback(Callback rsp_cb)
// {
// 	response_callbacks.push_back(rsp_cb);
// }
// 
// 
// void ServiceClient::unregister_response_callback(Callback rsp_cb)
// {
// }
// 
// 
// void ServiceClient::clear_response_callbacks()
// {
// 	response_callbacks.clear();
// }


void ServiceClient::isend(const ConnectionMessage &msg)
{
	connection_->isend(msg);
}

