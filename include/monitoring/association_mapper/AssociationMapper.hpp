#ifndef __ASSOCIATION_MAPPER_H
#define __ASSOCIATION_MAPPER_H

#include <core/component/Component.hpp>
#include <monitoring/ontology/OntologyDatatypes.hpp>


namespace monitoring{

class AssociationMapper : public core::Component{
public:
	/*
	Domain: "instance"
	Key: String, Value int32_t

	Domain: "process"
	OntologyAttributeID
	Beliebiger Value_Type. bzw. given by attribute

	Domain "component"
	Key: cid + OntologyAttributeID, VALUE as value...
	*/
	virtual void set_process_attribute(ProcessID pid, Attribute * att, Value & value) = 0;
	virtual Value & lookup_process_attribute(ProcessID pid, Attribute * att) = 0;

	virtual void set_component_attribute(ComponentID cid, Attribute * att, Value & value) = 0;
	virtual Value & lookup_component_attribute(ComponentID cid, Attribute * att) = 0;

	/* This functions are used to create the RemoteInstanceID */
	virtual RemoteInstanceID & create_instance_mapping(const string & value) = 0;
	virtual const string & lookup_instance_mapping(uint32_t id) = 0;
};

}

#define MONITORING_ASSOCIATION_MAPPER_INTERFACE "monitoring_association_mapper"

#endif

// BUILD_TEST_INTERFACE monitoring/association_mapper/modules/
