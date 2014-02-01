#ifndef SIOX_ONT_H
#define SIOX_ONT_H

#include <string>
#include <vector>

#include <core/component/Component.hpp>

#include <monitoring/ontology/OntologyDatatypes.hpp>
#include <monitoring/datatypes/Exceptions.hpp>

/* CLASS DIAGRAM OBSOLETE!!!
@ startuml Ontology.png

'set namespaceSeparator ::

package boost #DDDDDD
'    class variant {
'    }
end package


namespace core #SkyBlue

    class Component{
    }

end namespace


namespace std #SkyBlue

    class Value {
    }
    boost.variant <|-- Value

    class StorageType {
    }

    class OntologyAttribute {
        +string name
        +StorageType storage_type

        +OntologyAttribute( name, storage_type )
    }
    OntologyAttribute o-- StorageType

    class Metric {
        +string canonical_name
        +StorageType storage_type
        +string unit
        +List<OntologyAttribute> attributes

        +Metric( name, unit, storage_type )
    }
    Metric o-- StorageType
    Metric *-- OntologyAttribute

end namespace


namespace monitoring #Orange

    abstract class Ontology {
        -List<Metric> metrics
        -List<OntologyAttribute> attributes
        +OntologyAttribute attribute_register( name, storage_type ) {abstract}
        +Metric register_metric( canonical_name, unit, storage ) {abstract}
        +Metric find_metric_by_name( canonical_name ) {abstract}
        +void metric_set_attribute( metric, attribute, value ) {abstract}
    }
    core.Component <|-- Ontology
    Ontology *-- std.Metric
    Ontology *-- std.OntologyAttribute

    class FileOntology {
    }
    Ontology <|-- FileOntology
end namespace

@ enduml
*/

using namespace std;

namespace monitoring {

	class Ontology : public core::Component {

		public:
			/// Retrieve an attribute's data object from the ontology.
			/// If necessary, a new entry will be created.
			virtual const OntologyAttribute register_attribute( const string & domain, const string & name, VariableDatatype::Type storage_type ) throw( IllegalStateError ) = 0;

			/// Set a (meta) attribute applying to another attribute.
			/// Examples are an attribute's status as descriptor or a unit for the value
			/// of the (base) attribute.
			/// In the latter case, the meta attribute "[unit name]" of the domain "unit"
			/// and the storage type SIOX_STORAGE_STRING would first be registered
			virtual void attribute_set_meta_attribute( const OntologyAttribute & att, const OntologyAttribute & meta, const OntologyValue & value ) throw( IllegalStateError ) = 0;

			virtual const OntologyAttribute lookup_attribute_by_name( const string & domain, const string & name ) const throw( NotFoundError ) = 0;

			virtual const OntologyAttributeFull lookup_attribute_by_ID( const OntologyAttributeID aID ) const throw( NotFoundError ) = 0;

			virtual const vector<OntologyAttributeID> enumerate_meta_attributes( const OntologyAttribute & attribute ) const = 0;

			virtual const OntologyValue lookup_meta_attribute( const OntologyAttribute & attribute, const OntologyAttribute & meta ) const throw( NotFoundError ) = 0;


			// From the system information table:
			// the hardware ID -> is system specific
			// the device ID -> system specific
			// the filesystem ID -> system specific

			// => SystemInformationGlobalIDManager
			// the interface ID -> may exist across systems, global
			// register_activity ( UniqueInterface ID, Activity Name) -> may exist across systems, global
			// virtual UniqueInterfaceID           lookup_interface_id(string & interface, string & implementation);
			// virtual UniqueComponentActivityID   lookup_activity_id(UniqueInterfaceID & id, string & name);
			// virtual const string & lookup_interface_name(UniqueInterfaceID id);
			// virtual const string & lookup_interface_implementation(UniqueInterfaceID id);
			// virtual const string & lookup_activity_name(UniqueComponentActivityID id);


			// All runtime-specific information is stored by the association mapper:
			// ProcessID
			// AssociateID
			// ComponentID
			// process_set_attribute, component_set_attribute => association mapper

			// activity_set_attribute => part of the activity builder and transformed to the Activity
			// ALL activity calls including remote calls follow this rule.
	};

}


#define ONTOLOGY_INTERFACE "monitoring_ontology"

#endif

// BUILD_TEST_INTERFACE monitoring/ontology/modules/Ontology
