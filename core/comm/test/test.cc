#include <boost/bind.hpp>

class server : private boost::noncopyable {
public:
	explicit server(const std::string &address, const std::string &port);
private:
	void handle_accept(const boost::system::error_code &e);
	boost::asio::io_service io_service_;
	boost::asio::ip::tcp::acceptor acceptor_;
	connection_ptr new_connection_;
	request_handler request_handler;
}


server::server(const std::string &address, const std::string &port) :
	acceptor_(io_service_), new_connection_()
{
	boost::asio::ip::tcp::resolver resolver(io_service_);
	boost::asio::ip::tcp::resolver::query query(address, port);
	boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);

	acceptor_.open(endpoint.protocol());
	acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
	acceptor_.listen();

	new_connection_.reset(new connection(io_service_, request_handler_));
	acceptor_.async_accept(new_connection_->socket(), 
			boost::bind(&server::handle_accept, this
			boost::asio::placeholders::error));

}

void server::handle_accept(const boost::system::error_code &e) 
{
	if (!e)
		new_connection_->start();
	
	start_accept();
}

void server::start_accept()
{
	new_connection_.reset(new connection(io_service_, request_handler_));
	acceptor_.async_accept(new_connection_->socket(),
			       boost::bind(&server::handle_accept, this, boost::asio::placeholders::error));
}

class connection : public boost::enable_shared_from_this<connection>, 
		private boost::noncopyable {
public:
	explicit connection(boost::asio::io_service& io_service, request_handler& handler);
	boost::asio::ip::tcp::socket& socket();
	void start();
private:
	boost::asio::io_service::strand strand_;
	boost::asio::ip::tcp::socket socket_;
	request_handler& request_handler_;
	boost::array<char, 8192> buffer_;
}

connection::connection(boost::asio::io_service &io_service, 
		       request_handler &handler) : strand_(io_service),
		       socket_(io_service),
		       request_handler_(handler)
{
}

boost::asio::ip::tcp::socket& connection::socket()
{
  return socket_;
}       

connection::start()
{
	socket_.async_read_some(boost::asio::buffer(buffer_), 
		strand_.wrap(boost::bind(&connection::handle_read, shared_from_this(),
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred)));
}
