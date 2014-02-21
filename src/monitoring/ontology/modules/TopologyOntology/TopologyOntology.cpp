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

				TopologyObject ontologyTopologyObject = topology->registerObjectByPath( {{"ModuleName", "Ontology", "Module"}} );
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
				TopologyObject obj = topology->registerObjectByPath( {{"DomainName", domain, "Domain" }, {"AttributeName", name, "Attribute"} }, pluginTopoObjectID );
				if ( ! obj ){
					throw IllegalStateError("Could not register attribute: " + domain +"/" + name);
				}
				const uint32_t objID = obj.id();

				// query existing storage_type if any.
				TopologyValue existingStorageType = topology->getAttribute( objID, typeID );
				if ( existingStorageType ){
					 uint32_t currentST = existingStorageType.value().uint32();
					 if ( ((VariableDatatype::Type) currentST) != storage_type ){
					 	throw IllegalStateError("Existing storage type is not compatible in attribute " + domain + "/" + name);
					 }

	 				return OntologyAttribute(obj.id(), storage_type);
				}
				
				// store domain, name and storage type as attribute.				
				topology->setAttribute(objID, domainID, domain);
				topology->setAttribute(objID, nameID, name);
				topology->setAttribute(objID, typeID, (uint32_t) storage_type);
				
				return OntologyAttribute(obj.id(), storage_type);
			}

			const OntologyAttribute lookup_attribute_by_name( const string & domain, const string & name ) const throw( NotFoundError ) override {
				TopologyObject obj = topology->lookupObjectByPath( {{"DomainName", domain }, {"AttributeName", name}}, pluginTopoObjectID );
				if ( ! obj ){
					throw NotFoundError();
				}

				TopologyValue existingStorageType = topology->getAttribute( obj.id(), typeID );
				
				assert(existingStorageType);

				OntologyAttribute oa(obj.id(), (VariableDatatype::Type) existingStorageType.value().uint32());
				return oa;
			}

			const OntologyAttributeFull lookup_attribute_by_ID( OntologyAttributeID aID ) const throw( NotFoundError ) override {
				TopologyObjectId objID = aID;

				TopologyValue existingStorageType = topology->getAttribute(objID, typeID);
				if (! existingStorageType){
					// may happen if the object is not properly created
					throw NotFoundError();
				}

				return OntologyAttributeFull(aID, 
					topology->getAttribute(objID, domainID).value().str(), 
					topology->getAttribute(objID, nameID).value().str(), 
					(VariableDatatype::Type) existingStorageType.value().uint32());
			}

			void attribute_set_meta_attribute( const OntologyAttribute & att, const OntologyAttribute & meta, const OntologyValue & value ) throw( IllegalStateError ) override {

				// query existing value if any.
				TopologyValue existingValue = topology->getAttribute( att.aID, meta.aID );
				if ( existingValue ){
					if ( existingValue.value() == value ){
						// it is valid to set the identical value again
						return;
					}
					throw IllegalStateError("Meta attribute has been set already to an incompatible value");
				}
				
				// store domain, name and storage type as attribute.
				if ( ! topology->setAttribute(att.aID, meta.aID, value) ){
					throw IllegalStateError("Could not set the attribute");
				}
			}

			const vector<OntologyAttributeID> enumerate_meta_attributes( const OntologyAttribute & attribute ) const throw( NotFoundError ) override 
			{
				vector<OntologyAttributeID> ret;

				TopologyValueList childs = topology->enumerateAttributes( attribute.aID );
				for(auto itr = childs.begin(); itr != childs.end(); itr++){
					OntologyAttributeID attribute = itr->attribute();
					if(attribute != domainID && attribute != nameID  && attribute != typeID ){
						ret.push_back( attribute );
					}
				}

				return ret;
			}

			const OntologyValue lookup_meta_attribute( const OntologyAttribute & att, const OntologyAttribute & meta ) const throw( NotFoundError ) override {
				
				// query existing value if any.
				TopologyValue existingValue = topology->getAttribute( att.aID, meta.aID );
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
