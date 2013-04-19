#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>

#include "siox.pb.h"
#include "ConnectionMessage.h"
#include "misc/URI.h"

namespace asio = boost::asio;
using boost::asio::ip::tcp;

class TestClient {
public:
	TestClient(asio::io_service &io_service, const std::string &server, 
		    const std::string &port)
		: socket_(io_service),
		  resolver_(io_service),
		  io_service_(io_service)
	{
		tcp::resolver::query query(server, port);
		resolver_.async_resolve(query, 
			boost::bind(&TestClient::handle_resolve, this,
				asio::placeholders::error,
				asio::placeholders::iterator));
	}
	
	void write(const ConnectionMessage &msg)
	{
		std::cout << "Inside write() " << std::endl;
		
		io_service_.post(boost::bind(&TestClient::do_write, this, msg));
	}
	
	void close()
	{
		io_service_.post(boost::bind(&TestClient::do_close, this));
	}
	
// private:
	
	void handle_resolve(const boost::system::error_code &error,
			    tcp::resolver::iterator endpoint_iterator)
	{
		if (!error) {
		
			std::cout << "Server address resolved." << std::endl;
			
			asio::async_connect(socket_, endpoint_iterator,
				boost::bind(&TestClient::handle_connect, this,
					    asio::placeholders::error));
			
		} else {
			
			std::cerr << "Error resolving address. " 
				<< error.message() << std::endl;
		}
		
	}
	
	
	void handle_connect(const boost::system::error_code &error)
	{
		if (!error) {
			std::cout << "Connected to server." << std::endl;
		} else {
			std::cerr << "Error connecting to server. "
				<< error.message() << std::endl;
		}
	}
	
	
	void handle_write(const boost::system::error_code &error){
		
		if (!error) {
			std::cout << "Data successfuly written" << std::endl;
		} else {
			std::cerr << "Error writting data. " << error.message() 
				<< std::endl;
			do_close();
		}
	}
	
	
	void do_write(const ConnectionMessage &msg)
	{
		std::vector<boost::uint8_t> msgbuf;
		msg.pack(msgbuf);
		
// 		std::cout << "Buffer: " << writebuf << std::endl;
		
// 		asio::buffer ab(writebuf);
		std::cout << "Inside do_write()" << std::endl;
		std::cout << "MSGBUF: " << show_hex<std::vector<boost::uint8_t> >(msgbuf) << std::endl;
		
		asio::write(socket_, asio::buffer(msgbuf));
// 			boost::bind(&TestClient::handle_write, this, 
// 				asio::placeholders::error));
	}
	
	
	void do_close()
	{
		std::cout << "Closing TCP socket." << std::endl;
		
		socket_.close();
	}
	
	tcp::socket socket_;
	tcp::resolver resolver_;
	asio::io_service& io_service_;
};


int main(int argc, char *argv[])
{
	try {
		std::cout << "Starting TCP client" << std::endl;
		
		asio::io_service io_service;
		
		tcp::resolver resolver(io_service);
		tcp::resolver::query query("localhost", "6677");
		tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
		
		tcp::socket socket(io_service);
		asio::connect(socket, endpoint_iterator);
		
// 		TestClient c(io_service, "localhost", "6677");
		
// 		boost::thread t(boost::bind(&asio::io_service::run, 
// 					    &io_service));
		
		ConnectionMessage::MessagePointer 
			mp(new siox::MessageBuffer);
			
		mp->set_type(siox::MessageBuffer::TYPE1);
		mp->set_unid(10);
		mp->set_aid(20);
		
		ConnectionMessage sm(mp);
		
		std::vector<boost::uint8_t> msgbuf;
		sm.pack(msgbuf);

		std::cout << "Going to send buffer "
			<< show_hex<std::vector<boost::uint8_t> >(msgbuf).c_str() 
			<< std::endl;

		
		asio::write(socket, asio::buffer(msgbuf));
		
//  		c.do_write(sm);
		
// 		c.close();
// 		t.join();
		
	} catch (std::exception &e) {
		std::cerr << "Exception: " << e.what() << std::endl;
	}
	
	return 0;
}
