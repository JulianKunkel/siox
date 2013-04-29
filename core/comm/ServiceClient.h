/**
 * 
 * 
 */

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
	/**
	 * Creates an instance of ServiceClient and connects it to the given 
	 * address. E.g. <i>ipc:///tmp/sioxsock</i>, or <i>tcp://host:port</i>.
	 */
	ServiceClient(const std::string &endpoint_uri);
	
// 	void register_error_callback(Callback err_cb);
// 	void unregister_error_callback(Callback err_cb);
// 	void clear_error_callbacks();
// 
// 	void register_response_callback(Callback rsp_cb);
// 	void unregister_response_callback(Callback rsp_cb);
// 	void clear_response_callbacks();
	
	void isend(const ConnectionMessage &msg);

	void run();
	void stop();
	
private:
	asio::io_service io_service_;
	Connection *connection_;

	void handle_stop();
// 	std::list<Callback> error_callbacks;
// 	std::list<Callback> response_callbacks;
};


#endif
