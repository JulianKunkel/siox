#include "ServiceClient.h"

ServiceClient::ServiceClient(const std::string &endpoint_uri)
   : io_service_()
{
	connection_ = ConnectionFactory::create_connection(io_service_, 
							   endpoint_uri);

}


void ServiceClient::register_error_callback(Callback err_cb)
{
	error_callbacks.push_back(err_cb);
}


void ServiceClient::unregister_error_callback(Callback err_cb)
{
}


void ServiceClient::clear_error_callbacks()
{
	error_callbacks.clear();
}


void ServiceClient::register_response_callback(Callback rsp_cb)
{
	response_callbacks.push_back(rsp_cb);
}


void ServiceClient::unregister_response_callback(Callback rsp_cb)
{
}


void ServiceClient::clear_response_callbacks()
{
	response_callbacks.clear();
}

