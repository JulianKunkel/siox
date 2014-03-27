/**
 * Helper class for cacheTopology
 *
 * @author Roman Michel
 * @date 2014
 */
 #include <sstream>

#include <util/ExceptionHandling.hpp>
#include <monitoring/topology/TopologyImplementation.hpp>
#include <workarounds.hpp>

#include <unordered_map>
#include <map>
#include <boost/thread/shared_mutex.hpp>

using namespace std;
using namespace core;
using namespace monitoring;

struct CompareRelation {
	TopologyObjectId parent;
	TopologyTypeId relationType; 
	const string& childName;

    bool operator==(const CompareRelation &o)  const {
        return (parent == o.parent) && (relationType == o.relationType) && (childName == o.childName);
    }

    bool operator<(const CompareRelation &o)  const {
        return childName < o.childName;
        // TODO: Ist das in Ordnung so? Nur zur Sortierung?
    }
};

class cacheHelper {
	public:
		TopologyType registerType( const string& name, TopologyTypeId anId ) throw();
		TopologyType lookupTypeByName( const string& name ) throw();
		TopologyType lookupTypeById( TopologyTypeId anId ) throw();

		TopologyObject registerObject( TopologyTypeId objectType, TopologyObjectId anId ) throw();
		TopologyObject lookupObjectById(TopologyObjectId anId) throw();

		TopologyRelation registerRelation( TopologyObjectId parent, TopologyTypeId relationType, const string& childName, TopologyObjectId child ) throw();
		TopologyRelation lookupRelation( TopologyObjectId parent, TopologyTypeId relationType, const string& childName ) throw();

		TopologyAttribute registerAttribute( TopologyTypeId domain, const string& name, VariableDatatype::Type datatype, TopologyAttributeId attrId ) throw();
		TopologyAttribute lookupAttributeByName( TopologyTypeId domain, const string& name ) throw();
		TopologyAttribute lookupAttributeById( TopologyAttributeId attributeId ) throw();

		TopologyValue registerValue( TopologyObjectId object, TopologyAttributeId attribute, VariableDatatype myData ) throw();
		TopologyValue getValue( TopologyObjectId object, TopologyAttributeId attribute ) throw();

	private:
		boost::shared_mutex typesLock, objectsLock, attributesLock, valuesLock;

		unordered_map<string, TopologyType> typesByName; // typesLock
		unordered_map<int, TopologyType> typesById; // typesLock
		unordered_map<int, TopologyObject> objectsById; // objectsLock
		map<CompareRelation, TopologyRelation> relationsByStruct; //objectsLock
		unordered_map< pair<TopologyTypeId, string>, TopologyAttribute> attributesByName; // attributesLock
		unordered_map<int, TopologyAttribute> attributesById; // attributesLock
		unordered_map< pair<TopologyObjectId, TopologyAttributeId>, TopologyValue> valuesByPair; // valuesLock
};

TopologyType cacheHelper::registerType( const string& name, TopologyTypeId anId ) throw() {

	TopologyType result;

	// Check if it already there
	typesLock.lock_shared();
	IGNORE_EXCEPTIONS( result = typesByName.at( name ); );
	typesLock.unlock_shared();

	// Not there -> Create one and push it into cache
	if (!result) {
		Release<TopologyTypeImplementation> newType( new TopologyTypeImplementation( name, anId ) );
		typesLock.lock_shared();
		// Still not there?
		IGNORE_EXCEPTIONS( result = typesByName.at( name ); );
		if(!result) {
			// Hold in
			typesByName[name].setObject( newType );
			typesById[anId].setObject ( newType );
			result.setObject( newType );
		}
		typesLock.unlock_shared();
	}
	return result;
}

TopologyType cacheHelper::lookupTypeByName( const string& name ) throw() {
	
	TopologyType result;
	typesLock.lock_shared();
	IGNORE_EXCEPTIONS( result = typesByName.at( name ); );
	typesLock.unlock_shared();
	return result;
}


TopologyType cacheHelper::lookupTypeById( TopologyTypeId anId ) throw() {

	TopologyType result;
	typesLock.lock_shared();
	IGNORE_EXCEPTIONS( result = typesById.at( anId ); );
	typesLock.unlock_shared();
	return result;
}

TopologyObject cacheHelper::registerObject( TopologyTypeId objectType, TopologyObjectId anId ) throw() {

	TopologyObject result;

	// Check if it already there
	objectsLock.lock_shared();
	IGNORE_EXCEPTIONS( result = objectsById.at( anId ); );
	objectsLock.unlock_shared();

	// Not there, push
	if (!result) {
		Release<TopologyObjectImplementation> newObject( new TopologyObjectImplementation(objectType, anId) );
        
        // Still not there?
        objectsLock.lock_shared();
		IGNORE_EXCEPTIONS( result = objectsById.at( anId ); );
		if (!result) {
			result.setObject(newObject);
			objectsById[anId].setObject(newObject);
		}
		objectsLock.unlock_shared();
	}
	return result;
}

