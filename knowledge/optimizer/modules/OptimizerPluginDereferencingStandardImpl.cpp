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
    OntologyAttribute * lookup_attribute_by_name(const string & domain,const  string & name){
    	return ontology->lookup_attribute_by_name(domain, name);
    }

    OntologyAttribute * lookup_attribute_by_ID(OntologyAttributeID aID){
    	return ontology->lookup_attribute_by_ID(aID);
    } 

    const OntologyValue * lookup_meta_attribute(OntologyAttribute * attribute, OntologyAttribute * meta){
    	return ontology->lookup_meta_attribute(attribute, meta);
    }

	OntologyValue * lookup_process_attribute(ProcessID * pid, OntologyAttribute * att){
		return association_mapper->lookup_process_attribute(pid, att);
	}

	OntologyValue * lookup_component_attribute(ComponentID * cid, OntologyAttribute * att){
		return association_mapper->lookup_component_attribute(cid, att);
	}

	const string * lookup_instance_mapping(AssociateID id){
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
