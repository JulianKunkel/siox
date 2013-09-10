#include <string>

#include "../../siox.pb.h"
#include <misc/Util.hpp>
#include <core/comm/messages/BufferConverter.hpp>

void attribute_value_test()
{
	BufferConverter c;

	const int64_t int64 = 64;
	const VariableDatatype d64(int64);

	if (int64 != d64.int64())
		util::fail("SIOX INT64");

	buffers::AttributeValue *b64 = new buffers::AttributeValue();
	c.attribute_value (d64, *b64);

	if (d64.int64() != b64->i64())
		util::fail("Buffer INT64");

	const int32_t int32 = 32;
	const VariableDatatype d32(int32);

	if (int32 != d32.int32())
		util::fail("SIOX INT32");

	buffers::AttributeValue *b32 = new buffers::AttributeValue();
	c.attribute_value (d32, *b32);

	if (d32.int32() != b32->i32())
		util::fail("BUFFERS INT32");

	const uint64_t uint64 = 64;
	const VariableDatatype du64(uint64);

	if (uint64 != du64.uint64())
		util::fail("SIOX UINT64");

	buffers::AttributeValue *bu64 = new buffers::AttributeValue();
	c.attribute_value (du64, *bu64);

	if (du64.uint64() != bu64->ui64())
		util::fail("BUFFERS UINT64");

	const float f = 0.11;
	const VariableDatatype df(f);

	if (f != df.flt())
		util::fail("SIOX FLOAT");

	buffers::AttributeValue *bf = new buffers::AttributeValue();
	c.attribute_value(df, *bf);

	if (df.flt() != bf->f())
		util::fail("BUFFERS FLOAT");

	const double d = 0.22;
	const VariableDatatype dd(d);

	if (d != dd.dbl())
		util::fail("SIOX DOUBLE");

	buffers::AttributeValue *bd = new buffers::AttributeValue();
	c.attribute_value(dd, *bd);

	if (dd.dbl() != bd->d())
		util::fail("BUFFERS DOUBLE");

	const std::string str = "hello siox!";
	const VariableDatatype ds(str);

	if (str != ds.str())
		util::fail("SIOX STRING");

	buffers::AttributeValue *bs = new buffers::AttributeValue();
	buffers::AttributeValue &bss = *bs;

	c.attribute_value(ds, bss);

	if (ds.str() != bss.str())
		util::fail("BUFFERS STRING");
}


void attribute_test()
{
	BufferConverter c;

	const std::string str = "it's me again!";
	const AttributeValue value (str);
	const OntologyAttributeID id = 100;

	const Attribute attr (id, value);

	if (id != attr.id)
		util::fail("SIOX Attribute-ID");
	
	if (value != attr.value)
		util::fail("SIOX Attribute-Value");

	buffers::Attribute battr;
	c.attribute (attr, battr);

	if (attr.id != battr.id())
		util::fail("Buffers Attribute-ID");
	
	if (value.str() != battr.value().str())
		util::fail("Buffers Attribute-Value");

	const uint64_t i64 = 42;
	const AttributeValue value2 (i64);
	const OntologyAttributeID id2 = 100;

	const Attribute attr2 (id2, value2);

	if (id2 != attr2.id)
		util::fail("SIOX Attribute-ID");
	
	if (value2 != attr2.value)
		util::fail("SIOX Attribute-Value");

	buffers::Attribute battr2;
	c.attribute (attr2, battr2);

	if (attr2.id != battr2.id())
	    util::fail("Buffers Attribute-ID");
	
	if (value2.uint64() != battr2.value().ui64())
		util::fail("Buffers Attribute-ID");

}


void remote_call_id_test()
{
	BufferConverter c;

	const NodeID nid = 100;
	const UniqueComponentActivityID uuid = 200;
	const AssociateID instance = 300;
	const RemoteCallIdentifier rc (nid, uuid, instance);

	if (nid != rc.nid)
		util::fail("SIOX RemoteCall-NID");
	if (uuid != rc.uuid)
		util::fail("SIOX RemoteCall-UUID");
	if (instance != rc.instance)
		util::fail("SIOX RemoteCall-INSTANCE");

	buffers::RemoteCallIdentifier brc;
	c.remote_call_id (rc, brc);

	if (nid != brc.nid())
		util::fail("Buffers RemoteCall-NID");
	if (uuid != brc.uuid())
		util::fail("Buffers RemoteCall-UUID");
	if (instance != brc.instance())
		util::fail("Buffers RemoteCall-INSTANCE");
}


