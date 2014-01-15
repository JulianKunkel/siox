#include <string>
#include <sstream>

#include <monitoring/association_mapper/AssociationMapperImplementation.hpp>
#include <monitoring/association_mapper/modules/TopologyAssociationMapper/TopologyAssociationMapperOptions.hpp>
#include <monitoring/topology/Topology.hpp>

using namespace std;

using namespace core;
using namespace monitoring;

namespace monitoring {


class TopologyAssociationMapper: public AssociationMapper {
	Topology * topology = nullptr;
	TopologyAttributeId metaValueID[ (int) VariableDatatype::Type::INVALID];
	TopologyTypeId metaAttributeTypeID;
	TopologyObjectId pluginTopoObjectID;

	TopologyTypeId processID;
	TopologyTypeId componentID;

	TopologyAttributeId attrStringValue;

	public:
	
	void init() override {
		TopologyAssociationMapperOptions & o = getOptions<TopologyAssociationMapperOptions>();
		topology = GET_INSTANCE(Topology, o.topology);
		assert(topology);

		TopologyObject asObject = topology->registerObjectByPath( "Module:AssociationMapper" );
		pluginTopoObjectID = asObject.id();

		processID = topology->registerType("ProcessID").id();
		componentID = topology->registerType("ComponentID").id();

		TopologyType metaAttribute = topology->registerType("AssociationMapperMetaAttribute");
		metaAttributeTypeID = metaAttribute.id();

		for(int i=0 ; i < (int) VariableDatatype::Type::INVALID; i++){
			stringstream number;
			number << i;
			metaValueID[i] = topology->registerAttribute( metaAttributeTypeID, number.str(), (VariableDatatype::Type) i).id();
		}

		attrStringValue = topology->registerAttribute( metaAttributeTypeID, "String", VariableDatatype::Type::STRING ).id();
	}

	ComponentOptions * AvailableOptions() override {
		return new TopologyAssociationMapperOptions();
	}


	void set_process_attribute( const ProcessID & pid, const OntologyAttribute & att, const OntologyValue & value ) throw( IllegalStateError ) override {
		stringstream s;
		s << "ProcessID:" << pid << "/OntologyAttributeID:" << att.aID;

		TopologyObject obj = topology->registerObjectByPath( s.str(), pluginTopoObjectID );
		if ( ! obj ){
			throw IllegalStateError("Could not register attribute: " + s.str());
		}
		const uint32_t objID = obj.id();

		// query existing value if any.
		TopologyValue existingValue = topology->getAttribute( objID, metaValueID[(int) att.storage_type] );
		if ( existingValue ){
			// it is valid to set the identical value
			if ( value == existingValue.value() ){
				return;
			}
			throw IllegalStateError("Value has been set already");
		}
		
		// store value into the appriate datatype
		TopologyValue tv = topology->setAttribute(objID, metaValueID[(int) att.storage_type], value);
		if ( ! tv){
			throw IllegalStateError("Could not set the attribute");
		}
	}

	const OntologyValue lookup_process_attribute( const ProcessID & pid, const OntologyAttribute & att ) const throw( NotFoundError ) override {
		stringstream s;
		s << "ProcessID:" << pid << "/OntologyAttributeID:" << att.aID;

		TopologyObject obj = topology->lookupObjectByPath(s.str(), pluginTopoObjectID);
		if ( ! obj ){
			throw NotFoundError();
		}
		const uint32_t objID = obj.id();

		// query existing value if any.
		TopologyValue existingValue = topology->getAttribute( objID, metaValueID[(int) att.storage_type] );
		if ( ! existingValue ){
			throw NotFoundError();
		}

		return existingValue.value();		
	}

	void set_component_attribute( const ComponentID & cid, const OntologyAttribute & att, const  OntologyValue & value ) throw( IllegalStateError ) override {
		stringstream s;
		s << "ComponentID:" << cid << "/OntologyAttributeID:" << att.aID;

		TopologyObject obj = topology->registerObjectByPath( s.str(), pluginTopoObjectID );
		if ( ! obj ){
			throw IllegalStateError("Could not register attribute: " + s.str());
		}
		const uint32_t objID = obj.id();

		// query existing value if any.
		TopologyValue existingValue = topology->getAttribute( objID, metaValueID[(int) att.storage_type] );
		if ( existingValue ){
			// it is valid to set the identical value
			if ( value == existingValue.value() ){
				return;
			}
			throw IllegalStateError("Value has been set already");
		}
		
		// store value into the appriate datatype
		TopologyValue tv = topology->setAttribute(objID, metaValueID[(int) att.storage_type], value);
		if ( ! tv){
			throw IllegalStateError("Could not set the attribute");
		}
	}

	const OntologyValue lookup_component_attribute( const ComponentID & cid, const OntologyAttribute & att ) const throw( NotFoundError ) override {
		stringstream s;
		s << "ComponentID:" << cid << "/OntologyAttributeID:" << att.aID;

		TopologyObject obj = topology->lookupObjectByPath(s.str(), pluginTopoObjectID);
		if ( ! obj ){
			throw NotFoundError();
		}
		const uint32_t objID = obj.id();

		// query existing value if any.
		TopologyValue existingValue = topology->getAttribute( objID, metaValueID[(int) att.storage_type] );
		if ( ! existingValue ){
			throw NotFoundError();
		}

		return existingValue.value();				
	}


	/* These functions are used to create the RemoteInstanceID */
	AssociateID create_instance_mapping( const string & value ) {
		assert( value.size() > 0 );

		TopologyType obj = topology->registerType(value);
		if ( ! obj ){
			throw IllegalStateError("Could not register instance mapping: " + value);
		}
		return obj.id();
	}

	const string lookup_instance_mapping( AssociateID id ) const throw( NotFoundError ) override {
		TopologyType obj = topology->lookupTypeById( id );
		if (! obj){
			throw NotFoundError();
		}
		return obj.name();
	}

};

}



extern "C" {
	void * MONITORING_ASSOCIATION_MAPPER_INSTANCIATOR_NAME()
	{
		return new TopologyAssociationMapper();
	}
}
