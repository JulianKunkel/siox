#ifndef SIOX_CONVERTER_HPP
#define SIOX_CONVERTER_HPP

#include <vector>

#include <siox.pb.h>
#include <monitoring/datatypes/ids.hpp>
#include <monitoring/datatypes/Activity.hpp>

using namespace monitoring;

class BufferConverter {
	public:
		void aid( const ActivityID & s_aid, buffers::ActivityID & b_aid );
		void attribute( const Attribute & s_at, buffers::Attribute & b_at );
		void attribute_value( const VariableDatatype & s_atv,
		                      buffers::AttributeValue & b_atv );
		void remote_call_id( const RemoteCallIdentifier & s_rcid,
		                     buffers::RemoteCallIdentifier & b_rcid );
		void remote_call( const RemoteCall & s_rc, buffers::RemoteCall & b_rc );
		void activity( const Activity & s_act, buffers::Activity & b_act );
};

#endif