TopologyObject cacheHelper::lookupObjectById(TopologyObjectId anId) throw() {

	TopologyObject result;
	objectsLock.lock_shared();
	IGNORE_EXCEPTIONS( result = objectsById.at( anId ); );
	objectsLock.unlock_shared();
	return result;
}

TopologyRelation cacheHelper::registerRelation( TopologyObjectId parent, TopologyTypeId relationType, const string& childName, TopologyObjectId child ) throw() {
	
	struct CompareRelation tmpCompareRelation = {parent, relationType, childName};
	TopologyRelation tmpRelation;

	objectsLock.lock_shared();
	IGNORE_EXCEPTIONS( tmpRelation = relationsByStruct.at( tmpCompareRelation ); );
	objectsLock.unlock_shared();

	if (!tmpRelation) {
		Release<TopologyRelationImplementation> newRelation( new TopologyRelationImplementation( childName, parent, child, relationType ) );
        
        objectsLock.lock_shared();
        // Still not there?
        IGNORE_EXCEPTIONS( tmpRelation = relationsByStruct.at( tmpCompareRelation ); );
        if (!tmpRelation) {
			tmpRelation.setObject(newRelation);
			relationsByStruct[tmpCompareRelation].setObject(newRelation);
		}
		objectsLock.unlock_shared();
	}

	return tmpRelation;
}

TopologyRelation cacheHelper::lookupRelation( TopologyObjectId parent, TopologyTypeId relationType, const string& childName ) throw() {

	struct CompareRelation tmpCompareRelation = {parent, relationType, childName};
	TopologyRelation tmpRelation;

	objectsLock.lock_shared();
	IGNORE_EXCEPTIONS( tmpRelation = relationsByStruct.at( tmpCompareRelation ); );
	objectsLock.unlock_shared();
	return tmpRelation;
}

TopologyAttribute cacheHelper::registerAttribute( TopologyTypeId domain, const string& name, VariableDatatype::Type datatype, TopologyAttributeId attrId ) throw() {

	TopologyAttribute result;
	pair<TopologyTypeId, string> comPair (domain, name);

	attributesLock.lock_shared();
	IGNORE_EXCEPTIONS( result = attributesByName.at( comPair ); );
	attributesLock.unlock_shared();

	if (!result) {
		Release<TopologyAttributeImplementation> newAttribute( new TopologyAttributeImplementation( name, domain, datatype ) );
        newAttribute->id = attrId;

		// Still not there?
		attributesLock.lock_shared();
		IGNORE_EXCEPTIONS( result = attributesByName.at( comPair ); ); 

		if (!result) {
			attributesByName[comPair].setObject(newAttribute);
			attributesById[attrId].setObject(newAttribute);
	        result.setObject(newAttribute);
    	}
	    attributesLock.unlock_shared();    	
	}

	return result;
}

TopologyAttribute cacheHelper::lookupAttributeByName( TopologyTypeId domain, const string& name ) throw() {

	TopologyAttribute result;
	pair<TopologyTypeId, string> comPairs (domain, name);

	attributesLock.lock_shared();
	IGNORE_EXCEPTIONS( result = attributesByName.at( comPairs ); );
	attributesLock.unlock_shared();
	return result;
}

TopologyAttribute cacheHelper::lookupAttributeById( TopologyAttributeId attributeId ) throw() {

	TopologyAttribute result;
	attributesLock.lock_shared();
	IGNORE_EXCEPTIONS( result = attributesById.at( attributeId ); );
	attributesLock.unlock_shared();
	return result;
}

// Hint: Also updates a value if already there
TopologyValue cacheHelper::registerValue( TopologyObjectId object, TopologyAttributeId attribute, VariableDatatype myData ) throw() {

	TopologyValue result;
	pair<TopologyObjectId, TopologyAttributeId> comPairs (object, attribute);

	Release<TopologyValueImplementation> newValue( new TopologyValueImplementation( myData, object, attribute ) );
	valuesByPair[comPairs].setObject(newValue);
	result.setObject(newValue);

	return result;
}

TopologyValue cacheHelper::getValue( TopologyObjectId object, TopologyAttributeId attribute ) throw() {

	TopologyValue result;
	pair<TopologyObjectId, TopologyAttributeId> comPairs (object, attribute);

	valuesLock.lock_shared();
	IGNORE_EXCEPTIONS( result = valuesByPair.at( comPairs ); );
	valuesLock.unlock_shared();
	return result;
}