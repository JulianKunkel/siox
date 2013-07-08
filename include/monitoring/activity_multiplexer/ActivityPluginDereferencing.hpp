#ifndef ACTIVITY_PLUGIN_DEREFERENCING_FACADE_H_
#define ACTIVITY_PLUGIN_DEREFERENCING_FACADE_H_

#include <monitoring/component/Component.hpp>

#include <monitoring/ontology/Ontology.hpp>
#include <monitoring/system_information/SystemInformationGlobalIDManager.hpp>
#include <monitoring/association_mapper/AssociationMapper.hpp>

using namespace std;
using namespace core;
using namespace monitoring;

class ActivityPluginDereferencing : public Component{
public:
    inline OntologyAttribute * lookup_attribute_by_name(string & domain, string & name){
    	return ontology->lookup_attribute_by_name(domain, name);
    }

    inline OntologyAttribute * lookup_attribute_by_ID(OntologyAttributeID aID){
    	return ontology->lookup_attribute_by_ID(aID);
    }

    inline const OntologyValue * lookup_meta_attribute(OntologyAttribute * attribute, OntologyAttribute * meta){
    	return ontology->lookup_meta_attribute(attribute, meta);
    }

	inline OntologyValue * lookup_process_attribute(ProcessID * pid, OntologyAttribute * att){
		return association_mapper->lookup_process_attribute(pid, att);
	}

	inline OntologyValue * lookup_component_attribute(ComponentID * cid, OntologyAttribute * att){
		return association_mapper->lookup_component_attribute(cid, att);
	}

	inline const string * lookup_instance_mapping(AssociateID id){
		return association_mapper->lookup_instance_mapping(id);
	}

	inline const SystemInformationGlobalIDManager * get_system_information(){
		assert(system_information_manager != nullptr);
		return system_information_manager;
	}

	void init(ComponentOptions * options){
		ActivityPluginDereferencingFacadeOptions * o = dynamic_cast<ActivityPluginDereferencingFacadeOptions*>(options);
		ontology = 	o->ontology.instance<Ontology>();
		system_information_manager = o->system_information_manager.instance<SystemInformationGlobalIDManager>();
		association_mapper = o->association_mapper.instance<AssociationMapper>();
		delete(options);
	}

	ComponentOptions * get_options(){
		return new ActivityPluginDereferencingFacadeOptions();
	}

	void shutdown(){}

private:
	// Loaded ontology implementation
	Ontology * ontology = nullptr;
    // Loaded system information manager implementation
    SystemInformationGlobalIDManager * system_information_manager = nullptr;
    // Loaded association mapper implementation
    AssociationMapper * association_mapper = nullptr;
}; 


#endif