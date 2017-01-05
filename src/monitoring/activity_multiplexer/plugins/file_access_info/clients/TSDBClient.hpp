
#ifndef  TSDBClient_INC
#define  TSDBClient_INC

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/algorithm/string.hpp>

#include "../datatypes/TSDBDatapoint.hpp"
#include "BaseClient.hpp"

#include <iostream>

using namespace boost::asio;
using namespace boost::asio::ip;
using boost::asio::ip::tcp;


class TSDBClient : public Client{
	public:
	private:
		std::string make_send_uri() override;
};

#endif   /* ----- #ifndef TSDBClient_INC  ----- */
