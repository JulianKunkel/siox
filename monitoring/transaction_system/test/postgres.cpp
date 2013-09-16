#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <sys/types.h>
#include <unistd.h>

#include <monitoring/datatypes/Activity.hpp>
#include <monitoring/transaction_system/PostgreSQLBackend.hpp>
#include <monitoring/transaction_system/PostgreSQLPumpCallback.hpp>
#include <monitoring/transaction_system/TransactionBackend.hpp>
#include <monitoring/transaction_system/test/PostgreSQLReader.hpp>

#include <core/logger/SioxLogger.hpp>
#include <misc/Util.hpp>
#include <misc/Rnd.hpp>

//#define SIOX_DB_INFO "host = '127.0.0.1' port = '5432' user = 'postgres' password = 'siox.db.X916' dbname = 'siox' options = '-c log_error_verbosity=VERBOSE -c client_min_messages=DEBUG5 -c log_min_messages=DEBUG5 -c log_min_error_statement=DEBUG5 -c debug_pretty_print=1 -c log_statement=all' "
#define SIOX_DB_INFO "host = '127.0.0.1' port = '5432' user = 'postgres' password = 'siox.db.X916' dbname = 'siox'"
/*
  Reset DB:

  truncate activity, node, hardware, software, instance;
  alter sequence node_id_seq restart with 1;
  alter sequence hardware_id_seq restart with 1;
  alter sequence software_id_seq restart with 1;
  alter sequence instance_id_seq restart with 1;

 */


Logger *logger;
PostgreSQLReader *reader;

class PostgreSQLPumpCallbackTest {
public:
	PostgreSQLPumpCallbackTest(PostgreSQLPumpCallback *cb) : cb_(cb) 
	{ }

	void save_node(const std::string &hwdesc, const std::string &swdesc, const std::string &indesc) 
	{
// 		cb_->store_node(hwdesc, swdesc, indesc);
	}

	void save_activity(const Activity &act) 
	{
		std::cout << "Activity ID: " << act.aid().id << std::endl;
// 		cb_->store_activity(act);
	}

	uint64_t save_activity_id(const ActivityID &aid) 
	{
		return cb_->insert_activity_id(aid);
	}

private:
	PostgreSQLPumpCallback *cb_;
};


void activity_id_test(PostgreSQLPumpCallbackTest &cb)
{
	ActivityID *aid1 = rnd::activity_id();
	uint64_t uid = cb.save_activity_id(*aid1);
	
	ActivityID *aid2 = reader->activity_id(uid);
	
	std::cout << "ID: " << aid2->id << " THREAD: " << aid2->thread << std::endl;
	
	if (*aid1 != *aid2) {
		std::cerr << "Activity ID mismatch. (" << aid1->id << ") != (" << aid2->id << ")" << std::endl;
		exit(1);
	}
	
}


int main()
{
	if (!logger)
		logger = new Logger();

	TransactionBackend *be = new PostgreSQLBackend();
	be->init(SIOX_DB_INFO);
	
	Callback *cb = be->create_callback();
	PostgreSQLPumpCallback *pcb = dynamic_cast<PostgreSQLPumpCallback *>(cb);
	PostgreSQLPumpCallbackTest pg(pcb);

	reader = new PostgreSQLReader(SIOX_DB_INFO);
	
	srand(time(NULL));
	
	activity_id_test(pg);
	
	return 0;
}


// void node_test()
// {
// 	if (!logger)
// 		logger = new Logger();
// 
// 	TransactionBackend *be = new PostgreSQLBackend(SIOX_DB_INFO);
// 	Callback *cb = be->create_callback();
// 	PostgreSQLPumpCallback *pcb = dynamic_cast<PostgreSQLPumpCallback *>(cb);
// 	PostgreSQLPumpCallbackTest pg(pcb);
// 
// 	for (int i = 1; i < 10; ++i) {
// 		std::ostringstream hw, sw, in;
// 		hw << "Hardware " << i;
// 		sw << "Software " << i;
// 		in << "Instance " << i;
// 
// 		pg.save_node(hw.str(), sw.str(), in.str());
// 	}
// }
// 
// 
// void activity_test()
// {
// 	if (!logger)
// 		logger = new Logger();
// 
// 	UniqueComponentActivityID ucaid = 100;
// 	siox_timestamp time_start = util::time64(), time_stop = 330;
// 
// 	ProcessID p1 = {1, getpid(), util::time32()};
// 	ProcessID p2 = {2, getpid(), util::time32()};
// 	ProcessID p3 = {3, getpid(), util::time32()};
// 
// 	ComponentID c1 = {p1, 10};
// 	ComponentID c2 = {p2, 20};
// 	ComponentID c3 = {p3, 30};
// 
// 	ActivityID aid1 = {c1, 100}, aid2 = {c2, 200}, aid3 = {c3, 300};
// 	vector<ActivityID> parents;
// 	parents.push_back( aid2 );
// 	parents.push_back( aid3 );
// 
// 	Attribute a1 = {1, "attribute 1"}, a2 = {2, "attribute 2"};
// 	vector<Attribute> attributes;
// //  attributes.push_back(a1);
// //  attributes.push_back(a2);
// 
// 	UniqueInterfaceID uuid = {10, 20};
// 	AssociateID instance = 30;
// //  RemoteCallIdentifier target = {1, uuid, instance};
// 
// //  RemoteCall rc = {target, attributes};
// 	vector<RemoteCall> remote_calls;
// //  remote_calls.push_back(rc);
// 
// 	RemoteCallIdentifier *remote_invoker = NULL;
// 	siox_activity_error error_value = 0;
// 
// 	Activity act(ucaid, time_start, time_stop, aid1, parents, attributes, remote_calls, remote_invoker, error_value);
// 
// 	TransactionBackend *be = new PostgreSQLBackend(SIOX_DB_INFO);
// 	Callback *cb = be->create_callback();
// 	PostgreSQLPumpCallback *pcb = dynamic_cast<PostgreSQLPumpCallback *>(cb);
// 	PostgreSQLPumpCallbackTest pg(pcb);
// 
// 	pg.save_activity(act);
// }
// 
