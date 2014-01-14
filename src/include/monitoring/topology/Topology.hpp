/**
 * @file Topology.hpp
 *
 * @author Nathanael Hübbe
 * @date 2013
 */

/*
The topology is used to locate any node/device/software component within a cluster.

These objects are connected via parent/child relations, each child has a unique name in the context of its parent.
All objects must have at least one parent, the only exception is the (implicit) root object which has none.
The parent/child relationships may be used to lookup an object by a given path, specifying a sequence of relation type and child name pairs starting from the root object.
Since objects may have multiple parents, it is possible to look up the same object from different paths.
XXX: Do we need to add a facility to generate paths? I. e. something like `enumeratePaths( TopologyObjectId )`? I hesitate, because then we would need to ensure that this method would not explode when an object is its own grandfather. And that's a very common use case.
@todo TODO: As it turns out, specifying childs by a unique (parent, childName) combination is insufficient, we should change the specification so that just the tupel (parent, relationType, childName) is unique. The topology path format has already been changed accordingly.

The format of the topology paths is this:
	path ::= pathComponent [ /pathComponent ...]
	pathComponent ::= relationType:childName [ :objectType ]
	pathComponent ::= aliasName
	aliasName ::= @string
The semantics for each path component are as follows:
	Lookup of "relationType:childName": The returned object may have any type.
	Lookup of "relationType:childName:childType": The returned object must have the given type.
	Register of "relationType:childName": Equivalent to register of "relationType:childName:relationType".
	Register of "relationType:childName:childType", no preexisting object: Just register with the given types and child name.
	Register of "relationType:childName:childType", preexisting object: Fail if preexisting childType does not match the given childType.
	If the path component is an alias, it is first expanded and then reexamined.

Aliases map an almost arbitrary string to a path component. As far as the topology is concerned, this is purely syntactic sugar; aliases are not persisted, they are not communicated to another Topology object, they are only used within the path methods, and they are implemented directly in the Topology class. These are the rules for aliases (they are enforced by setAlias() ):
	Once set, an alias must not be changed. However, it is not an error to set an alias twice with the same value.
	An alias name must begin with the character '@'.
	An alias name must not contain a slash '/'.
	An alias value must not contain a slash '/'.
	An alias value must be a valid pathComponent.
	An alias value may be the name of another alias.

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

Example:
	TopologyType
	N "Node"
	D "Device"
	S "Steckdose"
	P "Plug"
	R "Rack"
	H "Hostname"
	s "SerialNumber"

	TopologyObject
	3 (Typ N)
	4 (Typ D)
	5 (Typ S)
	6 (Typ R)
	7 (Typ S)

	TopologyRelation
	0 "West1"     3 H
	3 "/dev/sda1" 4 D
	0 "(Rack) 0" 6 R
	6 "S 0" 5 S
	6 "S 1" 7 S
	5 "Plug 0" 3 P
	6 "West1" 3 H
	0 "SCHUH4711" 3 s
*/


#ifndef INCLUDE_GUARD_MONITORING_TOPOLOGY_HPP
#define INCLUDE_GUARD_MONITORING_TOPOLOGY_HPP

#include <util/ExceptionHandling.hpp>
#include <core/component/Component.hpp>
#include <monitoring/datatypes/Topology.hpp>
#include <monitoring/datatypes/Exceptions.hpp>

#include <vector>
#include <unordered_map>
#include <boost/thread/shared_mutex.hpp>

namespace monitoring {

	typedef std::vector<TopologyRelation> TopologyRelationList;
	typedef std::vector<TopologyValue> TopologyValueList;
				
	class Topology : public core::Component {
		//lookupXXX() members generally indicate failure by returning a false object, i. e. you are free to write
		//    if( TopologyObject myObject = topology->lookupObjectByPath( "foo/bar" ) ) {
		//        //do stuff with myObject
		//    }
		//without fearing any exceptions...
		public:

			//High level functions for easy handling of topology objects by specifying paths.
			//These are implemented as metaalgorithms directly in this class, calling the plumbing level functions to do their work.
			virtual TopologyObject registerObjectByPath( const string& path, TopologyObjectId parent = 0 ) throw();
			virtual TopologyObject lookupObjectByPath( const string& path, TopologyObjectId parent = 0 ) throw();


