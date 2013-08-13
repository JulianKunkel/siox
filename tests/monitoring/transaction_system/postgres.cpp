#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Main
#include <boost/test/unit_test.hpp>
#include <boost/thread.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <sys/types.h>
#include <unistd.h>

#include "siox-types.h"

#include "ids.hpp"
#include "Activity.hpp"
#include "PostgreSQLBackend.hpp"
#include "PostgreSQLPumpCallback.hpp"
#include "TransactionBackend.hpp"
#include "SioxLogger.hpp"
#include "Util.hpp"

//#define SIOX_DB_INFO "host = '127.0.0.1' port = '5432' user = 'postgres' password = 'siox.db.X916' dbname = 'siox-ts' options = '-c log_error_verbosity=VERBOSE -c client_min_messages=DEBUG5 -c log_min_messages=DEBUG5 -c log_min_error_statement=DEBUG5 -c debug_pretty_print=1 -c log_statement=all' "
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
	
	void save_activity(const Activity &act)
	{
		std::cout << "Activity ID: " << act.aid().id << std::endl;
		
		cb_->store_activity(act);
	}
	
private:
	PostgreSQLPumpCallback *cb_;
};


BOOST_AUTO_TEST_CASE(node_test)
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


BOOST_AUTO_TEST_CASE(activity_test)
{
	if (!logger)
		logger = new Logger();
	
	UniqueComponentActivityID ucaid = 100;
	siox_timestamp time_start = util::time64(), time_stop = 330;
	
	ProcessID p1 = {1, getpid(), util::time32()};
	ProcessID p2 = {2, getpid(), util::time32()};
	ProcessID p3 = {3, getpid(), util::time32()};
	
	ComponentID c1 = {p1, 10};
	ComponentID c2 = {p2, 20};
	ComponentID c3 = {p3, 30};
	
	ActivityID aid1 = {c1, 100}, aid2 = {c2, 200}, aid3 = {c3, 300};
	vector<ActivityID> parents;
	parents.push_back(aid2);
	parents.push_back(aid3);

	Attribute a1 = {1, "attribute 1"}, a2 = {2, "attribute 2"};
	vector<Attribute> attributes;
// 	attributes.push_back(a1);
// 	attributes.push_back(a2);
	
	UniqueInterfaceID uuid = {10, 20};
	AssociateID instance = 30;
// 	RemoteCallIdentifier target = {1, uuid, instance};
	
// 	RemoteCall rc = {target, attributes};
	vector<RemoteCall> remote_calls;
// 	remote_calls.push_back(rc);
	
	RemoteCallIdentifier *remote_invoker = NULL;
	siox_activity_error error_value = 0;
	
	Activity act(ucaid, time_start, time_stop, aid1, parents, attributes, 
		     remote_calls, remote_invoker, error_value);
	
	TransactionBackend *be = new PostgreSQLBackend(SIOX_DB_INFO);
	Callback *cb = be->create_callback();
	PostgreSQLPumpCallback *pcb = dynamic_cast<PostgreSQLPumpCallback *>(cb);
	PostgreSQLPumpCallbackTest pg(pcb);

	pg.save_activity(act);
}

