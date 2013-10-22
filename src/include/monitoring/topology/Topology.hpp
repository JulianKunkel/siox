/**
 * @file Topology.hpp
 *
 * @author Nathanael Hübbe
 * @date 2013
 */

/*
The topology is used to locate any node/device/software component within a cluster.

These objects are connected via parent/child relations, each child has a unique name in the context of its parent.
All objects must have at least one parent, the only exception is the root object which has none.
These parent/child relationships may be used to lookup an object by a given path, specifying a sequence of child names starting from the root object.
Since objects may have multiple parents, it is possible to look up the same object from different paths.
XXX: Do we need to add a facility to generate paths? I. e. something like `enumeratePaths( ObjectId )`? I hesitate, because then we would need to ensure that this method would not explode when an object is its own grandfather.

All objects and relations have a type, which is simply a string describing the kind of object/relation.
Example types for objects would be: node, HDD, NIC, Network-Switch, etc.
Example types for relations are: includes, connects to, runs on, etc.

Objects may be connected to attributes that add further description.
The attribute itself is just the abstract concept of an attribute that includes a datatype for the values,
an example would be: Bandwidth (MB/s), double.
Values store a concrete value for an Attribute/Object combination.



@startuml Topology.png

	class Type {
		+uint32_t id
		+string name
	}

	class Object {
		+uint32_t id
		+Type type
	}

	class Relation {
		+Object parent
		+string childName
		+Object child
		+Type type
	}

	class Attribute {
		+uint32_t id
		+string name
		+string dataType
	}

	class Value {
		+Object object
		+Attribute attribute
		+string value
	}

	Object --> Type

	Relation --> Object
	Relation --> Object
	Relation --> Type

	Value --> Object
	Value --> Attribute

@enduml
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
			virtual const Object& rootObject() throw() = 0;
			virtual const Object& lookupObjectById( Object anId ) throw( NotFoundError ) = 0;
			virtual const Object& lookupObjectByPath( string Path ) throw( NotFoundError ) = 0;

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
