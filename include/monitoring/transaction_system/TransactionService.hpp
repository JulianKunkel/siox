#ifndef TRANSACTION_SERVICE_H
#define TRANSACTION_SERVICE_H

#include <boost/ptr_container/ptr_list.hpp>
#include <boost/shared_ptr.hpp>
#include <string>

#include <core/comm/modules/asio/Connection.hpp>
#include <core/comm/modules/asio/ConnectionMessage.hpp>
#include <core/comm/modules/asio/ServerFactory.hpp>
#include <core/comm/modules/asio/ServiceServer.hpp>
#include <core/logger/SioxLogger.hpp>
#include <monitoring/transaction_system/TransactionBackend.hpp>

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
