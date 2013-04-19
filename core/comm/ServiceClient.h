#ifndef SERVICE_CLIENT_H
#define SERVICE_CLIENT_H

#include <list>
#include <string>

#include "Callback.h"
#include "Connection.h"
#include "ConnectionFactory.h"

namespace asio = boost::asio;

class ServiceClient {
public:
	ServiceClient(const std::string &endpoint_uri);
	
	void register_error_callback(Callback err_cb);
	void unregister_error_callback(Callback err_cb);
	void clear_error_callbacks();

	void register_response_callback(Callback rsp_cb);
	void unregister_response_callback(Callback rsp_cb);
	void clear_response_callbacks();
	
private:
	std::list<Callback> error_callbacks;
	std::list<Callback> response_callbacks;
	asio::io_service io_service_;
	Connection *connection_;
};


#endif
