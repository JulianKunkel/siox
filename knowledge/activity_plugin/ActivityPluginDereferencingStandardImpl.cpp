#include <knowledge/activity_plugin/ActivityPluginDereferencingImplementation.hpp>

#include <monitoring/ontology/Ontology.hpp>
#include <monitoring/association_mapper/AssociationMapper.hpp>

#include <core/component/ComponentReferenceSerializable.hpp>

#include <knowledge/reasoner/Reasoner.hpp>

#include "ActivityPluginDereferencingOptions.hpp"

using namespace std;
using namespace core;
using namespace monitoring;
using namespace knowledge;

CREATE_SERIALIZEABLE_CLS( ActivityPluginDereferencingFacadeOptions )

class ActivityPluginDereferencingImplementation : public ActivityPluginDereferencing {
	public:

		virtual const OntologyAttribute & lookup_attribute_by_name( const string & domain, const string & name ) const throw( NotFoundError ) {
			return ontology->lookup_attribute_by_name( domain, name );
		}

		virtual const OntologyAttribute & lookup_attribute_by_ID( const OntologyAttributeID aID ) const throw( NotFoundError ) {
			return ontology->lookup_attribute_by_ID( aID );
		}

		virtual const OntologyValue & lookup_meta_attribute( const OntologyAttribute & attribute, const OntologyAttribute & meta ) const throw( NotFoundError ) {
			return ontology->lookup_meta_attribute( attribute, meta );
		}

		const OntologyValue & lookup_process_attribute( const  ProcessID & pid, const  OntologyAttribute & att ) const throw( NotFoundError ) {
			return association_mapper->lookup_process_attribute( pid, att );
		}

		const OntologyValue & lookup_component_attribute( const ComponentID & cid, const  OntologyAttribute & att ) const throw( NotFoundError ) {
			return association_mapper->lookup_component_attribute( cid, att );
		}

		const string & lookup_instance_mapping( AssociateID id ) const throw( NotFoundError ) {
			return association_mapper->lookup_instance_mapping( id );
		}

		void registerAnomalyPlugin( knowledge::AnomalyPlugin * plugin ) {
			assert( reasoner != nullptr );
			reasoner->connectAnomalyPlugin( plugin );
		}


		SystemInformationGlobalIDManager * get_system_information() {
			assert( system_information_manager != nullptr );
			return system_information_manager;
		}

		void init( Ontology * o, SystemInformationGlobalIDManager * id, AssociationMapper * as ) {
			ontology = o;
			system_information_manager = id;
			association_mapper = as;
		}

		void init() {
			ActivityPluginDereferencingFacadeOptions & o = getOptions<ActivityPluginDereferencingFacadeOptions>();
			ontology =  GET_INSTANCE(Ontology, o.ontology);
			system_information_manager = GET_INSTANCE(SystemInformationGlobalIDManager, o.system_information_manager);
			association_mapper = GET_INSTANCE(AssociationMapper, o.association_mapper);
			reasoner =  GET_INSTANCE(Reasoner, o.reasoner);
		}

		ComponentOptions * AvailableOptions() {
			return new ActivityPluginDereferencingFacadeOptions();
		}

	private:
		// Loaded ontology implementation
		Ontology * ontology = nullptr;
		// Loaded system information manager implementation
		SystemInformationGlobalIDManager * system_information_manager = nullptr;
		// Loaded association mapper implementation
		AssociationMapper * association_mapper = nullptr;

		Reasoner * reasoner = nullptr;
};

extern "C"{
	void * MONITORING_ACTIVITY_PLUGIN_DEREFERENCING_INSTANCIATOR_NAME() {
		return new ActivityPluginDereferencingImplementation();
	}
}
