#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>

#include "build/siox.pb.h"
#include "ConnectionMessage.h"
#include "misc/URI.h"

namespace asio = boost::asio;

// class IPCClient {
// public:
// 	IPCClient(asio::io_service &io_service, const std::string &path)
// 		: socket_(io_service),
// 		  io_service_(io_service)
// 	{
// 		asio::local::stream_protocol::endpoint endpoint(path);
// 		socket_.connect(endpoint);
// 	}
// 	
// 	void write(const ConnectionMessage<siox_buffers::MessageBuffer> &msg)
// 	{
// 		std::cout << "Inside write() " << std::endl;
// 		
// 		io_service_.post(boost::bind(&IPCClient::do_write, this, msg));
// 	}
// 	
// 	void close()
// 	{
// 		io_service_.post(boost::bind(&IPCClient::do_close, this));
// 	}
// 	
// private:
// 	asio::io_service& io_service_;
// 	asio::local::stream_protocol::socket socket_;
// 
// 	void handle_write(const boost::system::error_code &error){
// 		
// 		if (!error) {
// 			std::cout << "Data successfuly written" << std::endl;
// 		} else {
// 			std::cerr << "Error writting data. " << error.message() 
// 				<< std::endl;
// 			do_close();
// 		}
// 	}
// 	
// 	
// 	void do_write(const ConnectionMessage<siox_buffers::MessageBuffer> &msg)
// 	{
// 		std::vector<boost::uint8_t> msgbuf;
// 		msg.pack(msgbuf);
// 		
// // 		std::cout << "Buffer: " << writebuf << std::endl;
// 		
// // 		asio::buffer ab(writebuf);
// 		std::cout << "Inside do_write()" << std::endl;
// 		std::cout << "MSGBUF: " << show_hex<std::vector<boost::uint8_t> >(msgbuf) << std::endl;
// 		
// 		asio::write(socket_, asio::buffer(msgbuf));
// // 			boost::bind(&TestClient::handle_write, this, 
// // 				asio::placeholders::error));
// 	}
// 	
// 	
// 	void do_close()
// 	{
// 		std::cout << "Closing TCP socket." << std::endl;
// 		
// 		socket_.close();
// 	}
// 
// };


int main(int argc, char *argv[])
{
	try {
		std::cout << "Starting IPC client" << std::endl;
		
		asio::io_service io_service;
		
		asio::local::stream_protocol::socket socket(io_service);
		asio::local::stream_protocol::endpoint endpoint("/tmp/ipcsock");
		socket.connect(endpoint);
		
// 		TestClient c(io_service, "localhost", "6677");
		
// 		boost::thread t(boost::bind(&asio::io_service::run, 
// 					    &io_service));
		
		boost::shared_ptr<siox::MessageBuffer> mp(new siox::MessageBuffer());
		
		mp->set_type(siox::MessageBuffer::TYPE1);
		mp->set_unid(10);
		mp->set_aid(20);

		ConnectionMessage sm(mp);

// 		std::vector<boost::uint8_t> msgbuf;
// 		sm.pack(msgbuf);
// 
// 		std::cout << "Going to send buffer "
// 			<< show_hex<std::vector<boost::uint8_t> >(msgbuf).c_str() 
// 			<< std::endl;
// 
// 		
// 		asio::write(socket, asio::buffer(msgbuf));
		
//  		c.do_write(sm);
		
// 		c.close();
// 		t.join();

	} catch (std::exception &e) {
		std::cerr << "Exception: " << e.what() << std::endl;
	}
	
	return 0;
}
