#ifndef TRANSACTION_SERVICE_H
#define TRANSACTION_SERVICE_H

#include <boost/ptr_container/ptr_list.hpp>
#include <boost/shared_ptr.hpp>
#include <string>

#include "Connection.h"
#include "ConnectionMessage.h"
#include "ServerFactory.h"
#include "ServiceServer.h"
#include "SioxLogger.h"
#include "TransactionBackend.h"

class TransactionService 
   : public Service {
	   
public:
	TransactionService(const std::string &address);
	~TransactionService();
	
	int run();
	int stop();
	
	void add_transaction_backend(TransactionBackend *tb);
	void clear_transaction_backends();
	
	void handle_message(ConnectionMessage &msg, Connection &connection);
	void handle_message(boost::shared_ptr<ConnectionMessage> msg, 
			    Connection &connection);
	
private:
	ServiceServer *network_service_;
	boost::ptr_list<TransactionBackend> backends_;
	boost::ptr_list<Callback> message_callbacks_;
	
	void add_callback(Callback &cb);
	void clear_callbacks();
};

#endif
