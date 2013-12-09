/**
 * A Topology implementation that keeps all data in main memory.
 *
 * @author Nathanael Hübbe
 * @date 2013
 */

#include <util/ExceptionHandling.hpp>
#include <monitoring/topology/TopologyImplementation.hpp>
#include <workarounds.hpp>

#include <unordered_map>
#include <boost/thread/shared_mutex.hpp>

#include "RamTopologyOptions.hpp"

using namespace std;
using namespace core;
using namespace monitoring;


class RamTopology : public Topology {
	public:
		virtual void init();
		virtual ComponentOptions * AvailableOptions();

		virtual TopologyType registerType( const string& name ) throw();
		virtual TopologyType lookupTypeByName( const string& name ) throw( NotFoundError );
		virtual TopologyType lookupTypeById( TopologyTypeId anId ) throw( NotFoundError );

		virtual const TopologyObject& registerObject( TopologyObjectId parent, TopologyTypeId objectType, TopologyTypeId relationType, const string& childName ) throw( IllegalStateError );
		virtual const TopologyObject& lookupObjectByPath( const string& Path ) throw( NotFoundError );
		virtual const TopologyObject& lookupObjectById( TopologyObject anId ) throw( NotFoundError );	//Passing 0 yields a reference to the root object.

		virtual const TopologyRelation& registerRelation( TopologyTypeId relationType, TopologyObjectId parent, TopologyObjectId child, const string& childName ) throw( IllegalStateError );
		virtual const TopologyRelation& lookupRelation( TopologyObjectId parent, const string& childName ) throw( NotFoundError );

		// TopologyRelationType may be 0 which indicates any parent/child.
		virtual TopologyRelationList enumerateChildren( TopologyObjectId parent, TopologyTypeId relationType ) throw();
		virtual TopologyRelationList enumerateParents( TopologyObjectId child, TopologyTypeId relationType ) throw();


		virtual const TopologyAttribute registerAttribute( TopologyTypeId domain, const string& name, VariableDatatype::Type datatype ) throw( IllegalStateError );
		virtual const TopologyAttribute lookupAttributeByName( TopologyTypeId domain, const string& name ) throw( NotFoundError );
		virtual const TopologyAttribute lookupAttributeById( TopologyAttributeId attributeId ) throw( NotFoundError );
		virtual void setAttribute( TopologyObjectId object, TopologyAttributeId attribute, const TopologyValue& value ) throw( IllegalStateError );
		virtual const TopologyValue& getAttribute( TopologyObjectId object, TopologyAttributeId attribute ) throw( NotFoundError );
		virtual TopologyValueList enumerateAttributes( TopologyObjectId object ) throw();

	private:
		boost::shared_mutex typesLock, objectsLock, relationsLock, valuesLock, attributesLock;

		unordered_map<string, TopologyType> typesByName;	//Protected by typesLock.
		vector<TopologyType> typesById;	//Protected by typesLock. The IDs are 1 based, the index into this vector is not!

		unordered_map<pair<TopologyTypeId, string>, TopologyAttribute> attributesByName;	//Protected by attributesLock.
		vector<TopologyAttribute> attributesById;	//Protected by attributesLock.
};


void RamTopology::init() {
}


ComponentOptions* RamTopology::AvailableOptions() {
	return new RamTopologyOptions;
}


TopologyType RamTopology::registerType( const string& name ) throw() {
	TopologyType result;
	typesLock.lock_shared();
	IGNORE_EXCEPTIONS( result = typesByName.at( name ); );
	typesLock.unlock_shared();
	if( !result ) {
		Release<TopologyTypeImplementation> newType( new TopologyTypeImplementation( name ) );	//The new operator can take a _lot_ of time, don't let other processes wait for that!

		typesLock.lock();
		IGNORE_EXCEPTIONS( result = typesByName.at( name ); );	//Someone else might have registered it in the mean time.
		if( !result ) {
			typesById.resize( newType->id = typesById.size() + 1 );
			typesById.back().setObject( newType );
			typesByName[name].setObject( newType );
			result.setObject( newType );
		}
		typesLock.unlock();
	}
	return result;
}


TopologyType RamTopology::lookupTypeByName( const string& name ) throw( NotFoundError ) {
	TopologyType result;
	typesLock.lock_shared();
	IGNORE_EXCEPTIONS( result = typesByName.at( name ); );
	typesLock.unlock_shared();
	if( !result ) throw NotFoundError();
	return result;
}