			virtual bool setAlias( const string& aliasName, const string& aliasValue ) throw();	//Return true on success. For requirements on the alias name and value, see the class comment above.

			//The normal, plumbing level functions.
			virtual TopologyType registerType( const string& name ) throw() = 0;
			virtual TopologyType lookupTypeByName( const string& name ) throw() = 0;
			virtual TopologyType lookupTypeById( TopologyTypeId anId ) throw() = 0;

			virtual TopologyObject registerObject( TopologyObjectId parent, TopologyTypeId relationType, const string& childName, TopologyTypeId objectType ) throw() = 0;
			virtual TopologyObject lookupObjectById( TopologyObjectId anId ) throw() = 0;

			virtual TopologyRelation registerRelation( TopologyObjectId parent, TopologyTypeId relationType, const string& childName, TopologyObjectId child ) throw() = 0;
			virtual TopologyRelation lookupRelation( TopologyObjectId parent, TopologyTypeId relationType, const string& childName ) throw() = 0;

			// TopologyRelationType may be 0 which indicates any parent/child.
			virtual TopologyRelationList enumerateChildren( TopologyObjectId parent, TopologyTypeId relationType ) throw() = 0;
			virtual TopologyRelationList enumerateParents( TopologyObjectId child, TopologyTypeId relationType ) throw() = 0;


			virtual TopologyAttribute registerAttribute( TopologyTypeId domain, const string& name, VariableDatatype::Type datatype ) throw() = 0;
			virtual TopologyAttribute lookupAttributeByName( TopologyTypeId domain, const string& name ) throw() = 0;
			virtual TopologyAttribute lookupAttributeById( TopologyAttributeId attributeId ) throw() = 0;
			virtual TopologyValue setAttribute( TopologyObjectId object, TopologyAttributeId attribute, const TopologyVariable& value ) throw() = 0;
			virtual TopologyValue getAttribute( TopologyObjectId object, TopologyAttributeId attribute ) throw() = 0;
			virtual TopologyValueList enumerateAttributes( TopologyObjectId object ) throw() = 0;

		private:
			boost::shared_mutex aliasesLock;
			std::unordered_map<string, string> aliases;	//Protected by aliasesLock.

			class PathComponentDescription {
				public:
					string relationName, childName, childTypeName;
					bool haveChildType;
			};
			PathComponentDescription* parsePath( const string& path, size_t* outComponentCount ) throw();	//Returns a new array of outComponentCount PathComponentDescriptions or NULL if an error occured.
			bool parsePathComponent( const string& component, PathComponentDescription* outDescription) throw();	//Returns true on success.
	};

	Topology::PathComponentDescription* Topology::parsePath( const string& path, size_t* outComponentCount ) throw() {
		//First count the path components, so that we can allocate descriptions for them.
		size_t componentCount = 1, pathSize = path.size();
		if( !pathSize ) return NULL;
		for( size_t i = pathSize; i--; ) if( path[i] == '/' ) componentCount++;
		//Get some mem.
		PathComponentDescription* result = new PathComponentDescription[componentCount];
		//Parse the components.
		for( size_t i = 0, curComponent = 0; i < pathSize; i++, curComponent++ ) {
			size_t componentStart = i;
			for( ; i < pathSize && path[i] != '/'; i++ ) ;
			string componentString = path.substr( componentStart, i - componentStart );
			if( !parsePathComponent( componentString, &result[curComponent] ) ) {
				delete[] result;
				return NULL;
			}
		}
		*outComponentCount = componentCount;
		return result;
	}

