#include <vector>
#include <string>

#include <siox.pb.h>
#include <core/comm/messages/BufferConverter.hpp>
#include <monitoring/datatypes/Activity.hpp>


void BufferConverter::attribute_value(const VariableDatatype &s_atv,
				      buffers::AttributeValue &b_atv)
{
	switch(s_atv.type()) {
	case VariableDatatype::Type::INT32:
		b_atv.set_type(buffers::AttributeValue::INT32);
		b_atv.set_i32(s_atv.int32());
		break;
	case VariableDatatype::Type::UINT32:
		b_atv.set_type(buffers::AttributeValue::UINT32);
		b_atv.set_ui32(s_atv.uint32());
		break;
	case VariableDatatype::Type::INT64:
		b_atv.set_type(buffers::AttributeValue::INT64);
		b_atv.set_i64(s_atv.int64());
		break;
	case VariableDatatype::Type::UINT64:
		b_atv.set_type(buffers::AttributeValue::UINT64);
		b_atv.set_ui64(s_atv.uint64());
		break;
	case VariableDatatype::Type::FLOAT:
		b_atv.set_type(buffers::AttributeValue::FLOAT);
		b_atv.set_f(s_atv.flt());
		break;
	case VariableDatatype::Type::DOUBLE:
		b_atv.set_type(buffers::AttributeValue::DOUBLE);
		b_atv.set_d(s_atv.dbl());
		break;
	case VariableDatatype::Type::STRING:
		b_atv.set_type(buffers::AttributeValue::STRING);
		b_atv.set_str(s_atv.str());
		break;
	default:
		b_atv.set_type(buffers::AttributeValue::INVALID);
	}
	
}


void BufferConverter::attribute(const Attribute &s_at, buffers::Attribute &b_at)
{
	buffers::AttributeValue *b_av = new buffers::AttributeValue();
	attribute_value(s_at.value, *b_av);

	b_at.set_id(s_at.id);
	b_at.set_allocated_value(b_av);
}


void BufferConverter::remote_call_id(const RemoteCallIdentifier &s_rcid,
				     buffers::RemoteCallIdentifier &b_rcid)
{
	b_rcid.set_nid(s_rcid.nid);
	b_rcid.set_uuid(s_rcid.uuid);
	b_rcid.set_instance(s_rcid.instance);
}


void BufferConverter::remote_call(const RemoteCall &s_rc, 
				  buffers::RemoteCall &b_rc)
{
	buffers::RemoteCallIdentifier *b_target = new buffers::RemoteCallIdentifier();
	remote_call_id(s_rc.target, *b_target);
	b_rc.set_allocated_target(b_target);

	std::vector<Attribute>::const_iterator s_attr;
	for (s_attr = s_rc.attributes.begin(); 
	     s_attr < s_rc.attributes.end(); ++s_attr) {
	
		buffers::Attribute *b_attr = b_rc.add_attributes();
		attribute(*s_attr, *b_attr);

	}
	
}


void BufferConverter::aid(const ActivityID &s_aid, buffers::ActivityID &b_aid)
{
	buffers::ProcessID *b_proc = new buffers::ProcessID();
	b_proc->set_nid(s_aid.cid.pid.nid);
	b_proc->set_pid(s_aid.cid.pid.pid);
	b_proc->set_time(s_aid.cid.pid.time);
	
	buffers::ComponentID *b_cid = new buffers::ComponentID();
	b_cid->set_allocated_pid(b_proc);
	b_cid->set_id(s_aid.cid.id);
	
	b_aid.set_allocated_cid(b_cid);
	b_aid.set_id(s_aid.id);
}



void BufferConverter::activity(const Activity &s_act, buffers::Activity &b_act) 
{
	aid(s_act.aid(), *(b_act.mutable_aid()));
	
	b_act.set_ucaid(s_act.ucaid());
	b_act.set_time_start(s_act.time_start());
	b_act.set_time_stop(s_act.time_stop());
	b_act.set_error_value(s_act.errorValue());
 
	std::vector<ActivityID>::const_iterator s_aid;
	for (s_aid = s_act.parentArray().begin(); 
	     s_aid < s_act.parentArray().end(); ++s_aid) {
	
		buffers::ActivityID *b_aid = b_act.add_parents();
		aid(*s_aid, *b_aid);

	}

	std::vector<Attribute>::const_iterator s_attr;
	for (s_attr = s_act.attributeArray().begin(); 
	     s_attr < s_act.attributeArray().end(); ++s_attr) {
	
		buffers::Attribute *b_attr = b_act.add_attributes();
		attribute(*s_attr, *b_attr);

	}

	std::vector<RemoteCall>::const_iterator s_rc;
	for (s_rc = s_act.remoteCallsArray().begin(); 
	     s_rc < s_act.remoteCallsArray().end(); ++s_rc) {
	
		buffers::RemoteCall *b_rc = b_act.add_remote_calls();
		remote_call(*s_rc, *b_rc);

	}

}

