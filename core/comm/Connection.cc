#include "Connection.h"

Connection::Connection(asio::io_service &io_service)
   : io_service_(&io_service),
     strand_(io_service),
     msg_(boost::shared_ptr<siox::MessageBuffer>(new siox::MessageBuffer()))
{
}


void Connection::disconnect()
{
}


asio::io_service *Connection::get_io_service()
{
	return io_service_;
}


void Connection::ireconnect()
{
}


// void Connection::isend(const ConnectionMessage &msg, Callback &rsp_cb)
// {
// 	
// }
// 
// 
// void Connection::register_connection_callback(Callback &conn_cb)
// {
// 	
// }
// 
// 
// void Connection::register_message_callback(int msg_type, Callback &msg_cb)
// {
// }
// 
// 
// void Connection::subscribe(int msg_type, Callback &cb)
// {
// 	
// }
// 
