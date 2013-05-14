#include "ServiceServer.h"


ServiceServer::ServiceServer(const std::size_t worker_pool_size)
  : worker_pool_size_(worker_pool_size)
{
}


void ServiceServer::run()
{
	for (std::size_t i = 0; i < worker_pool_size_; ++i) {
		boost::shared_ptr<boost::thread> worker(
			new boost::thread(boost::bind(
				&asio::io_service::run, &io_service_)));
		workers_.push_back(worker);
	}
}


void ServiceServer::stop()
{
#ifndef NDEBUG
	syslog(LOG_NOTICE, "Waiting for server workers to finish."); 
#endif

	handle_stop();
}


void ServiceServer::handle_stop()
{
	io_service_.stop();
	
	for (std::size_t i = 0; i < workers_.size(); ++i) 
		workers_[i]->join();
}


void ServiceServer::advertise(boost::uint64_t mtype)
{
	boost::shared_ptr<siox::MessageBuffer> mbuf(new siox::MessageBuffer());
	mbuf->set_action(siox::MessageBuffer::Advertise);
	mbuf->set_type(mtype);
	
	boost::shared_ptr<ConnectionMessage> msg(new ConnectionMessage(mbuf));
	ipublish(msg);
}


void ServiceServer::isend_response(ConnectionMessage &message, 
				ConnectionMessage &response)
{
	
}


void ServiceServer::isend_response(boost::shared_ptr<ConnectionMessage> msg, 
				   boost::shared_ptr<ConnectionMessage> rsp)
{
	
}


void ServiceServer::register_message_callback(Callback &msg_rcvd_callback)
{
	message_callbacks_.push_back(&msg_rcvd_callback);
}


void ServiceServer::clear_message_callbacks()
{
	message_callbacks_.clear();
}


void ServiceServer::register_error_callback(Callback &err_callback)
{
	error_callbacks_.push_back(&err_callback);
}


void ServiceServer::clear_error_callbacks()
{
	error_callbacks_.clear();
}


void ServiceServer::handle_message(ConnectionMessage &msg)
{
#ifndef NDEBUG
	syslog(LOG_NOTICE, "Handling message...");
#endif
	boost::ptr_list<Callback>::iterator i;
	for (i = message_callbacks_.begin(); i != message_callbacks_.end(); ++i) {
#ifndef NDEBUG
		syslog(LOG_NOTICE, "Executing message response callback.");
#endif
		i->execute(msg);
	}
}


void ServiceServer::handle_message(boost::shared_ptr<ConnectionMessage> msg)
{
#ifndef NDEBUG
	syslog(LOG_NOTICE, "Handling message...");
#endif
	boost::ptr_list<Callback>::iterator i;
	for (i = message_callbacks_.begin(); i != message_callbacks_.end(); ++i) {
#ifndef NDEBUG
		syslog(LOG_NOTICE, "Executing message response callback.");
#endif
		i->execute(msg);
	}
}
