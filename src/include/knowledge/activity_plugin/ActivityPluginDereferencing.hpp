#ifndef ACTIVITY_PLUGIN_DEREFERENCING_FACADE_H_
#define ACTIVITY_PLUGIN_DEREFERENCING_FACADE_H_

#include <core/component/Component.hpp>

#include <monitoring/ontology/OntologyDatatypes.hpp>
#include <monitoring/system_information/SystemInformationGlobalIDManager.hpp>
#include <monitoring/datatypes/Exceptions.hpp>

#include <knowledge/reasoner/AnomalyPlugin.hpp>


using namespace std;
using namespace core;
using namespace monitoring;

/*
 This class provides a facade for quering details for activities.
 */
namespace monitoring {
	class Topology;

	class ActivityPluginDereferencing : public Component {
		public:
			virtual Topology* topology() = 0;

			virtual const OntologyAttribute lookup_attribute_by_name( const string & domain, const string & name ) const throw( NotFoundError ) = 0;

			virtual const OntologyAttributeFull lookup_attribute_by_ID( const OntologyAttributeID aID ) const throw( NotFoundError ) = 0;

			virtual const OntologyValue lookup_meta_attribute( const OntologyAttribute & attribute, const OntologyAttribute & meta ) const throw( NotFoundError ) = 0;

			virtual const OntologyValue lookup_meta_attribute( const OntologyAttribute & attribute, const string & domain, const string & name ) const throw( NotFoundError ) {
				const OntologyAttribute unit = lookup_attribute_by_name( domain, name );
				return lookup_meta_attribute( attribute, unit );
			}

			virtual const OntologyValue lookup_process_attribute( const  ProcessID & pid, const  OntologyAttribute & att ) const throw( NotFoundError ) = 0;

			virtual const OntologyValue lookup_component_attribute( const ComponentID & cid, const  OntologyAttribute & att ) const throw( NotFoundError ) = 0;

			virtual const string lookup_instance_mapping( AssociateID id ) const throw( NotFoundError ) = 0;

			// Register a plugin to behave as an anomaly plugin.
			virtual void register_anomaly_plugin( knowledge::AnomalyPlugin * plugin ) = 0;

			virtual SystemInformationGlobalIDManager * get_system_information() = 0;
	};
}

#define ACTIVITY_DEREFERENCING_FACADE_INTERFACE "monitoring_activity_plugin_dereferencing"


#endif
