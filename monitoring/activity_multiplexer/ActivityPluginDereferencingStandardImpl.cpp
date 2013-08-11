#include <monitoring/activity_multiplexer/ActivityPluginDereferencingImplementation.hpp>

#include <monitoring/ontology/Ontology.hpp>
#include <monitoring/association_mapper/AssociationMapper.hpp>

#include <core/component/ComponentReferenceSerializable.hpp>

#include "ActivityPluginDereferencingOptions.hpp"

using namespace std;
using namespace core;
using namespace monitoring;

CREATE_SERIALIZEABLE_CLS(ActivityPluginDereferencingFacadeOptions)

class ActivityPluginDereferencingImplementation : public ActivityPluginDereferencing{
public:
	
	virtual const OntologyAttribute & lookup_attribute_by_name(const string & domain, const string & name) const throw(NotFoundError){
		return ontology->lookup_attribute_by_name(domain, name);
	}

    virtual const OntologyAttribute & lookup_attribute_by_ID(const OntologyAttributeID aID) const throw(NotFoundError){
    	return ontology->lookup_attribute_by_ID(aID);
    }

    virtual const OntologyValue & lookup_meta_attribute(const OntologyAttribute & attribute, const OntologyAttribute & meta) const throw(NotFoundError){
    	return ontology->lookup_meta_attribute(attribute, meta);
    }  	

	const OntologyValue & lookup_process_attribute(const  ProcessID & pid, const  OntologyAttribute & att) const throw(NotFoundError){
		return association_mapper->lookup_process_attribute(pid, att);
	}

	const OntologyValue & lookup_component_attribute(const ComponentID & cid, const  OntologyAttribute & att) const throw(NotFoundError){
		return association_mapper->lookup_component_attribute(cid, att);
	}

	const string & lookup_instance_mapping(AssociateID id) const throw(NotFoundError){
		return association_mapper->lookup_instance_mapping(id);
	}

	SystemInformationGlobalIDManager * get_system_information(){
		assert(system_information_manager != nullptr);
		return system_information_manager;
	}

	void init(Ontology * o, SystemInformationGlobalIDManager * id, AssociationMapper * as){
		ontology = o;
		system_information_manager = id;
		association_mapper = as;
	}

	void init(){
		ActivityPluginDereferencingFacadeOptions & o = getOptions<ActivityPluginDereferencingFacadeOptions>();
		ontology = 	o.ontology.instance<Ontology>();
		system_information_manager = o.system_information_manager.instance<SystemInformationGlobalIDManager>();
		association_mapper = o.association_mapper.instance<AssociationMapper>();
	}

	ComponentOptions * AvailableOptions(){
		return new ActivityPluginDereferencingFacadeOptions();
	}

private:
	// Loaded ontology implementation
	Ontology * ontology = nullptr;
    // Loaded system information manager implementation
    SystemInformationGlobalIDManager * system_information_manager = nullptr;
    // Loaded association mapper implementation
    AssociationMapper * association_mapper = nullptr;
}; 

PLUGIN(ActivityPluginDereferencingImplementation)
