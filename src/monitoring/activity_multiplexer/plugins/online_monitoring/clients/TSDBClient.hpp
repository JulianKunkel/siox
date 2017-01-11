
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

#include "BaseClient.hpp"

#include <iostream>
#include <memory>


class TSDBClient : public Client{
	public:
		std::string to_json(std::shared_ptr<Client::Datapoint> point) const override;
	private:
		std::string make_send_uri() override;
		std::string to_json_metric(const std::string& metric, LongType value, std::shared_ptr<Client::Datapoint> point) const;
};

#endif   /* ----- #ifndef TSDBClient_INC  ----- */