TopologyType RamTopology::lookupTypeById( TopologyTypeId anId ) throw( NotFoundError ) {
	if( !anId ) throw NotFoundError();
	TopologyType result;
	typesLock.lock_shared();
	if( anId - 1 < typesById.size() ) result = typesById[anId - 1];
	typesLock.unlock_shared();
	if( !result ) throw NotFoundError();
	return result;
}


const TopologyObject& RamTopology::registerObject( TopologyObjectId parent, TopologyTypeId objectType, TopologyTypeId relationType, const string& childName ) throw( IllegalStateError ) {
	assert(0 && "TODO"), abort();
}


const TopologyObject& RamTopology::lookupObjectByPath( const string& Path ) throw( NotFoundError ) {
	assert(0 && "TODO"), abort();
}


const TopologyObject& RamTopology::lookupObjectById( TopologyObject anId ) throw( NotFoundError ) {
	assert(0 && "TODO"), abort();
}


const TopologyRelation& RamTopology::registerRelation( TopologyTypeId relationType, TopologyObjectId parent, TopologyObjectId child, const string& childName ) throw( IllegalStateError ) {
	assert(0 && "TODO"), abort();
}


const TopologyRelation& RamTopology::lookupRelation( TopologyObjectId parent, const string& childName ) throw( NotFoundError ) {
	assert(0 && "TODO"), abort();
}


Topology::TopologyRelationList RamTopology::enumerateChildren( TopologyObjectId parent, TopologyTypeId relationType ) throw() {
	assert(0 && "TODO"), abort();
}


Topology::TopologyRelationList RamTopology::enumerateParents( TopologyObjectId child, TopologyTypeId relationType ) throw() {
	assert(0 && "TODO"), abort();
}


const TopologyAttribute RamTopology::registerAttribute( TopologyTypeId domain, const string& name, VariableDatatype::Type datatype ) throw( IllegalStateError ) {
	TopologyAttribute result;
	pair<TopologyTypeId, string> key( domain, name );
	attributesLock.lock_shared();
	IGNORE_EXCEPTIONS( result = attributesByName.at( key ); );
	attributesLock.unlock_shared();
	if( !result ) {
		Release<TopologyAttributeImplementation> newAttribute( new TopologyAttributeImplementation( name, domain, datatype ) );	//Don't let other processes wait for this.
		attributesLock.lock();
		IGNORE_EXCEPTIONS( result = attributesByName.at( key ); );	//Someone else might have registered it in the mean time.
		if( !result ) {
			attributesById.resize( newAttribute->id = attributesById.size() + 1 );
			attributesById.back().setObject( newAttribute );
			attributesByName[key].setObject( newAttribute );
			result.setObject( newAttribute );
		}
		attributesLock.unlock();
	}
	if( result.dataType() != datatype ) throw IllegalStateError( "RamTopology::registerAttribute(): requested attribute datatype does not match previously registered one." );
	return result;
}


const TopologyAttribute RamTopology::lookupAttributeByName( TopologyTypeId domain, const string& name ) throw( NotFoundError ) {
	TopologyAttribute result;
	pair<TopologyTypeId, string> key( domain, name );
	attributesLock.lock_shared();
	IGNORE_EXCEPTIONS( result = attributesByName.at( key ); );
	attributesLock.unlock_shared();
	if( !result ) throw NotFoundError();
	return result;
}


const TopologyAttribute RamTopology::lookupAttributeById( TopologyAttributeId attributeId ) throw( NotFoundError ) {
	if( !attributeId ) throw NotFoundError();
	TopologyAttribute result;
	attributesLock.lock_shared();
	if( attributeId - 1 < attributesById.size() ) result = attributesById[attributeId - 1];
	attributesLock.unlock_shared();
	if( !result ) throw NotFoundError();
	return result;
}


void RamTopology::setAttribute( TopologyObjectId object, TopologyAttributeId attribute, const TopologyValue& value ) throw( IllegalStateError ) {
	assert(0 && "TODO"), abort();
}


const TopologyValue& RamTopology::getAttribute( TopologyObjectId object, TopologyAttributeId attribute ) throw( NotFoundError ) {
	assert(0 && "TODO"), abort();
}


Topology::TopologyValueList RamTopology::enumerateAttributes( TopologyObjectId object ) throw() {
	assert(0 && "TODO"), abort();
}


extern "C" {
	void* TOPOLOGY_INSTANCIATOR_NAME()
	{
		return new RamTopology();
	}
}