void remote_call_test()
{
	BufferConverter c;

	const NodeID nid = 10;
	const UniqueComponentActivityID uuid = 20;
	const AssociateID instance = 30;
	const RemoteCallIdentifier target (nid, uuid, instance);

	vector<Attribute> attributes;

	const std::string str = "it's me again!";
	const AttributeValue value (str);
	const OntologyAttributeID id = 100;
	const Attribute attr (id, value);
	attributes.push_back (attr);

	const uint64_t i64 = 42;
	const AttributeValue value2 (i64);
	const OntologyAttributeID id2 = 100;
	const Attribute attr2 (id2, value2);
	attributes.push_back (attr2);

	const RemoteCall rc = { target, attributes };

	buffers::RemoteCall brc;
	c.remote_call (rc, brc);

	if (rc.target.nid != brc.target().nid())
		util::fail("RemoteCall-NID");
	if (rc.target.uuid != brc.target().uuid())
		util::fail("RemoteCall-UUID");
	if (rc.target.instance != brc.target().instance())
		util::fail("RemoteCall-INSTANCE");

	if (rc.attributes.size() != (unsigned) brc.attributes_size())
		util::fail("Different attribute count.");

	vector<Attribute>::const_iterator i;
	for (i = rc.attributes.begin(); i < rc.attributes.end(); ++i) {

		buffers::Attribute a1;
		c.attribute (*i, a1);

		bool found = false;

		for (int j = 0; j < brc.attributes_size();  j++) {

			const buffers::Attribute &a2 = brc.attributes (j);
			if (a1.DebugString() == a2.DebugString()) {

				found = true;
				break;

			}

		}

		if (!found) {
			util::fail("Attribute not found.");
		}

	}
}


void activityid_test()
{
	BufferConverter c;

	const uint32_t nid = 10, pid = 20, time = 30, aid = 40;
	const uint16_t cid = 50;
	const ProcessID proc = { nid, pid, time };
	const ComponentID comp = { proc, cid };
	const ActivityID act = { comp, aid };

	if (nid != proc.nid)
		util::fail("Activity-NID");
	if (pid != proc.pid)
		util::fail("Activity-PID");
	if (time != proc.time)
		util::fail("Activity-TIME");
	if (comp.pid != proc)
		util::fail("Activity-PID");
	if (act.cid != comp)
		util::fail("Activity-COMPONENT");
	if (act.id != aid)
		util::fail("Activity-ID");

	buffers::ActivityID bact;
	c.aid (act, bact);

	if (act.id != bact.id())
		util::fail("Buffer Activity-ID");
	if (act.cid.id != bact.cid().id())
		util::fail("Buffer Activity-CID");
	if (act.cid.pid.nid != bact.cid().pid().nid())
		util::fail("Buffer Activity-NID");
	if (act.cid.pid.pid != bact.cid().pid().pid())
		util::fail("Buffer Activity-PID");
	if (act.cid.pid.time != bact.cid().pid().time())
		util::fail("Buffer Activity-TIME");

}