	bool Topology::parsePathComponent( const string& aliasedComponent, Topology::PathComponentDescription* outDescription ) throw() {
		//Substitute possible aliases.
		const string* component = &aliasedComponent;
		aliasesLock.lock_shared();
		{
			while( component && component->size() && (*component)[0] == '@' ) {
				const string* temp = NULL;
				IGNORE_EXCEPTIONS( temp = &aliases.at( *component ); );
				component = temp;
			}
		}
		aliasesLock.unlock_shared();
		if( !component ) return false;
		//Sanity check: Count the number of colons in the component string.
		size_t colonCount = 0, componentSize = component->size();
		for( size_t i = componentSize; i--; ) {
			switch( (*component)[i] ) {
				case ':': colonCount++; break;
				case '/': return false;
			}
		}
		if( !colonCount || colonCount > 2 ) return false;
		if( colonCount == 1 ) {
			//Find the one colon and check for empty names.
			size_t colonPosition = 0;
			for( ; (*component)[colonPosition] != ':'; colonPosition++ ) ;
			if( !colonPosition || colonPosition == componentSize - 1 ) return false;
			//Fill in the descriptor.
			outDescription->relationName = component->substr( 0, colonPosition );
			outDescription->childName = component->substr( colonPosition + 1, componentSize - colonPosition - 1 );
			outDescription->haveChildType = false;
		} else {
			//Find the colon positions and check for empty names.
			size_t colon1 = 0, colon2 = 0;
			for( ; (*component)[colon1] != ':'; colon1++ ) ;
			for( colon2 = colon1 + 1; (*component)[colon2] != ':'; colon2++ ) ;
			if( !colon1 || colon1 == colon2 - 1 || colon2 == componentSize - 1 ) return false;
			//Fill in the descriptor.
			outDescription->relationName = component->substr( 0, colon1 );
			outDescription->childName = component->substr( colon1 + 1, colon2 - colon1 - 1 );
			outDescription->childTypeName = component->substr( colon2 + 1, componentSize - colon2 - 1 );
			outDescription->haveChildType = true;
		}
		return true;
	}

	TopologyObject Topology::registerObjectByPath( const string& path, TopologyObjectId parent ) throw() {
		size_t componentCount;
		if( PathComponentDescription* components = parsePath( path, &componentCount ) ) {
			TopologyObjectId resultId = parent;
			TopologyObject result;
			for( size_t i = 0; i < componentCount; i++ ) {
				PathComponentDescription* curComponent = &components[i];
				//Lookup the corresponding relation and update resultId.
				TopologyType relationType = registerType( curComponent->relationName );
				assert( relationType );
				if( !curComponent->haveChildType ) curComponent->childTypeName = curComponent->relationName;
				TopologyType childType = registerType( curComponent->childTypeName );
				assert( childType );
				TopologyObject child = registerObject( resultId, relationType.id(), curComponent->childName, childType.id() );
				if( !child ) break;
				resultId = child.id();
				if( i == componentCount - 1 ) result = child;
			}
			delete[] components;
			return result;
		}
		return TopologyObject();
	}

	TopologyObject Topology::lookupObjectByPath( const string& path, TopologyObjectId parent ) throw() {
		size_t componentCount;
		if( PathComponentDescription* components = parsePath( path, &componentCount ) ) {
			TopologyObjectId resultId = parent;
			TopologyObject result;
			for( size_t i = 0; i < componentCount; i++ ) {
				PathComponentDescription* curComponent = &components[i];
				//Lookup the corresponding relation and update resultId.
				TopologyType relationType = lookupTypeByName( curComponent->relationName );
				if( !relationType ) break;
				TopologyRelation relation = lookupRelation( resultId, relationType.id(), curComponent->childName );
				if( !relation ) break;
				resultId = relation.child();
				if( curComponent->haveChildType ) {	//Does the user want a type check?
					TopologyObject child = lookupObjectById( resultId );
					TopologyType expectedType = lookupTypeByName( curComponent->childTypeName );
					if( !child || !expectedType || child.type() != expectedType.id() ) break;
					if( i == componentCount - 1 ) result = child;
				} else if( i == componentCount - 1 ) {
					result = lookupObjectById( resultId );
				}
			}
			delete[] components;
			return result;
		}
		return TopologyObject();
	}

	bool Topology::setAlias( const string& name, const string& value ) throw() {
		//First some sanity checks.
		size_t nameSize = name.size();
		if( nameSize < 2 ) return false;
		if( name[0] != '@' ) return false;
		for( size_t i = nameSize; i--; ) if( name[i] == '/' ) return false;
		PathComponentDescription trash;
		if( !parsePathComponent( value, &trash ) ) return false;

		bool success = false;
		aliasesLock.lock();
		{
			string* oldValue = NULL;
			IGNORE_EXCEPTIONS( oldValue = &aliases.at( name ); );
			if( !oldValue || *oldValue == value ) {
				//Then the actual "work".
				aliases[name] = value;
				success = true;
			}
		}
		aliasesLock.unlock();
		return success;
	}

}

#define MONITORING_TOPOLOGY_INTERFACE "monitoring_topology"

#endif
