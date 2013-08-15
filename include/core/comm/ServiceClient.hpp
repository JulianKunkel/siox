/**
 * ServiceClient provides an interface to communicate with a ServiceServer.
 * 
 */

#ifndef SERVICE_CLIENT_H
#define SERVICE_CLIENT_H

#include <exception>
#include <string>
#include <vector>

#include <boost/ptr_container/ptr_list.hpp>

#include <core/comm/Callback.hpp>
#include <core/comm/Connection.hpp>
#include <core/comm/ConnectionFactory.hpp>
#include <core/logger/SioxLogger.hpp>

namespace asio = boost::asio;


class ServiceClientException 
   : public std::exception {
	   
public:
	ServiceClientException(const char *err_msg) : err_msg_(err_msg) {}
	const char *what() const throw() { return err_msg_; }
private:
	const char *err_msg_;
	
};


class ServiceClient
   : public Service,
     private boost::noncopyable {
	     
public:
	/**
	 * Creates an instance of ServiceClient and connects it to the given 
	 * address. 
	 * 
	 * @param endpoint_uri Address where the ServiceServer is listening for 
	 * connections.E.g. <i>ipc:///tmp/sioxsock</i> and <i>tcp://host:port</i>.
	 * @param worker_pool_size The number of threads handling the messages. 
	 * (Default is 1).
	 */
	ServiceClient(const std::string &endpoint_uri, 
		      std::size_t worker_pool_size = 1);
	
	/**
	 * Adds a Callback object to the list of callbacks responsible for error
	 * handling.
	 * 
	 */
	void register_error_callback(Callback &err_cb);
	
	/**
	 * Adds a Callback object to the list of callbacks responsible for 
	 * handling the message responses from the connected ServiceServer.
	 */
	void register_response_callback(Callback &rsp_cb);
	
	/**
	 * Remove all callbacks from the list of error handling callbacks.
	 */
	void clear_error_callbacks();
	
	/**
	 * Remove all callbacks from the list of response handling callbacks.
	 */
	void clear_response_callbacks();
	
	/**
	 * Asynchronously sends a message to the ServiceServer. 
	 * 
	 * @note This function returns immediately and the message is actually 
	 * sent by the threads spawn by the run() method.
	 */
	void isend(boost::shared_ptr<ConnectionMessage> msg);

	/**
	 * Spawns the worker threads and starts the event processing loop.
	 *
	 * @note If the ServiceClient is not running, messages won't be 
	 * processed.
	 */
	void run();
	
	/**
	 * Stops the client and waits for completion of all worker threads.
	 */
	void stop();

	void handle_message(ConnectionMessage &msg, 
			    Connection &connection);
	
	void handle_message(boost::shared_ptr<ConnectionMessage> msg, 
			    Connection &connection);
	
private:
	Connection *connection_;
	asio::io_service io_service_;
	std::size_t worker_pool_size_;
	boost::ptr_list<Callback> error_callbacks;
	boost::ptr_list<Callback> response_callbacks;
	std::vector<boost::shared_ptr<boost::thread> > workers;
};


#endif