void activity_test()
{
	BufferConverter c;

	UniqueComponentActivityID ucaid = 10;
	Timestamp time_start = 20, time_end = 30;
	uint32_t nid = 40, pid = 50, time = 60, aid = 70;
	uint16_t cid = 80;
	ProcessID proc = { nid, pid, time };
	ComponentID comp = { proc, cid };
	ActivityID actid = { comp, aid };
	ActivityError error = 404;
	RemoteCallIdentifier *remote_invoker = NULL;

	ProcessID proc2 = { 110, 210, 310 };
	ComponentID comp2 = { proc2, 410 };
	ActivityID actid2 = { comp2, 510 };

	ProcessID proc3 = { 120, 220, 320 };
	ComponentID comp3 = { proc3, 420 };
	ActivityID actid3 = { comp3, 520 };

	vector<ActivityID> parents;
	parents.push_back (actid2);
	parents.push_back (actid3);

	const std::string str1 = "klingon functions don't have parameters, they have arguments, and always win them!";
	const AttributeValue value1 (str1);
	const Attribute attr1 (100, value1);

	const AttributeValue value2 (42);
	const Attribute attr2 (101, value2);

	vector<Attribute> attributes;
	attributes.push_back (attr1);
	attributes.push_back (attr2);

	const RemoteCallIdentifier target1 (5, 10, 15);
	const RemoteCallIdentifier target2 (10, 20, 30);

	const std::string str3 = "all your base are belong to us";
	const AttributeValue value3 (str3);
	const Attribute attr3 (100, value3);

	const AttributeValue value4 (42);
	const Attribute attr4 (103, value4);
	attributes.push_back (attr4);

	vector<Attribute> attributes2;
	vector<Attribute> attributes3;
	attributes2.push_back (attr3);
	attributes2.push_back (attr4);

	RemoteCall rc1 = { target1, attributes2 };
	RemoteCall rc2 = { target1, attributes3 };

	vector<RemoteCall> remote_calls;
	remote_calls.push_back (rc1);
	remote_calls.push_back (rc2);

	Activity act (ucaid, time_start, time_end, actid, parents,
	              attributes, remote_calls, remote_invoker, error);

	buffers::Activity bact;

	c.activity (act, bact);

	if (act.ucaid() != bact.ucaid())
		util::fail("Activity-UCAID");
	if (act.time_start() != bact.time_start())
		util::fail("Activity-TIME-START");
	if (act.time_stop() != bact.time_stop())
		util::fail("Activity-TIME-STOP");
	if (act.errorValue() != bact.error_value())
		util::fail("Activity-ERROR-VALUE");

	if (act.aid().id != bact.aid().id())
		util::fail("Activity-ID");
	if (act.aid().cid.id != bact.aid().cid().id())
		util::fail("Activity-CID");
	if (act.aid().cid.pid.nid != bact.aid().cid().pid().nid())
		util::fail("Activity-NID");
	if (act.aid().cid.pid.pid != bact.aid().cid().pid().pid())
		util::fail("Activity-PID");
	if (act.aid().cid.pid.time != bact.aid().cid().pid().time())
		util::fail("Activity-TIME");

	if (act.parentArray().size() != (unsigned) bact.parents_size())
		util::fail("Different parent count.");

	for (vector<ActivityID>::const_iterator i = act.parentArray().begin();
	        i < act.parentArray().end(); i++) {

		buffers::ActivityID aid1;
		c.aid (*i, aid1);

		bool found = false;

		for (int j = 0; j < bact.parents_size(); j++) {

			const buffers::ActivityID &aid2 = bact.parents (j);
			if (aid1.DebugString() == aid2.DebugString()) {

				found = true;
				break;

			}

		}

		if (!found) {
			util::fail("Attribute not found.");
		}

	}

	if (act.attributeArray().size() != (unsigned) bact.attributes_size())
		util::fail("Different attribute count.");

	for (vector<Attribute>::const_iterator i = act.attributeArray().begin();
	        i < act.attributeArray().end(); i++) {

		buffers::Attribute attr1;
		c.attribute (*i, attr1);

		bool found = false;

		for (int j = 0; j < bact.attributes_size();  j++) {

			const buffers::Attribute &attr2 = bact.attributes (j);
			if (attr1.DebugString() == attr2.DebugString()) {

				found = true;
				break;

			}

		}

		if (!found) {
			util::fail("Attribute not found.");
		}

	}

	if (act.remoteCallsArray().size() != (unsigned) bact.remote_calls_size())
		util::fail("Different remote call count.");

	for (vector<RemoteCall>::const_iterator i = act.remoteCallsArray().begin();
	        i < act.remoteCallsArray().end(); i++) {

		buffers::RemoteCall rc1;
		c.remote_call (*i, rc1);

		bool found = false;

		for (int j = 0; j < bact.remote_calls_size();  j++) {

			const buffers::RemoteCall &rc2 = bact.remote_calls (j);
			if (rc1.DebugString() == rc2.DebugString()) {

				found = true;
				break;

			}

		}

		if (!found) {
			util::fail("Attribute not found.");
		}

	}
}

int main()
{
	attribute_value_test();
	attribute_test();
	remote_call_id_test();
	remote_call_test();
	activityid_test();
	activity_test();
	
	return 0;
}

