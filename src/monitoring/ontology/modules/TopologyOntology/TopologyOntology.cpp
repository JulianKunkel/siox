#include <string>

#include <sstream>

#include <monitoring/ontology/OntologyImplementation.hpp>
#include <monitoring/ontology/modules/TopologyOntology/TopologyOntologyOptions.hpp>
#include <monitoring/topology/Topology.hpp>

using namespace std;

using namespace core;
using namespace monitoring;

namespace monitoring {

/*
	The data of the ontology is stored in the topology data structure with the following scheme:
	There are three dataTypes: domainName, attributeName, attributeID
	An OntologyAttribute is stored as the object Ontology/<Domain>/<Name>
		The type of the Attribute is stored as the attribute "Type".
		The name of the Attribute is stored as the attribute "Name"
		The domain of the Attribute is stored as the attribute "Domain"
		The ID of the Attribute is stored as the attribute "ID".
	Alternatively, the same object is accessible using the attributeID relation and the ID: Ontology/<ID>.
	Metaattributes are registered as attributes with the appropriate data type 
*/

	class TopologyOntology: public Ontology {

			void init() {
				TopologyOntologyOptions & o = getOptions<TopologyOntologyOptions>();
				topology = GET_INSTANCE(Topology, o.topology);

				assert(topology);

				TopologyObject ontologyTopologyObject = topology->registerObjectByPath( "Module:Ontology" );
				pluginTopoObjectID = ontologyTopologyObject.id();

				TopologyType attribute = topology->registerType("OntologyAttribute");
				TopologyType metaAttribute = topology->registerType("OntologyMetaAttribute");
				metaAttributeTypeID = metaAttribute.id();

				idTypeID = topology->registerType("OntologyAttributeID").id();

				typeID = topology->registerAttribute( attribute.id(), "Type", VariableDatatype::Type::UINT32 ).id();
				nameID = topology->registerAttribute( attribute.id(), "Name", VariableDatatype::Type::STRING ).id();
				domainID = topology->registerAttribute( attribute.id(), "Domain", VariableDatatype::Type::STRING ).id();

				for(int i=0 ; i < (int) VariableDatatype::Type::INVALID; i++){
					stringstream number;
					number << i;
					metaValueID[i] = topology->registerAttribute( metaAttributeTypeID, number.str(), (VariableDatatype::Type) i).id();
				}
			}

			ComponentOptions * AvailableOptions() {
				return new TopologyOntologyOptions();
			}

			///////////////////////////////////////////////////

			const OntologyAttribute register_attribute( const string & domain, const string & name, VariableDatatype::Type storage_type ) throw( IllegalStateError ) override {
				stringstream s;
				s << "domain:" << domain << "/" << "attributeName:" << name;
				TopologyObject obj = topology->registerObjectByPath( s.str(), pluginTopoObjectID );			
				if ( ! obj ){
					throw IllegalStateError("Could not register attribute: " + s.str());
				}
				const uint32_t objID = obj.id();

				// query existing storage_type if any.
				TopologyValue existingStorageType = topology->getAttribute( objID, typeID );
				if ( existingStorageType ){
					 uint32_t currentST = existingStorageType.value().uint32();
					 if ( ((VariableDatatype::Type) currentST) != storage_type ){
					 	throw IllegalStateError("Existing storage type is not compatible in attribute " + s.str());
					 }

	 				return OntologyAttribute(obj.id(), domain, name, storage_type);
				}
				
				// store domain, name and storage type as attribute.				
				topology->setAttribute(objID, domainID, domain);
				topology->setAttribute(objID, nameID, name);
				topology->setAttribute(objID, typeID, (uint32_t) storage_type);
				
				return OntologyAttribute(obj.id(), domain, name, storage_type);
			}

