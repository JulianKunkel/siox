#ifndef CALLBACK_H
#define CALLBACK_H

#include "ConnectionMessage.hpp"

class Callback {

	public:
		Callback( int msg_type = 0 ) : msg_type_( msg_type ) {};

		void execute( ConnectionMessage & msg ) const {
			if( msg_type_ == 0 || msg_type_ == msg.get_msg()->type() )
				return handle_message( msg );
		}

		void execute( boost::shared_ptr<ConnectionMessage> msg ) const {
			if( msg_type_ == 0 || msg_type_ == msg->get_msg()->type() )
				return handle_message( msg );
		}

	protected:
		virtual void handle_message( ConnectionMessage & msg ) const {};
		virtual void handle_message( boost::shared_ptr<ConnectionMessage> msg ) const {};

	private:
		boost::uint32_t msg_type_;
};

#endif
