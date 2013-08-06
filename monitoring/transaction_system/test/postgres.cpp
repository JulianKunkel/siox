#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Main
#include <boost/test/unit_test.hpp>
#include <boost/thread.hpp>

#include <iostream>
#include <sstream>
#include <string>

#include "PostgreSQLBackend.hpp"
#include "PostgreSQLPumpCallback.hpp"
#include "TransactionBackend.hpp"
#include "SioxLogger.hpp"

#define SIOX_DB_INFO "host = '127.0.0.1' port = '5432' user = 'postgres' password = 'siox.db.X916' dbname = 'siox-ts'"

/*
  Reset DB:
  
  truncate activity, node, hardware, software, instance;
  alter sequence node_id_seq restart with 1;
  alter sequence hardware_id_seq restart with 1;
  alter sequence software_id_seq restart with 1;
  alter sequence instance_id_seq restart with 1;
  
 */


Logger *logger;

class PostgreSQLPumpCallbackTest {
public:
	PostgreSQLPumpCallbackTest(PostgreSQLPumpCallback *cb) : cb_(cb)
	{
	}
	
	void save_node(const std::string &hwdesc, const std::string &swdesc,
		       const std::string &indesc)
	{
		cb_->store_node(hwdesc, swdesc, indesc);
	}
	
private:
	PostgreSQLPumpCallback *cb_;
};


BOOST_AUTO_TEST_CASE(postgresql)
{
	if (!logger)
		logger = new Logger();
	
	TransactionBackend *be = new PostgreSQLBackend(SIOX_DB_INFO);
	Callback *cb = be->create_callback();
	PostgreSQLPumpCallback *pcb = dynamic_cast<PostgreSQLPumpCallback *>(cb);
	PostgreSQLPumpCallbackTest pg(pcb);
	
	for (int i = 1; i < 10; ++i) {
		std::ostringstream hw, sw, in;
		hw << "Hardware " << i;
		sw << "Software " << i;
		in << "Instance " << i;
	
 		pg.save_node(hw.str(), sw.str(), in.str());
	}
}


