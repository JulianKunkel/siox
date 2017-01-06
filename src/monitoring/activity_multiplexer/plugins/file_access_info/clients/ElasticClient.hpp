/*
 * =====================================================================================
 *
 *       Filename:  ElasticClient.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/04/2017 02:33:08 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
 
#ifndef  ElasticClient_INC
#define  ElasticClient_INC

#include "../Debug.hpp"
#include "Basics.hpp"
#include "BaseClient.hpp"

#include <memory>


class ElasticClient : public Client {
	public:
		ElasticClient() : m_index{"siox"}, m_type{"access"} {}
		void init(const std::string& host, const std::string& port, const std::string& username, const std::string& password) override;
		std::string make_send_uri() override;
		std::string to_json(std::shared_ptr<Client::Datapoint> point) const override;
	private:
		std::string get_json_mappings() const;
		const std::string m_mappings;
		size_t m_id_counter = 0;
		const std::string m_index;
		const std::string m_type;
};



#endif   /* ----- #ifndef ElasticClient_INC  ----- */
