#include <monitoring/transaction_system/TransactionService.hpp>

TransactionService::TransactionService(const std::string &address)
{
	try {

		network_service_ = ServerFactory::create_server(address);

	} catch (ServerFactoryException &e) {

		logger->log(Logger::ERR, "Error creating network service: %s.", e.what());
	}
}


TransactionService::~TransactionService()
{
	stop();
}


int TransactionService::run()
{
	network_service_->run();
	return 0;
}


int TransactionService::stop()
{
	network_service_->stop();
	return 0;
}


void TransactionService::register_transaction_backend(TransactionBackend *tb)
{
	backends_.push_back(tb);
	register_callback(*(tb->create_callback()));
}


void TransactionService::clear_transaction_backends()
{
	backends_.clear();
}


void TransactionService::register_callback(Callback &cb)
{
	network_service_->register_message_callback(cb);
}


void TransactionService::clear_callbacks()
{
	network_service_->clear_message_callbacks();
}


