#ifndef SIOX_ONT_H
#define SIOX_ONT_H

#include <string>
#include <vector>

#include <core/component/Component.hpp>

#include <monitoring/ontology/OntologyDatatypes.hpp>

/* CLASS DIAGRAM OBSOLETE!!!
@startuml Ontology.png

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

 
@enduml
*/

using namespace std;

namespace monitoring{

class Ontology : public core::Component {

public: 
    virtual OntologyAttribute * register_attribute(string & domain, string & name, enum siox_ont_storage_type storage_type) = 0;

    virtual bool attribute_set_meta_attribute(OntologyAttribute * att, OntologyAttribute * meta, OntologyValue & value) = 0;

    virtual OntologyAttribute * lookup_attribute_by_name(string & domain, string & name) = 0;

    virtual OntologyAttribute * lookup_attribute_by_ID(OntologyAttributeID aID) = 0;

    virtual const vector<OntologyAttribute*> & enumerate_meta_attributes(OntologyAttribute * attribute) = 0;

    virtual const OntologyValue * lookup_meta_attribute(OntologyAttribute * attribute, OntologyAttribute * meta) = 0;

    // the interface ID -> may exist across systems, global
    // register_activity ( UniqueInterface ID, Activity Name) -> may exist across systems, global

    // From the system information table:
    // the hardware ID -> is system specific
    // the device ID -> system specific
    // the filesystem ID -> system specific

    // => SystemInformationGlobalIDManager
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
