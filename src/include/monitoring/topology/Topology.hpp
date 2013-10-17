/**
 * @file Topology.hpp
 *
 * @author Nathanael Hübbe
 * @date 2013
 */

#ifndef INCLUDE_GUARD_MONITORING_TOPOLOGY_HPP
#define INCLUDE_GUARD_MONITORING_TOPOLOGY_HPP

#include <string>

namespace monitoring {
	class Topology : public core::Component {
		public:
			typedef uint32_t TypeId;
			typedef uint32_t ObjectId;
			typedef uint32_t RelationId;
			typedef uint32_t ValueId;
			typedef uint32_t AttributeId;

			class Type;
			class Object;
			class Relation;
			class Value;
			class Attribute;

			virtual const Type& registerType( const string& name ) throw() = 0;
			virtual const Type& lookupTypeByName( const string& name ) throw( NotFoundError ) = 0;
			virtual const Type& lookupTypeById( TypeId anId ) throw( NotFoundError ) = 0;

			virtual const Object& registerObject( ObjectId parent, TypeId objectType, TypeId relationType, string childName ) throw( IllegalStateError ) = 0;
			virtual const Object& lookupObjectById( Object anId ) throw( NotFoundError ) = 0;

			virtual const Relation& registerRelation( TypeId relationType, ObjectId parent, ObjectId child, string childName ) throw( IllegalStateError ) = 0;
			virtual const Relation& lookupRelation( ObjectId parent, string childName ) throw( NotFoundError ) = 0;
			virtual vector<const Relation&> enumerateChildren( ObjectId parent ) throw() = 0;
			virtual vector<const Relation&> enumerateParents( ObjectId child ) throw() = 0;

			virtual const Attribute& registerAttribute( string name, string datatype ) throw( IllegalStateError ) = 0;
			virtual const Attribute& lookupAttribute( string name ) throw( NotFoundError ) = 0;

			virtual const Value& registerValue( ObjectId object, AttributeId, string value ) throw( IllegalStateError ) = 0;
			virtual const Value& lookupValueByObject( ObjectId object, AttributeId ) throw( NotFoundError ) = 0;
			virtual const Value& lookupValueById( ValueId ) throw( NotFoundError ) = 0;
			virtual vector<const Value&> enumerateAttributes( ObjectId object ) throw() = 0;

	};
}

#define TOPOLOGY_INTERFACE "monitoring_topology"

#endif
