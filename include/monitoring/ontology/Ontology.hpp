#ifndef SIOX_ONT_H
#define SIOX_ONT_H

#include <string>
#include <memory>
#include <vector>

#include <boost/variant.hpp>
//#include <boost/shared_ptr.hpp>

#include <core/component/Component.hpp>

#include <monitoring/datatypes/basic-types.hpp>

/*
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
    
    class Attribute {
        +string name
        +StorageType storage_type

        +Attribute( name, storage_type )
    }
    Attribute o-- StorageType

    class Metric {
        +string canonical_name
        +StorageType storage_type
        +string unit
        +List<Attribute> attributes

        +Metric( name, unit, storage_type )
    }
    Metric o-- StorageType
    Metric *-- Attribute

end namespace


namespace monitoring #Orange

    abstract class Ontology {
        -List<Metric> metrics
        -List<Attribute> attributes
        +Attribute attribute_register( name, storage_type ) {abstract}
        +Metric register_metric( canonical_name, unit, storage ) {abstract}
        +Metric find_metric_by_name( canonical_name ) {abstract}
        +void metric_set_attribute( metric, attribute, value ) {abstract}
    }
    core.Component <|-- Ontology
    Ontology *-- std.Metric
    Ontology *-- std.Attribute

    class FileOntology {
    }
    Ontology <|-- FileOntology
end namespace


@enduml
*/

using namespace std;

namespace monitoring{

namespace ontology{

typedef boost::variant<int64_t, uint64_t, int32_t, uint32_t, std::string, float, double> Value;

class Attribute {
public:
    OntologyAttributeID aID;

    string name;
    enum siox_ont_storage_type storage_type;

    Attribute(OntologyAttributeID aid, string & name, enum siox_ont_storage_type storage_type) : aID(aid), name(name), storage_type(storage_type) {}
};

class OntologyProvider : public core::Component {

public: 
	virtual Attribute * register_attribute(string & domain, string & name, enum siox_ont_storage_type storage_type) = 0 ;

    virtual bool attribute_set_meta_attribute(Attribute * att, Attribute * meta, Value & value);

	virtual Attribute * lookup_attribute_by_name(string & domain, string & name) = 0;

    virtual Attribute * lookup_attribute_by_ID(OntologyAttributeID aID) = 0;

    virtual const vector<Attribute> & lookup_meta_attributes(Attribute * attribute);

    virtual const Value & lookup_meta_attribute(Attribute * attribute, Attribute * meta) = 0;

    // the interface ID -> may exist across systems, global
    // register_activity ( UniqueInterface ID, Activity Name) -> may exist across systems, global

    // From the system information table:
    // the hardware ID -> is system specific
    // the device ID -> system specific
    // the filesystem ID -> system specific

    // Global Association Store
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
}





// Module interfaces 

#define COMPONENT(x) \
extern "C"{\
void * get_instance_monitoring_ontology() { return new x(); }\
}

#define ONTOLOGY_INTERFACE "monitoring_ontology"



#endif
