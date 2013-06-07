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

class TransactionService {
	   
public:
	TransactionService(const std::string &address);
	~TransactionService();
	
	int run();
	int stop();
	
	void register_transaction_backend(TransactionBackend *tb);
	void clear_transaction_backends();
	
private:
	ServiceServer *network_service_;
	boost::ptr_list<TransactionBackend> backends_;
	
	void register_callback(Callback &cb);
	void clear_callbacks();
};

#endif