			const OntologyAttribute lookup_attribute_by_name( const string & domain, const string & name ) const throw( NotFoundError ) override {
				stringstream s;
				s << "domain:" << domain << "/" << "attributeName:" << name;
				TopologyObject obj = topology->lookupObjectByPath( s.str(), pluginTopoObjectID );			
				if ( ! obj ){
					throw NotFoundError();
				}

				TopologyValue existingStorageType = topology->getAttribute( obj.id(), typeID );
				
				assert(existingStorageType);

				OntologyAttribute oa(obj.id(), domain, name, (VariableDatatype::Type) existingStorageType.value().uint32());
				return oa;
			}

			const OntologyAttribute lookup_attribute_by_ID( OntologyAttributeID aID ) const throw( NotFoundError ) override {

				TopologyObject obj = topology->lookupObjectById( aID );
				if ( ! obj ){
					throw NotFoundError();
				}
				TopologyObjectId objID = obj.id();

				TopologyValue existingStorageType = topology->getAttribute(objID, typeID);
				if (! existingStorageType){
					// may happen if the object is not properly created
					throw NotFoundError();
				}

				return OntologyAttribute(aID, 
					topology->getAttribute(objID, domainID).value().str(), 
					topology->getAttribute(objID, nameID).value().str(), 
					(VariableDatatype::Type) existingStorageType.value().uint32());
			}

			void attribute_set_meta_attribute( const OntologyAttribute & att, const OntologyAttribute & meta, const OntologyValue & value ) throw( IllegalStateError ) override {

				stringstream s;
				s << meta.aID;

				TopologyObject obj = topology->registerObject( att.aID, metaAttributeTypeID, s.str(), metaAttributeTypeID);
				if ( ! obj ){
					throw IllegalStateError("Could not register attribute: " + s.str());
				}
				const uint32_t objID = obj.id();

				// query existing value if any.
				TopologyValue existingValue = topology->getAttribute( objID, metaValueID[(int) meta.storage_type] );
				if ( existingValue ){
					throw IllegalStateError("Value has been set already");
				}
				
				// store domain, name and storage type as attribute.				
				TopologyValue tv = topology->setAttribute(objID, metaValueID[(int) meta.storage_type], value);
				if ( ! tv){
					throw IllegalStateError("Could not set the attribute");
				}
			}

			const vector<OntologyAttributeID> enumerate_meta_attributes( const OntologyAttribute & attribute ) const throw( NotFoundError ) override 
			{
				vector<OntologyAttributeID> ret;

				TopologyRelationList childs = topology->enumerateChildren( attribute.aID, metaAttributeTypeID );
				for(auto itr = childs.begin(); itr != childs.end(); itr++){
					ret.push_back( itr->child() );
				}

				return ret;
			}

			const OntologyValue lookup_meta_attribute( const OntologyAttribute & att, const OntologyAttribute & meta ) const throw( NotFoundError ) override {
				
				stringstream s;
				s << meta.aID;

				TopologyRelation tr = topology->lookupRelation( att.aID, metaAttributeTypeID, s.str());
				if ( ! tr ){
					throw NotFoundError();
				}
				const uint32_t objID = tr.child();

				// query existing value if any.
				TopologyValue existingValue = topology->getAttribute( objID, metaValueID[(int) meta.storage_type] );
				if ( ! existingValue ){
					throw NotFoundError();
				}

				return existingValue.value();				
			}

		private:
			Topology * topology = nullptr;

			TopologyObjectId pluginTopoObjectID;
			TopologyTypeId idTypeID;
			TopologyTypeId metaAttributeTypeID;

			TopologyAttributeId attributeID;

			TopologyAttributeId typeID;
			TopologyAttributeId nameID;
			TopologyAttributeId domainID;
			// based on the type of the value we store a different type
			TopologyAttributeId metaValueID[ (int) VariableDatatype::Type::INVALID];
	};

}

extern "C" {
	void * MONITORING_ONTOLOGY_INSTANCIATOR_NAME()
	{
		return new TopologyOntology();
	}
}
