/**
 * datatypes for the topology interface
 *
 * @date 2013
 * @author Nathanael Hübbe
 */

#ifndef INCLUDE_GUARD_MONITORING_DATATYPES_TOPOLOGY_HPP
#define INCLUDE_GUARD_MONITORING_DATATYPES_TOPOLOGY_HPP

#include <util/referenceCounting.hpp>
#include <core/datatypes/VariableDatatype.hpp>

#include <string>

namespace monitoring {
	typedef VariableDatatype TopologyVariable;

	typedef uint32_t TopologyTypeId;
	typedef uint32_t TopologyObjectId;
	typedef uint32_t TopologyRelationId;
	typedef uint32_t TopologyValueId;
	typedef uint32_t TopologyAttributeId;

	//The following classes are for internal use of the Topology implementations only,
	//Topology users only have to use the classes in the block below.
	class TopologyTypeImplementation : public ReferencedObject {
		public:
			TopologyTypeImplementation( const string& name ) : name(name), id(0) {}
			TopologyTypeImplementation( const string& name, TopologyTypeId id ) : name(name), id(id) {}
			string name;
			TopologyTypeId id;
	};

	class TopologyObjectImplementation : public ReferencedObject {
		public:
			TopologyObjectImplementation( TopologyTypeId type ) : id(0), typeId(type) {}
			TopologyObjectImplementation( TopologyTypeId type, TopologyObjectId id ) : id(id), typeId(type) {}

			TopologyObjectId id;
			TopologyTypeId typeId;
	};

	class TopologyRelationImplementation : public ReferencedObject {
		public:
			TopologyRelationImplementation(string childName, TopologyObjectId parentId, TopologyObjectId childId, TopologyTypeId typeId) : childName(childName), parentId(parentId), childId(childId), typeId(typeId) {}
			string childName;
			TopologyObjectId parentId;
			TopologyObjectId childId;
			TopologyTypeId typeId;
	};

	class TopologyAttributeImplementation : public ReferencedObject {
		public:
			TopologyAttributeImplementation( const string& name, TopologyTypeId domainId, VariableDatatype::Type dataType) : name(name), id(0), domainId(domainId), dataType(dataType) {}
			string name;
			TopologyAttributeId id;
			TopologyTypeId domainId;
			VariableDatatype::Type dataType;
	};

	class TopologyValueImplementation : public ReferencedObject {
		public:
			TopologyValueImplementation( const TopologyVariable & value, TopologyObjectId object, TopologyAttributeId attribute) : value(value), object(object), attribute(attribute) {}
			TopologyVariable value;
			TopologyObjectId object;
			TopologyAttributeId attribute;
	};

	//The classes relevant to the users of a Topology.
	class TopologyType : public Retain<const TopologyTypeImplementation> {
		public:
			const string& name() const { return itsObject->name; }
			TopologyTypeId id() const { return itsObject->id; }	//SQL key
	};

	class TopologyObject : public Retain<const TopologyObjectImplementation> {
		public:
			TopologyObjectId id() const { return itsObject->id; }	//SQL key
			TopologyTypeId type() const { return itsObject->typeId; }
	};

	class TopologyRelation : public Retain<const TopologyRelationImplementation> {
		public:
			//The combination of childName and parentId is unique and used as the SQL key.
			const string& childName() const { return itsObject->childName; }	//SQL key
			TopologyObjectId parent() const { return itsObject->parentId; }	//SQL key
			TopologyObjectId child() const { return itsObject->childId; }
			TopologyTypeId type() const { return itsObject->typeId; }
	};

	class TopologyAttribute : public Retain<const TopologyAttributeImplementation> {
		public:
			string name() const { return itsObject->name; }
			TopologyAttributeId id() const { return itsObject->id; }	//SQL key
			TopologyTypeId domainId() const { return itsObject->domainId; }	//XXX Maybe, we should change this to a string.
			VariableDatatype::Type dataType() const { return itsObject->dataType; }
	};

	class TopologyValue : public Retain<const TopologyValueImplementation> {
		public:
			//The combination of object and attribute is unique and used as the SQL key.
			TopologyVariable value() const { return itsObject->value; }
			TopologyObjectId object() const { return itsObject->object; }	//SQL key
			TopologyAttributeId attribute() const { return itsObject->attribute; }	//SQL key
	};
}

#endif
