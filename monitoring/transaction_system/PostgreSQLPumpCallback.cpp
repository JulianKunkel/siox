#include "PostgreSQLPumpCallback.hpp"

PostgreSQLPumpCallback::PostgreSQLPumpCallback(PGconn &dbconn)
   : dbconn_(&dbconn)
{
}


void PostgreSQLPumpCallback::handle_message(ConnectionMessage &msg) const
{
	if (msg.get_msg()->action() == siox::MessageBuffer::Activity) {
		
// 		store_activity(msg);
		
	} else if (msg.get_msg()->action() == siox::MessageBuffer::Component) {
		
// 		store_component(msg);
		
	}
}


void PostgreSQLPumpCallback::handle_message(boost::shared_ptr<ConnectionMessage> msg) const
{
}


