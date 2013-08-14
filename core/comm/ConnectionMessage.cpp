#include <core/comm/ConnectionMessage.hpp>

ConnectionMessage::ConnectionMessage(MessagePointer msg)
  : msg_(msg)
{
}


ConnectionMessage::ConnectionMessage(const ConnectionMessage &cm)
  : msg_(cm.get_msg())
{
}


void ConnectionMessage::set_msg(MessagePointer msg)
{
	msg_ = msg;
}


typename ConnectionMessage::MessagePointer ConnectionMessage::get_msg() const
{
	return msg_;
}


bool ConnectionMessage::pack(data_buffer &buf) const
{
	if (!msg_) 
		return false;
	
	unsigned msg_size = msg_->ByteSize();
	buf.resize(HEADER_SIZE + msg_size);
	encode_header(buf, msg_size);
	
	return msg_->SerializeToArray(&buf[HEADER_SIZE], msg_size);
}


unsigned ConnectionMessage::decode_header(const data_buffer &buf) const
{
	if (buf.size() < HEADER_SIZE)
		return 0;
	
	unsigned msg_size = 0;
	for (unsigned i = 0; i < HEADER_SIZE; ++i)
		msg_size = msg_size*256 
			+ (static_cast<unsigned>(buf[i]) & 0xFF);
	
	return msg_size;
}


bool ConnectionMessage::unpack(const data_buffer &buf) 
{
	return msg_->ParseFromArray(&buf[HEADER_SIZE], buf.size() 
		- HEADER_SIZE);
}


void ConnectionMessage::encode_header(data_buffer &buf, unsigned size) const
{
	buf[0] = static_cast<boost::uint8_t>((size >> 24) & 0xFF);
	buf[1] = static_cast<boost::uint8_t>((size >> 16) & 0xFF);
	buf[2] = static_cast<boost::uint8_t>((size >> 8) & 0xFF);
	buf[3] = static_cast<boost::uint8_t>(size & 0xFF);
}


