#include "TransactionService.h"

TransactionService::TransactionService(const std::string &address)
{
	try {
		
		network_service_ = ServerFactory::create_server(address);
		
	} catch(ServerFactoryException &e) {
		
		logger->log(Logger::ERR, 
			    "Error creating network service: %s.", e.what());
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


void TransactionService::add_transaction_backend(TransactionBackend *tb)
{
	backends_.push_back(tb);
	add_callback(*(tb->create_callback()));
}


void TransactionService::clear_transaction_backends()
{
	backends_.clear();
}


void TransactionService::handle_message(ConnectionMessage &msg, 
					Connection &connection)
{
	boost::ptr_list<Callback>::iterator i;
	for (i = message_callbacks_.begin(); i != message_callbacks_.end(); ++i) {
		i->execute(msg);
	}
}


void TransactionService::handle_message(boost::shared_ptr<ConnectionMessage> msg, 
					Connection &connection)
{
	boost::ptr_list<Callback>::iterator i;
	for (i = message_callbacks_.begin(); i != message_callbacks_.end(); ++i) {
		i->execute(msg);
	}
}


void TransactionService::add_callback(Callback &cb)
{
	message_callbacks_.push_back(&cb);
}


void TransactionService::clear_callbacks()
{
	message_callbacks_.clear();
}


