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
This root object may be retrieved by calling `lookupObjectById(0)`.
The parent/child relationships may be used to lookup an object by a given path, specifying a sequence of child names starting from the root object.
Since objects may have multiple parents, it is possible to look up the same object from different paths.
XXX: Do we need to add a facility to generate paths? I. e. something like `enumeratePaths( ObjectId )`? I hesitate, because then we would need to ensure that this method would not explode when an object is its own grandfather. And that's a very common use case.

All objects and relations have a type, which is simply a string describing the kind of object/relation.
Example types for objects would be: node, block device, NIC, Network-Switch, etc.
Example types for relations are: includes, connects to, runs on, etc.

Objects may be connected to attributes that add further description.
The attribute itself is just the abstract concept of an attribute that includes a datatype for the values,
an example would be: Bandwidth (MB/s), double.
Values store a concrete value for an Attribute/Object combination.

XXX The following might need to change, should the need arise to store a reference to a relation or a value.
Both, the relations and the values have no identifier of their own, but use a combination of two members as a unique key.
Consequently, their use is a bit more implicit than the use of types, objects, and attributes.
This is especially true for values, which only become explicit when an object is asked to enumerate its attributes.



Use cases:
	U1	Store and retrieve static system information like which nodes/devices/connections/deamons/etc. the system consists of.
	U2	Talk about any part of the system by using a unique ID.
	U3	Implement an ontology using the abstractions provided by the topology (simple code reuse, because describing the topology requires more powerful abstractions than the ontology).

Requirements:
	R1	U1 => Relations between objects must be representable, and it must be possible to enumerate them.
	R2	U2 => It must be possible to lookup objects by their ID.
	R3	U3 => Attributes must consist of a name and a domain.



@startuml Topology.png

	class Type {
		+uint32_t id
		+string name
	}

	class Object {
		+uint32_t id
		+uint32_t typeId
	}

	class Relation {
		+uint32_t parentObjectId
		+string childName
		+uint32_t childObjectId
		+uint32_t relationTypeId
	}

	class Attribute {
		+uint32_t id
		+string name
		+uint32_t domainTypeId
		+uint8_t dataType
	}

	class Value {
		+uint32_t objectId
		+uint32_t attributeId
		+TopologyValue value
	}

	Object "type" --> Type

	Relation "parent" --> Object
	Relation "child" --> Object
	Relation "type" --> Type

	Value "object" --> Object
	Value "attribute" --> Attribute

@enduml
*/


#ifndef INCLUDE_GUARD_MONITORING_TOPOLOGY_HPP
#define INCLUDE_GUARD_MONITORING_TOPOLOGY_HPP

#include <core/component/Component.hpp>
#include <core/datatypes/VariableDatatype.hpp>
#include <monitoring/datatypes/Exceptions.hpp>

#include <vector>
#include <string>

namespace monitoring {
	class Topology : public core::Component {
		public:
			typedef VariableDatatype TopologyValue;

			typedef uint32_t TypeId;
			typedef uint32_t ObjectId;
			typedef uint32_t RelationId;
			typedef uint32_t ValueId;
			typedef uint32_t AttributeId;

			typedef struct Type {
				string name;
				TypeId id;	//SQL key
			} Type;
			typedef struct Object {
				ObjectId id;	//SQL key
				TypeId typeId;
			} Object;
			typedef struct Relation {
				//The combination of childName and parentId is unique and used as the SQL key.
				string childName;	//SQL key
				ObjectId parentId;	//SQL key
				ObjectId childId;
				TypeId typeId;
			} Relation;
			typedef struct Attribute {
				string name;
				AttributeId id;	//SQL key
				TypeId domainId;	//XXX Maybe, we should change this to a string.
				VariableDatatype::Type dataType;
			} Attribute;
			typedef struct Value {
				//The combination of object and attribute is unique and used as the SQL key.
				TopologyValue value;
				ObjectId object;	//SQL key
				AttributeId attribute;	//SQL key
			} Value;

			virtual const Type& registerType( const string& name ) throw() = 0;
			virtual const Type& lookupTypeByName( const string& name ) throw( NotFoundError ) = 0;
			virtual const Type& lookupTypeById( TypeId anId ) throw( NotFoundError ) = 0;

			virtual const Object& registerObject( ObjectId parent, TypeId objectType, TypeId relationType, const string& childName ) throw( IllegalStateError ) = 0;
			virtual const Object& lookupObjectByPath( const string& Path ) throw( NotFoundError ) = 0;
			virtual const Object& lookupObjectById( Object anId ) throw( NotFoundError ) = 0;	//Passing 0 yields a reference to the root object.

			virtual const Relation& registerRelation( TypeId relationType, ObjectId parent, ObjectId child, const string& childName ) throw( IllegalStateError ) = 0;
			virtual const Relation& lookupRelation( ObjectId parent, const string& childName ) throw( NotFoundError ) = 0;

			// RelationType may be 0 which indicates any parent/child.
			virtual std::vector<const Relation*> enumerateChildren( ObjectId parent, TypeId relationType ) throw() = 0;
			virtual std::vector<const Relation*> enumerateParents( ObjectId child, TypeId relationType ) throw() = 0;


			virtual const Attribute& registerAttribute( TypeId domain, const string& name, VariableDatatype::Type datatype ) throw( IllegalStateError ) = 0;
			virtual const Attribute& lookupAttributeByName( TypeId domain, const string& name ) throw( NotFoundError ) = 0;
			virtual const Attribute& lookupAttributeById( AttributeId attributeId ) throw( NotFoundError ) = 0;
			virtual void setAttribute( ObjectId object, AttributeId attribute, const TopologyValue& value ) throw( IllegalStateError ) = 0;
			virtual const TopologyValue& getAttribute( ObjectId object, AttributeId attribute ) throw( NotFoundError ) = 0;
			virtual std::vector<const Value*> enumerateAttributes( ObjectId object ) throw() = 0;
	};
}

#define MONITORING_TOPOLOGY_INTERFACE "monitoring_topology"

#endif
