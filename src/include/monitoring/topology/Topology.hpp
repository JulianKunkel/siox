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
XXX: Do we need to add a facility to generate paths? I. e. something like `enumeratePaths( TopologyObjectId )`? I hesitate, because then we would need to ensure that this method would not explode when an object is its own grandfather. And that's a very common use case.

All objects and relations have a type, which is simply a string describing the kind of object/relation.
Example types for objects would be: node, block device, NIC, Network-Switch, etc.
Example types for relations are: includes, connects to, runs on, etc.

TopologyObjects may be connected to attributes that add further description.
The attribute itself is just the abstract concept of an attribute that includes a datatype for the values,
an example would be: Bandwidth (MB/s), double.
TopologyValues store a concrete value for an TopologyAttribute/TopologyObject combination.

XXX The following might need to change, should the need arise to store a reference to a relation or a value.
Both, the relations and the values have no identifier of their own, but use a combination of two members as a unique key.
Consequently, their use is a bit more implicit than the use of types, objects, and attributes.
This is especially true for values, which only become explicit when an object is asked to enumerate its attributes.



Use cases:
	U1	Store and retrieve static system information like which nodes/devices/connections/deamons/etc. the system consists of.
	U2	Talk about any part of the system by using a unique ID.
	U3	Implement an ontology using the abstractions provided by the topology (simple code reuse, because describing the topology requires more powerful abstractions than the ontology).

Requirements:
	R1	U1 => TopologyRelations between objects must be representable, and it must be possible to enumerate them.
	R2	U2 => It must be possible to lookup objects by their ID.
	R3	U3 => TopologyAttributes must consist of a name and a domain.



@startuml Topology.png

	class TopologyType {
		+uint32_t id
		+string name
	}

	class TopologyObject {
		+uint32_t id
		+uint32_t typeId
	}

	class TopologyRelation {
		+uint32_t parentObjectId
		+string childName
		+uint32_t childObjectId
		+uint32_t relationTypeId
	}

	class TopologyAttribute {
		+uint32_t id
		+string name
		+uint32_t domainTypeId
		+uint8_t dataType
	}

	class TopologyValue {
		+uint32_t objectId
		+uint32_t attributeId
		+TopologyValue value
	}

	TopologyObject "type" --> TopologyType

	TopologyRelation "parent" --> TopologyObject
	TopologyRelation "child" --> TopologyObject
	TopologyRelation "type" --> TopologyType

	TopologyValue "object" --> TopologyObject
	TopologyValue "attribute" --> TopologyAttribute

@enduml
*/


#ifndef INCLUDE_GUARD_MONITORING_TOPOLOGY_HPP
#define INCLUDE_GUARD_MONITORING_TOPOLOGY_HPP

#include <core/component/Component.hpp>
#include <monitoring/datatypes/Topology.hpp>
#include <monitoring/datatypes/Exceptions.hpp>

#include <vector>

namespace monitoring {
	class Topology : public core::Component {
		public:

			typedef std::vector<TopologyRelation> TopologyRelationList;
			typedef std::vector<TopologyValue> TopologyValueList;

			virtual TopologyType registerType( const string& name ) throw() = 0;
			virtual TopologyType lookupTypeByName( const string& name ) throw( NotFoundError ) = 0;
			virtual TopologyType lookupTypeById( TopologyTypeId anId ) throw( NotFoundError ) = 0;

			virtual TopologyObject registerObject( TopologyObjectId parent, TopologyTypeId objectType, TopologyTypeId relationType, const string& childName ) throw( IllegalStateError ) = 0;
			virtual TopologyObject lookupObjectByPath( const string& Path ) throw( NotFoundError ) = 0;
			virtual TopologyObject lookupObjectById( TopologyObjectId anId ) throw( NotFoundError ) = 0;

			virtual TopologyRelation registerRelation( TopologyTypeId relationType, TopologyObjectId parent, TopologyObjectId child, const string& childName ) throw( IllegalStateError ) = 0;
			virtual TopologyRelation lookupRelation( TopologyObjectId parent, const string& childName ) throw( NotFoundError ) = 0;

			// TopologyRelationType may be 0 which indicates any parent/child.
			virtual TopologyRelationList enumerateChildren( TopologyObjectId parent, TopologyTypeId relationType ) throw() = 0;
			virtual TopologyRelationList enumerateParents( TopologyObjectId child, TopologyTypeId relationType ) throw() = 0;


			virtual TopologyAttribute registerAttribute( TopologyTypeId domain, const string& name, VariableDatatype::Type datatype ) throw( IllegalStateError ) = 0;
			virtual TopologyAttribute lookupAttributeByName( TopologyTypeId domain, const string& name ) throw( NotFoundError ) = 0;
			virtual TopologyAttribute lookupAttributeById( TopologyAttributeId attributeId ) throw( NotFoundError ) = 0;
			virtual void setAttribute( TopologyObjectId object, TopologyAttributeId attribute, const TopologyValue& value ) throw( IllegalStateError ) = 0;
			virtual TopologyValue getAttribute( TopologyObjectId object, TopologyAttributeId attribute ) throw( NotFoundError ) = 0;
			virtual TopologyValueList enumerateAttributes( TopologyObjectId object ) throw() = 0;
	};
}

#define MONITORING_TOPOLOGY_INTERFACE "monitoring_topology"

#endif
