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
	virtual bool set_process_attribute(ProcessID * pid, OntologyAttribute * att, const OntologyValue & value) = 0;
	virtual OntologyValue * lookup_process_attribute(ProcessID * pid, OntologyAttribute * att) = 0;

	virtual bool set_component_attribute(ComponentID * cid, OntologyAttribute * att, const  OntologyValue & value) = 0;
	virtual OntologyValue * lookup_component_attribute(ComponentID * cid, OntologyAttribute * att) = 0;

	/* These functions are used to create the RemoteInstanceID */
	virtual AssociateID create_instance_mapping(const string & value) = 0;
	virtual const string * lookup_instance_mapping(AssociateID id) = 0;
};

}

#define MONITORING_ASSOCIATION_MAPPER_INTERFACE "monitoring_association_mapper"

#endif

// BUILD_TEST_INTERFACE monitoring/association_mapper/modules/
